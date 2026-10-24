# Student Document Management System — bản nạp toàn bộ dữ liệu lên RAM

## Thay đổi so với bản trước

- `DbContext::loadAll()` đọc **tất cả** file TXT lên RAM **một lần** lúc khởi động (`main.cpp`), sau đó dựng quan hệ hai chiều giữa các đối tượng bằng con trỏ.
- `DbSet<T>` giữ `MyList<T*>` và là chủ sở hữu duy nhất của các đối tượng. Mọi truy vấn (`all`, `findByKey`, `findBy`...) chạy trên RAM, **không đọc lại file**.
- Service sửa thẳng đối tượng trên RAM rồi gọi `update()` / `add()` / `remove()` -> RAM đổi trước, sau đó ghi bảng đó về TXT.
- Chỗ phải reload từ file thì giữ nguyên: **Dashboard** vẫn đọc `CurrentServing.txt` mỗi 3 giây (`DashboardUI` chỉ gọi `readCurrent()`, không ghi đè file bằng RAM).
- `Entity` không còn sao chép được (mỗi đối tượng chỉ tồn tại một bản trên RAM); `Entity.cpp` bị xóa, mỗi entity tự cài `getKey()`.
- `Reference<T>` / `Collection<T>` chỉ giữ con trỏ (không sở hữu, không deep copy).

## Bản đồ quan hệ giữa các lớp

| Quan hệ | Loại | Hướng |
|---|---|---|
| `Nguoi` <- `SinhVien`, `Phong`, `Quay` | Inheritance | — |
| `Nguoi` — `TaiKhoan` | Association 1-1 | hai chiều (`taiKhoan` <-> `sinhVien`/`phong`/`quay`) |
| `Phong` ◆— `Quay` | Composition 1-N | hai chiều (`quays` <-> `phong`); xóa phòng -> xóa quầy |
| `Quay` ◆— `PhieuHangDoi` | Composition 1-N | hai chiều (`phieus` <-> `quay`); xóa quầy -> xóa phiếu |
| `SinhVien` ◆— `DangKy` | Composition 1-N | hai chiều (`dangKys` <-> `sinhVien`); xóa sinh viên -> xóa hồ sơ |
| `Phong` ◇— `VanBan` | Aggregation 1-N | hai chiều (`vanBans` <-> `phong`); xóa phòng, văn bản vẫn còn |
| `VanBan` — `DangKy` | Association 1-N | hai chiều (`dangKys` <-> `vanBan`) |
| `VanBan` — `PhieuHangDoi` | Association 1-N | hai chiều (`phieus` <-> `vanBan`) |
| `SinhVien` — `PhieuHangDoi` | Association 1-N | hai chiều (`phieus` <-> `sinhVien`) |

Lưu ý: dữ liệu chỉ được nạp một lần, nên chương trình giả định **một tiến trình** ghi dữ liệu. Cửa sổ Dashboard chạy riêng vẫn dùng được vì chỉ đọc `CurrentServing.txt`.


## main.cpp

```cpp
#include <iostream>
#include "business/AppContext.h"
#include "presentation/MainUI.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);      // hiển thị tiếng Việt UTF-8
    SetConsoleCP(65001);
#endif

    AppContext ctx("data/");

    if (!ctx.db.database().tableExists("TaiKhoan"))
    {
        cout << "Không tìm thấy data/TaiKhoan.txt\n";
        cout << "Hãy tạo thư mục data/ và các file dữ liệu mẫu (xem mục 7 của tài liệu).\n";
        return 1;
    }

    ctx.db.loadAll();       // nạp TOÀN BỘ dữ liệu lên RAM + dựng quan hệ giữa các đối tượng
    ctx.dash.refresh();     // đồng bộ CurrentServing.txt với trạng thái hàng đợi vừa nạp

    MainUI ui(ctx);
    ui.run();
    return 0;
}
```

## business\AppContext.cpp

```cpp
#include "AppContext.h"

AppContext::AppContext(const string& dataFolder)
    : db(dataFolder), auth(db), doc(db), reg(db), dash(db), queue(db, dash)
{
}
```

## business\AppContext.h

```h
#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include "../orm/DbContext.h"
#include "AuthService.h"
#include "DocumentService.h"
#include "RegistrationService.h"
#include "DashboardService.h"
#include "QueueService.h"

// Gom DbContext + các service để truyền cho tầng Presentation
// (thứ tự khai báo = thứ tự khởi tạo)
class AppContext
{
public:
    DbContext db;
    AuthService auth;
    DocumentService doc;
    RegistrationService reg;
    DashboardService dash;
    QueueService queue;

    AppContext(const string& dataFolder);
};

#endif
```

## business\AuthService.cpp

```cpp
#include "AuthService.h"

AuthService::AuthService(DbContext& d) : db(d) {}

// Trả về tài khoản trên RAM; từ đó đi tiếp qua navigation (sinhVien / phong / quay)
TaiKhoan* AuthService::login(const string& username, const string& password)
{
    TaiKhoan* t = db.taiKhoan.findByKey(username);       // username là khóa chính
    if (t == NULL || t->getPassword() != password) return NULL;
    return t;
}
```

## business\AuthService.h

```h
#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../orm/DbContext.h"

class AuthService
{
private:
    DbContext& db;

public:
    AuthService(DbContext& db);
    TaiKhoan* login(const string& username, const string& password);   // NULL nếu sai
};

#endif
```

## business\DashboardService.cpp

```cpp
#include "DashboardService.h"

DashboardService::DashboardService(DbContext& d) : db(d) {}

void DashboardService::refresh()
{
    const MyList<Quay*>& quays = db.quay.all();

    MyList<string> lines;
    for (int i = 0; i < quays.getSize(); i++)
    {
        Quay* q = quays[i];

        PhieuHangDoi* serving = q->servingTicket();             // phiếu đang phục vụ
        string so = (serving != NULL) ? serving->getDisplay() : string("---");

        MyList<PhieuHangDoi*> cho = q->waitingTickets();        // đã sắp theo số thứ tự
        string dsCho;                                           // "A001,A002,..."
        for (int j = 0; j < cho.getSize(); j++)
        {
            if (j > 0) dsCho += ",";
            dsCho += cho[j]->getDisplay();
        }

        lines.add(q->getMaQuay() + "|" + so + "|" +
                  (q->isActive() ? "1" : "0") + "|" + to_string(cho.getSize()) + "|" + dsCho);
    }
    db.database().writeLines("CurrentServing.txt", lines);
}

MyList<CounterDisplay> DashboardService::readCurrent()
{
    MyList<CounterDisplay> result;
    MyList<string> lines = db.database().readLines("CurrentServing.txt");
    for (int i = 0; i < lines.getSize(); i++)
    {
        MyList<string> f = split(lines[i], '|');
        if (f.getSize() < 3) continue;
        CounterDisplay c;
        c.maQuay    = f[0];
        c.soHienThi = f[1];
        c.active    = (f[2] != "0");

        // cột 5: danh sách số chờ, phân tách bằng dấu phẩy
        if (f.getSize() >= 5 && !f[4].empty())
        {
            MyList<string> parts = split(f[4], ',');
            for (int j = 0; j < parts.getSize(); j++)
            {
                string s = trim(parts[j]);
                if (!s.empty()) c.danhSachCho.add(s);
            }
        }
        result.add(c);
    }
    return result;
}
```

## business\DashboardService.h

```h
#ifndef DASHBOARDSERVICE_H
#define DASHBOARDSERVICE_H

#include "../orm/DbContext.h"
#include "../models/Constants.h"

struct CounterDisplay
{
    string maQuay;
    string soHienThi;
    bool active;
    MyList<string> danhSachCho;
    CounterDisplay() : active(true) {}
};

// Dashboard công khai: ghi / đọc data/CurrentServing.txt
class DashboardService
{
private:
    DbContext& db;

public:
    DashboardService(DbContext& db);

    // Dựng lại CurrentServing.txt từ các phiếu trên RAM.
    // Được gọi mỗi khi hàng đợi thay đổi (lấy số, gọi số) và một lần lúc khởi động.
    void refresh();

    // Đọc CurrentServing.txt TỪ FILE (không dùng RAM): dashboard luôn reload từ file
    // mỗi 3 giây nên cửa sổ dashboard chạy riêng vẫn thấy dữ liệu mới.
    MyList<CounterDisplay> readCurrent();
};

#endif
```

## business\DocumentService.cpp

```cpp
#include "DocumentService.h"

DocumentService::DocumentService(DbContext& d) : db(d) {}

MyList<Phong*> DocumentService::getAllPhong()
{
    return db.phong.all();                   // danh sách con trỏ trên RAM
}

// Navigation 1-N: Phong -> các VanBan của phòng
MyList<VanBan*> DocumentService::getVanBanByPhong(const string& maPHG)
{
    MyList<VanBan*> result;
    Phong* p = db.phong.findByKey(maPHG);
    if (p != NULL) result = p->vanBans.items();
    return result;
}
```

## business\DocumentService.h

```h
#ifndef DOCUMENTSERVICE_H
#define DOCUMENTSERVICE_H

#include "../orm/DbContext.h"

class DocumentService
{
private:
    DbContext& db;

public:
    DocumentService(DbContext& db);

    MyList<Phong*> getAllPhong();
    MyList<VanBan*> getVanBanByPhong(const string& maPHG);
};

#endif
```

## business\QueueService.cpp

```cpp
#include "QueueService.h"
#include "../templates/MyQueue.h"

QueueService::QueueService(DbContext& d, DashboardService& ds) : db(d), dash(ds) {}

bool QueueService::canTakeTicket(const string& maSV, string& msg)
{
    SinhVien* sv = db.sinhVien.findByKey(maSV);
    if (sv == NULL) { msg = "MSSV không tồn tại."; return false; }

    const MyList<PhieuHangDoi*>& mine = sv->phieus.items();    // Navigation 1-N: phiếu của sinh viên
    for (int i = 0; i < mine.getSize(); i++)
    {
        if (mine[i]->getTrangThai() == Q_WAITING)
        {
            msg = "Bạn đang có số " + mine[i]->getDisplay() + " chờ tại quầy " + mine[i]->getMaQuay() + ".";
            return false;
        }
    }
    return true;
}

// UC07 + UC08: chọn phòng + nội dung (giấy tờ / tư vấn) -> gán vào quầy đang hoạt động
// của PHÒNG đó có ít người chờ nhất
bool QueueService::takeTicket(const string& maSV, const string& maPHG, const string& maVB,
                              PhieuHangDoi*& out, string& msg)
{
    out = NULL;
    if (!canTakeTicket(maSV, msg)) return false;

    Phong* ph = db.phong.findByKey(maPHG);
    if (ph == NULL) { msg = "Phòng không tồn tại."; return false; }

    // Nội dung: "Tư vấn" hoặc một văn bản thuộc đúng phòng đã chọn
    string request = REQ_CONSULT;
    if (maVB != REQ_CONSULT)
    {
        VanBan* vb = db.vanBan.findByKey(maVB);
        if (vb == NULL || vb->getMaPHG() != maPHG)
        {
            msg = "Giấy tờ không thuộc phòng đã chọn.";
            return false;
        }
        request = maVB;
    }

    // Chỉ xét các quầy của phòng này đang hoạt động (Navigation 1-N: Phong -> Quay)
    const MyList<Quay*>& all = ph->quays.items();
    Quay* best = NULL;
    for (int i = 0; i < all.getSize(); i++)
    {
        if (!all[i]->isActive()) continue;
        if (best == NULL || all[i]->countWaiting() < best->countWaiting()) best = all[i];
    }

    if (best == NULL)
    {
        msg = "Hiện " + ph->getTenPhong() + " không có quầy nào đang hoạt động.";
        return false;
    }

    string id = db.phieu.nextId("T", 4);                      // T0001, T0002, ...
    PhieuHangDoi* p = new PhieuHangDoi(id, toInt(id.substr(1)), maSV, best->getMaQuay(), Q_WAITING, request);
    db.phieu.add(p);                                          // thêm vào RAM + liên kết + ghi file

    out = p;
    dash.refresh();
    return true;
}

// UC10: quầy gọi số tiếp theo (WAITING -> SERVING); phiếu SERVING trước đó -> FINISHED
// Nếu không còn ai chờ: kết thúc luôn phiếu đang SERVING hiện tại (nếu có) và báo trống.
bool QueueService::callNext(Quay* quay, PhieuHangDoi*& out, string& msg)
{
    out = NULL;
    const string maQuay = quay->getMaQuay();

    // Phiếu WAITING vào hàng đợi theo thứ tự số thứ tự tăng dần
    MyList<PhieuHangDoi*> waiting = quay->waitingTickets();
    MyQueue<PhieuHangDoi*> q;                                 // hàng đợi của quầy
    for (int i = 0; i < waiting.getSize(); i++) q.enqueue(waiting[i]);

    // Phiếu SERVING cũ chuyển FINISHED (sửa trên RAM)
    bool hadServing = false;
    const MyList<PhieuHangDoi*>& list = quay->phieus.items();
    for (int i = 0; i < list.getSize(); i++)
    {
        if (list[i]->getTrangThai() == Q_SERVING)
        {
            list[i]->setTrangThai(Q_FINISHED);
            hadServing = true;
        }
    }

    // Trường hợp 1: không còn ai chờ
    if (q.isEmpty())
    {
        if (hadServing) db.phieu.save();                      // ghi các phiếu vừa kết thúc về file
        dash.refresh();
        msg = hadServing
            ? "Hàng đợi của quầy " + maQuay + " đã hết. Đã kết thúc phiếu đang phục vụ."
            : "Hàng đợi của quầy " + maQuay + " đang trống.";
        return false;
    }

    // Trường hợp 2: có người chờ -> gọi số tiếp theo
    PhieuHangDoi* next = q.peek();
    q.dequeue();
    next->setTrangThai(Q_SERVING);
    db.phieu.update(next);                                    // ghi cả thay đổi FINISHED lẫn SERVING (1 lần ghi file)

    dash.refresh();
    out = next;
    return true;
}
```

## business\QueueService.h

```h
#ifndef QUEUESERVICE_H
#define QUEUESERVICE_H

#include "../orm/DbContext.h"
#include "../models/Constants.h"
#include "DashboardService.h"

// Hàng đợi nhiều quầy + chia tải tự động (theo từng phòng). Mọi thao tác xử lý trên RAM.
class QueueService
{
private:
    DbContext& db;
    DashboardService& dash;

public:
    QueueService(DbContext& db, DashboardService& dash);

    // Kiểm tra sinh viên có được lấy số không (tồn tại, chưa có số đang chờ)
    bool canTakeTicket(const string& maSV, string& msg);

    // UC07 + UC08: lấy số theo PHÒNG và NỘI DUNG (maVB = REQ_CONSULT nếu chọn "Tư vấn")
    // Phiếu được gán vào quầy ĐANG HOẠT ĐỘNG của phòng đó có ít người chờ nhất
    bool takeTicket(const string& maSV, const string& maPHG, const string& maVB,
                    PhieuHangDoi*& out, string& msg);

    // UC10: quầy gọi số tiếp theo
    bool callNext(Quay* quay, PhieuHangDoi*& out, string& msg);
};

#endif
```

## business\RegistrationService.cpp

```cpp
#include "RegistrationService.h"

RegistrationService::RegistrationService(DbContext& d) : db(d) {}

// UC01: Đăng ký giấy tờ -> tạo hồ sơ PENDING
bool RegistrationService::registerDocument(const string& maSV, const string& maVB, string& msg)
{
    SinhVien* sv = db.sinhVien.findByKey(maSV);
    if (sv == NULL) { msg = "Sinh viên không tồn tại."; return false; }

    VanBan* vb = db.vanBan.findByKey(maVB);
    if (vb == NULL) { msg = "Văn bản không tồn tại."; return false; }

    const MyList<DangKy*>& mine = sv->dangKys.items();   // Navigation 1-N: hồ sơ của sinh viên
    for (int i = 0; i < mine.getSize(); i++)
    {
        if (mine[i]->getMaVB() == maVB &&
            (mine[i]->getTrangThai() == ST_PENDING || mine[i]->getTrangThai() == ST_APPROVED))
        {
            msg = "Bạn đã có hồ sơ " + mine[i]->getMaDK() + " cho văn bản này đang được xử lý.";
            return false;
        }
    }

    string id = db.dangKy.nextId("DK", 3);
    DangKy* dk = new DangKy(id, maSV, maVB, ST_PENDING, "", nowDateTime(), "", "");
    db.dangKy.add(dk);                                   // thêm vào RAM + liên kết hai chiều + ghi file
    msg = "Đăng ký thành công. Mã đơn: " + id;
    return true;
}

// UC02: Hồ sơ của một sinh viên (Navigation 1-N: SinhVien -> DangKy)
MyList<DangKy*> RegistrationService::getBySinhVien(const string& maSV)
{
    MyList<DangKy*> result;
    SinhVien* sv = db.sinhVien.findByKey(maSV);
    if (sv != NULL) result = sv->dangKys.items();
    return result;
}

// Lọc hồ sơ theo trạng thái và theo phòng (đi qua Navigation N-1: DangKy -> VanBan).
MyList<DangKy*> RegistrationService::collect(const string& maPHG, const string& status, int mode)
{
    const MyList<DangKy*>& list = db.dangKy.all();
    MyList<DangKy*> result;

    for (int i = 0; i < list.getSize(); i++)
    {
        DangKy* dk = list[i];
        if (dk->getTrangThai() != status) continue;

        VanBan* vb = dk->vanBan.get();                   // Navigation N-1
        if (vb == NULL || vb->getMaPHG() != maPHG) continue;

        bool coIn = !dk->getNgayIn().empty();
        bool coNhan = !dk->getNgayNhan().empty();

        if (mode == 1 && coIn) continue;
        if (mode == 2 && !coIn) continue;
        if (mode == 3 && (!coIn || coNhan)) continue;
        result.add(dk);
    }
    return result;
}

MyList<DangKy*> RegistrationService::getPending(const string& maPHG)    { return collect(maPHG, ST_PENDING, 0); }
MyList<DangKy*> RegistrationService::getForPrint(const string& maPHG)   { return collect(maPHG, ST_APPROVED, 1); }
MyList<DangKy*> RegistrationService::getForReceive(const string& maPHG) { return collect(maPHG, ST_APPROVED, 3); }

DangKy* RegistrationService::findPending(const string& maDK, string& msg)
{
    DangKy* dk = db.dangKy.findByKey(maDK);
    if (dk == NULL) { msg = "Không tìm thấy đơn " + maDK + "."; return NULL; }
    if (dk->getTrangThai() != ST_PENDING)
    {
        msg = "Đơn " + maDK + " không ở trạng thái PENDING.";
        return NULL;
    }
    return dk;
}

// UC03: Duyệt / từ chối (sửa đối tượng trên RAM rồi update -> ghi file)
bool RegistrationService::approve(const string& maDK, string& msg)
{
    DangKy* dk = findPending(maDK, msg);
    if (dk == NULL) return false;
    dk->setTrangThai(ST_APPROVED);
    db.dangKy.update(dk);
    msg = "Đã duyệt đơn " + maDK + ".";
    return true;
}

bool RegistrationService::reject(const string& maDK, const string& reason, string& msg)
{
    DangKy* dk = findPending(maDK, msg);
    if (dk == NULL) return false;
    dk->setTrangThai(ST_REJECTED);
    dk->setLyDo(reason);
    db.dangKy.update(dk);
    msg = "Đã từ chối đơn " + maDK + ".";
    return true;
}

// UC04: Xuất danh sách cần in ra data/PrintList.txt. Trả về số hồ sơ, -1 nếu lỗi ghi file
int RegistrationService::exportPrintList(const string& maPHG)
{
    MyList<DangKy*> ds = getForPrint(maPHG);
    if (ds.isEmpty()) return 0;

    MyList<string> lines;
    lines.add("DANH SACH CAN IN - " + nowDateTime());
    lines.add("STT | MaDon | MSSV | TenVanBan | NgayDangKy");
    for (int i = 0; i < ds.getSize(); i++)
    {
        lines.add(to_string(i + 1) + " | " + ds[i]->getMaDK() + " | " + ds[i]->getMaSV() + " | " +
                  ds[i]->tenVanBan() + " | " + ds[i]->getNgayDK());
    }
    return db.database().writeLines("PrintList.txt", lines) ? ds.getSize() : -1;
}

// UC05: Cập nhật ngày in = thời điểm bấm cập nhật (dd/mm/yyyy HH:MM:SS)
bool RegistrationService::setNgayIn(const string& maDK, const string& thoiGian, string& msg)
{
    DangKy* dk = db.dangKy.findByKey(maDK);
    if (dk == NULL) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
    if (dk->getTrangThai() != ST_APPROVED) { msg = "Đơn " + maDK + " chưa được duyệt (APPROVED)."; return false; }
    if (!dk->getNgayIn().empty()) { msg = "Đơn " + maDK + " đã có ngày in (" + dk->getNgayIn() + ")."; return false; }

    dk->setNgayIn(thoiGian);
    db.dangKy.update(dk);
    msg = "Đã cập nhật ngày in cho đơn " + maDK + ": " + thoiGian;
    return true;
}

// UC06: Cập nhật ngày nhận = thời điểm bấm cập nhật (không được trước ngày in)
bool RegistrationService::setNgayNhan(const string& maDK, const string& thoiGian, string& msg)
{
    DangKy* dk = db.dangKy.findByKey(maDK);
    if (dk == NULL) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
    if (dk->getTrangThai() != ST_APPROVED) { msg = "Đơn " + maDK + " chưa được duyệt (APPROVED)."; return false; }
    if (dk->getNgayIn().empty()) { msg = "Đơn " + maDK + " chưa có ngày in."; return false; }
    if (!dk->getNgayNhan().empty()) { msg = "Đơn " + maDK + " đã có ngày nhận (" + dk->getNgayNhan() + ")."; return false; }
    if (dateTimeToSeconds(thoiGian) < dateTimeToSeconds(dk->getNgayIn()))
    {
        msg = "Ngày nhận không được trước ngày in (" + dk->getNgayIn() + ").";
        return false;
    }

    dk->setNgayNhan(thoiGian);
    db.dangKy.update(dk);
    msg = "Đã cập nhật ngày nhận cho đơn " + maDK + ": " + thoiGian;
    return true;
}

// UC12: các giấy của sinh viên cần xử lý trả tại phòng maPHG
// (đã duyệt, đã có ngày nhận, chưa trả, thuộc phòng của quầy)
MyList<DangKy*> RegistrationService::getForReturn(const string& maSV, const string& maPHG)
{
    MyList<DangKy*> result;
    SinhVien* sv = db.sinhVien.findByKey(maSV);
    if (sv == NULL) return result;

    const MyList<DangKy*>& list = sv->dangKys.items();   // Navigation 1-N
    for (int i = 0; i < list.getSize(); i++)
    {
        DangKy* dk = list[i];
        if (dk->getTrangThai() != ST_APPROVED) continue;
        if (dk->getNgayNhan().empty()) continue;
        VanBan* vb = dk->vanBan.get();                   // Navigation N-1
        if (vb == NULL || vb->getMaPHG() != maPHG) continue;
        result.add(dk);
    }
    return result;
}

// UC12: Xác nhận trả giấy. Điều kiện: ngày nhận <= hôm nay <= ngày nhận + 30 ngày
// (so sánh theo ngày, phần giờ không ảnh hưởng)
bool RegistrationService::confirmReceive(const string& maDK, const string& maSVDangPhucVu, string& msg)
{
    DangKy* dk = db.dangKy.findByKey(maDK);
    if (dk == NULL) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
    if (dk->getMaSV() != maSVDangPhucVu) { msg = "Đơn này không thuộc sinh viên đang được phục vụ."; return false; }
    if (dk->getTrangThai() != ST_APPROVED)
    {
        msg = "Đơn chưa ở trạng thái APPROVED (hiện tại: " + dk->getTrangThai() + ").";
        return false;
    }
    if (dk->getNgayNhan().empty()) { msg = "Đơn chưa có ngày nhận."; return false; }

    long t = dateToDays(today());
    long n = dateToDays(dk->getNgayNhan());
    if (t < n) { msg = "Chưa đến ngày nhận (" + dk->getNgayNhan() + ")."; return false; }
    if (t > n + 30) { msg = "Đã quá hạn nhận giấy (hạn: ngày nhận + 30 ngày)."; return false; }

    dk->setTrangThai(ST_DONE);
    db.dangKy.update(dk);
    msg = "Đã nhận thành công. Đơn " + maDK + " chuyển sang DONE.";
    return true;
}
```

## business\RegistrationService.h

```h
#ifndef REGISTRATIONSERVICE_H
#define REGISTRATIONSERVICE_H

#include "../orm/DbContext.h"
#include "../models/Constants.h"

// Nghiệp vụ đăng ký, duyệt, in, trả giấy tờ (xử lý trên RAM, mỗi thay đổi ghi về TXT)
// Mọi mốc thời gian lưu theo định dạng dd/mm/yyyy HH:MM:SS
class RegistrationService
{
private:
    DbContext& db;

    // mode: 0 = tất cả, 1 = chưa có ngày in, 2 = đã có ngày in,
    //       3 = đã có ngày in nhưng chưa có ngày nhận
    MyList<DangKy*> collect(const string& maPHG, const string& status, int mode);
    DangKy* findPending(const string& maDK, string& msg);

public:
    RegistrationService(DbContext& db);

    // UC01, UC02
    bool registerDocument(const string& maSV, const string& maVB, string& msg);
    MyList<DangKy*> getBySinhVien(const string& maSV);

    // UC03
    MyList<DangKy*> getPending(const string& maPHG);
    bool approve(const string& maDK, string& msg);
    bool reject(const string& maDK, const string& reason, string& msg);

    // UC04, UC05, UC06  (thoiGian = thời điểm bấm cập nhật, do tầng trên truyền vào)
    MyList<DangKy*> getForPrint(const string& maPHG);
    MyList<DangKy*> getForReceive(const string& maPHG);
    int exportPrintList(const string& maPHG);
    bool setNgayIn(const string& maDK, const string& thoiGian, string& msg);
    bool setNgayNhan(const string& maDK, const string& thoiGian, string& msg);

    // UC12: danh sách giấy của sinh viên cần xử lý trả tại một phòng
    MyList<DangKy*> getForReturn(const string& maSV, const string& maPHG);
    bool confirmReceive(const string& maDK, const string& maSVDangPhucVu, string& msg);
};

#endif
```

## data\CurrentServing.txt

```txt
Q01|---|1|1|A005
Q02|---|1|0|
Q03|---|1|0|
```

## data\DangKy.txt

```txt
DK001|2212345|VB001|DONE||22/10/2026 20:52:30|22/10/2026 20:58:16|22/10/2026 20:58:24
DK002|2212345|VB001|PENDING||22/10/2026 21:45:09||
DK003|2212345|VB002|PENDING||22/10/2026 22:07:38||
```

## data\PhieuHangDoi.txt

```txt
T0001|1|2212345|Q01|FINISHED|TUVAN
T0002|2|2212345|Q01|FINISHED|VB001
T0003|3|2212346|Q03|FINISHED|TUVAN
T0004|4|2212345|Q01|FINISHED|TUVAN
T0005|5|2212345|Q01|WAITING|VB001
T0006|6|2212346|Q03|FINISHED|VB005
```

## data\Phong.txt

```txt
P01|Phòng Công tác Sinh viên|ctsv001
P02|Phòng Công tác Sinh viên 2|ctsv002
P03|Phòng Đào tạo|pdt001
```

## data\PrintList.txt

```txt
DANH SACH CAN IN - NGAY 22/10/2026
STT | MaDon | MSSV | TenVanBan | NgayDangKy
1 | DK003 | 2212345 | Giấy xác nhận sinh viên | 22/10/2026
```

## data\Quay.txt

```txt
Q01|P01|q01|1
Q02|P01|q02|1
Q03|P03|q03|1
```

## data\SinhVien.txt

```txt
2212345|sv001
2212346|sv002
2212347|sv003
```

## data\TaiKhoan.txt

```txt
sv001|123|SINHVIEN
sv002|123|SINHVIEN
sv003|123|SINHVIEN
ctsv001|123|PHONG
ctsv002|123|PHONG
pdt001|123|PHONG
q01|123|QUAY
q02|123|QUAY
q03|123|QUAY
```

## data\VanBan.txt

```txt
VB001|Giấy xác nhận sinh viên|P01
VB002|Giấy xác nhận vay vốn ngân hàng|P01
VB003|Giấy xác nhận tạm hoãn nghĩa vụ quân sự|P01
VB004|Bảng điểm học kỳ|P03
VB005|Giấy xác nhận tốt nghiệp tạm thời|P03
VB006|Giấy xác nhận đăng ký môn học|P03
```

## models\Constants.h

```h
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
using namespace std;

const string ROLE_STUDENT = "SINHVIEN";
const string ROLE_PHONG   = "PHONG";
const string ROLE_QUAY    = "QUAY";

const string ST_PENDING  = "PENDING";
const string ST_APPROVED = "APPROVED";
const string ST_REJECTED = "REJECTED";
const string ST_DONE     = "DONE";

// Yêu cầu của phiếu hàng đợi: "TUVAN" hoặc mã văn bản (VBxxx)
const string REQ_CONSULT = "TUVAN";

const string Q_WAITING  = "WAITING";
const string Q_SERVING  = "SERVING";
const string Q_FINISHED = "FINISHED";

#endif
```

## models\DangKy.cpp

```cpp
#include "DangKy.h"
#include "../orm/DbContext.h"

DangKy::DangKy() {}

DangKy::DangKy(const string& dk, const string& sv, const string& vb, const string& tt,
               const string& lydo, const string& ndk, const string& ni, const string& nn)
    : maDK(dk), maSV(sv), maVB(vb), trangThai(tt), lyDoTuChoi(lydo),
      ngayDangKy(ndk), ngayIn(ni), ngayNhan(nn) {}

// Đi qua Navigation Property: không cần tự truy vấn bảng VanBan
string DangKy::tenVanBan() const
{
    VanBan* vb = vanBan.get();
    if (vb != NULL) return vb->getTenVB();
    return maVB;                                         // không tìm thấy văn bản
}

string DangKy::tableName() const { return "DangKy"; }

Row DangKy::columns() const
{
    return split("maDK|maSV|maVB|trangThai|lyDoTuChoi|ngayDangKy|ngayIn|ngayNhan", '|');
}

Row DangKy::toRow() const
{
    Row r;
    r.add(maDK);
    r.add(maSV);
    r.add(maVB);
    r.add(trangThai);
    r.add(lyDoTuChoi);
    r.add(ngayDangKy);
    r.add(ngayIn);
    r.add(ngayNhan);
    return r;
}

void DangKy::fromRow(const Row& r)
{
    maDK = r[0];
    maSV = r[1];
    maVB = r[2];
    trangThai = r[3];
    lyDoTuChoi = r[4];
    ngayDangKy = r[5];
    ngayIn = r[6];
    ngayNhan = r[7];
}

void DangKy::linkRelations(DbContext* ctx)
{
    SinhVien* sv = ctx->sinhVien.findByKey(maSV);        // N-1
    if (sv != NULL)
    {
        sinhVien.set(sv);
        sv->dangKys.add(this);                           // chiều ngược lại: SinhVien.dangKys
    }

    VanBan* vb = ctx->vanBan.findByKey(maVB);            // N-1
    if (vb != NULL)
    {
        vanBan.set(vb);
        vb->dangKys.add(this);                           // chiều ngược lại: VanBan.dangKys
    }
}

void DangKy::unlinkRelations()
{
    if (sinhVien.hasValue()) sinhVien->dangKys.remove(this);
    if (vanBan.hasValue())   vanBan->dangKys.remove(this);
    sinhVien.clear();
    vanBan.clear();
}
```

## models\DangKy.h

```h
#ifndef DANGKY_H
#define DANGKY_H

#include "../orm/Entity.h"
#include "../orm/Navigation.h"

class SinhVien;
class VanBan;

class DangKy : public Entity
{
private:
    string maDK;
    string maSV;
    string maVB;
    string trangThai;
    string lyDoTuChoi;
    string ngayDangKy;
    string ngayIn;
    string ngayNhan;

public:
    // ----- Quan hệ (hai chiều) -----
    Reference<SinhVien> sinhVien;          // COMPOSITION (phía con): hồ sơ của sinh viên nào
    Reference<VanBan>   vanBan;            // ASSOCIATION: đăng ký văn bản nào

    DangKy();
    DangKy(const string& maDK, const string& maSV, const string& maVB, const string& trangThai,
           const string& lyDoTuChoi, const string& ngayDangKy, const string& ngayIn, const string& ngayNhan);

    string getMaDK() const { return maDK; }
    string getMaSV() const { return maSV; }
    string getMaVB() const { return maVB; }
    string getTrangThai() const { return trangThai; }
    string getLyDo() const { return lyDoTuChoi; }
    string getNgayDK() const { return ngayDangKy; }
    string getNgayIn() const { return ngayIn; }
    string getNgayNhan() const { return ngayNhan; }

    void setTrangThai(const string& v) { trangThai = v; }
    void setLyDo(const string& v) { lyDoTuChoi = v; }
    void setNgayIn(const string& v) { ngayIn = v; }
    void setNgayNhan(const string& v) { ngayNhan = v; }

    string tenVanBan() const;       // tên văn bản, lấy qua navigation vanBan

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    string getKey() const { return maDK; }
    void linkRelations(DbContext* ctx);
    void unlinkRelations();
};

#endif
```

## models\Nguoi.h

```h
#ifndef NGUOI_H
#define NGUOI_H

#include "../orm/Entity.h"
#include "../orm/Navigation.h"

class TaiKhoan;

// INHERITANCE: lớp cha của SinhVien, Phong (NhanVienPhong), Quay (NhanVienQuay)
// ASSOCIATION 1-1 hai chiều: Nguoi <-> TaiKhoan (TaiKhoan cũng trỏ ngược lại)
class Nguoi : public Entity
{
protected:
    string username;

public:
    Reference<TaiKhoan> taiKhoan;          // tài khoản đăng nhập của người này

    Nguoi() {}
    Nguoi(const string& u) : username(u) {}
    string getUsername() const { return username; }
};

#endif
```

## models\PhieuHangDoi.cpp

```cpp
#include "PhieuHangDoi.h"
#include "Constants.h"
#include "../orm/DbContext.h"

PhieuHangDoi::PhieuHangDoi() : soThuTu(0) {}

PhieuHangDoi::PhieuHangDoi(const string& p, int stt, const string& sv, const string& q,
                           const string& tt, const string& vb)
    : maPhieu(p), soThuTu(stt), maSV(sv), maQuay(q), trangThai(tt), maVB(vb) {}

string PhieuHangDoi::getDisplay() const { return "A" + pad(soThuTu, 3); }

// Dòng cũ chưa có cột maVB (rỗng) được xem là tư vấn
bool PhieuHangDoi::isConsult() const { return maVB.empty() || maVB == REQ_CONSULT; }

string PhieuHangDoi::tenYeuCau() const
{
    if (isConsult()) return "Tư vấn";
    VanBan* vb = vanBan.get();
    if (vb != NULL) return vb->getTenVB();
    return maVB;                                         // không tìm thấy văn bản
}

string PhieuHangDoi::tableName() const { return "PhieuHangDoi"; }

Row PhieuHangDoi::columns() const { return split("maPhieu|soThuTu|maSV|maQuay|trangThai|maVB", '|'); }

Row PhieuHangDoi::toRow() const
{
    Row r;
    r.add(maPhieu);
    r.add(to_string(soThuTu));
    r.add(maSV);
    r.add(maQuay);
    r.add(trangThai);
    r.add(maVB);
    return r;
}

void PhieuHangDoi::fromRow(const Row& r)
{
    maPhieu = r[0];
    soThuTu = toInt(r[1]);
    maSV = r[2];
    maQuay = r[3];
    trangThai = r[4];
    maVB = r[5];
}

void PhieuHangDoi::linkRelations(DbContext* ctx)
{
    SinhVien* sv = ctx->sinhVien.findByKey(maSV);        // N-1
    if (sv != NULL)
    {
        sinhVien.set(sv);
        sv->phieus.add(this);                            // chiều ngược lại: SinhVien.phieus
    }

    Quay* q = ctx->quay.findByKey(maQuay);               // N-1
    if (q != NULL)
    {
        quay.set(q);
        q->phieus.add(this);                             // chiều ngược lại: Quay.phieus
    }

    if (!isConsult())                                    // tư vấn thì không có văn bản
    {
        VanBan* vb = ctx->vanBan.findByKey(maVB);        // N-1
        if (vb != NULL)
        {
            vanBan.set(vb);
            vb->phieus.add(this);                        // chiều ngược lại: VanBan.phieus
        }
    }
}

void PhieuHangDoi::unlinkRelations()
{
    if (sinhVien.hasValue()) sinhVien->phieus.remove(this);
    if (quay.hasValue())     quay->phieus.remove(this);
    if (vanBan.hasValue())   vanBan->phieus.remove(this);
    sinhVien.clear();
    quay.clear();
    vanBan.clear();
}
```

## models\PhieuHangDoi.h

```h
#ifndef PHIEUHANGDOI_H
#define PHIEUHANGDOI_H

#include "../orm/Entity.h"
#include "../orm/Navigation.h"

class SinhVien;
class Quay;
class VanBan;

class PhieuHangDoi : public Entity
{
private:
    string maPhieu;
    int soThuTu;
    string maSV;
    string maQuay;
    string trangThai;
    string maVB;                           // yêu cầu: "TUVAN" hoặc mã văn bản cần giải quyết

public:
    // ----- Quan hệ (hai chiều) -----
    Reference<SinhVien> sinhVien;          // ASSOCIATION: phiếu của sinh viên nào
    Reference<Quay>     quay;              // COMPOSITION (phía con): phiếu thuộc quầy nào
    Reference<VanBan>   vanBan;            // ASSOCIATION: văn bản cần giải quyết (NULL nếu là tư vấn)

    PhieuHangDoi();
    PhieuHangDoi(const string& maPhieu, int soThuTu, const string& maSV,
                 const string& maQuay, const string& trangThai, const string& maVB);

    string getMaPhieu() const { return maPhieu; }
    int getSoThuTu() const { return soThuTu; }
    string getMaSV() const { return maSV; }
    string getMaQuay() const { return maQuay; }
    string getTrangThai() const { return trangThai; }
    string getMaVB() const { return maVB; }
    void setTrangThai(const string& v) { trangThai = v; }

    string getDisplay() const;      // số hiển thị trên vé, ví dụ A015
    bool isConsult() const;         // true nếu là phiếu tư vấn
    string tenYeuCau() const;       // "Tư vấn" hoặc tên văn bản (qua navigation vanBan)

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    string getKey() const { return maPhieu; }
    void linkRelations(DbContext* ctx);
    void unlinkRelations();
};

#endif
```

## models\Phong.cpp

```cpp
#include "Phong.h"
#include "../orm/DbContext.h"

Phong::Phong() {}

Phong::Phong(const string& ma, const string& ten, const string& u)
    : Nguoi(u), maPHG(ma), tenPhong(ten) {}

string Phong::tableName() const { return "Phong"; }

Row Phong::columns() const { return split("maPHG|tenPhong|username", '|'); }

Row Phong::toRow() const
{
    Row r;
    r.add(maPHG);
    r.add(tenPhong);
    r.add(username);
    return r;
}

void Phong::fromRow(const Row& r)
{
    maPHG = r[0];
    tenPhong = r[1];
    username = r[2];
}

void Phong::linkRelations(DbContext* ctx)
{
    TaiKhoan* t = ctx->taiKhoan.findByKey(username);     // 1-1
    if (t != NULL)
    {
        taiKhoan.set(t);
        t->phong.set(this);                              // chiều ngược lại
    }
    // vanBans, quays được VanBan / Quay tự thêm vào khi chúng liên kết
}

void Phong::unlinkRelations()
{
    if (taiKhoan.hasValue()) taiKhoan->phong.clear();
    taiKhoan.clear();

    // Aggregation: văn bản vẫn tồn tại nhưng không còn thuộc phòng nào
    for (int i = 0; i < vanBans.count(); i++) vanBans.items()[i]->phong.clear();
    vanBans.clear();
}

// Composition: quầy không tồn tại độc lập khỏi phòng
void Phong::removeChildren(DbContext* ctx)
{
    MyList<Quay*> copy = quays.items();
    for (int i = 0; i < copy.getSize(); i++) ctx->quay.remove(copy[i]->getKey());
}
```

## models\Phong.h

```h
#ifndef PHONG_H
#define PHONG_H

#include "Nguoi.h"

class VanBan;
class Quay;

// Phòng ban (đồng thời là nhân viên phòng đăng nhập bằng username)
class Phong : public Nguoi
{
private:
    string maPHG;
    string tenPhong;

public:
    // ----- Quan hệ (hai chiều) -----
    // (taiKhoan: ASSOCIATION 1-1, kế thừa từ Nguoi)
    Collection<VanBan> vanBans;            // AGGREGATION 1-N: văn bản phòng cấp (xóa phòng, văn bản vẫn còn)
    Collection<Quay>   quays;              // COMPOSITION 1-N: quầy thuộc phòng (xóa phòng -> xóa quầy)

    Phong();
    Phong(const string& maPHG, const string& tenPhong, const string& username);

    string getMaPHG() const { return maPHG; }
    string getTenPhong() const { return tenPhong; }

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    string getKey() const { return maPHG; }
    void linkRelations(DbContext* ctx);
    void unlinkRelations();
    void removeChildren(DbContext* ctx);
};

typedef Phong NhanVienPhong;

#endif
```

## models\Quay.cpp

```cpp
#include "Quay.h"
#include "Constants.h"
#include "../orm/DbContext.h"

Quay::Quay() : active(true) {}

Quay::Quay(const string& mq, const string& mp, const string& u, bool a)
    : Nguoi(u), maQuay(mq), maPHG(mp), active(a) {}

MyList<PhieuHangDoi*> Quay::waitingTickets() const
{
    MyList<PhieuHangDoi*> cho;
    const MyList<PhieuHangDoi*>& all = phieus.items();
    for (int i = 0; i < all.getSize(); i++)
        if (all[i]->getTrangThai() == Q_WAITING) cho.add(all[i]);

    // thứ tự gọi thực tế là theo số thứ tự (sắp xếp nổi bọt)
    for (int a = 0; a < cho.getSize(); a++)
        for (int b = a + 1; b < cho.getSize(); b++)
            if (cho[b]->getSoThuTu() < cho[a]->getSoThuTu())
            {
                PhieuHangDoi* tmp = cho[a]; cho[a] = cho[b]; cho[b] = tmp;
            }
    return cho;
}

PhieuHangDoi* Quay::servingTicket() const
{
    const MyList<PhieuHangDoi*>& all = phieus.items();
    for (int i = 0; i < all.getSize(); i++)
        if (all[i]->getTrangThai() == Q_SERVING) return all[i];
    return NULL;
}

int Quay::countWaiting() const
{
    int c = 0;
    const MyList<PhieuHangDoi*>& all = phieus.items();
    for (int i = 0; i < all.getSize(); i++)
        if (all[i]->getTrangThai() == Q_WAITING) c++;
    return c;
}

string Quay::tableName() const { return "Quay"; }

Row Quay::columns() const { return split("maQuay|maPHG|username|active", '|'); }

Row Quay::toRow() const
{
    Row r;
    r.add(maQuay);
    r.add(maPHG);
    r.add(username);
    r.add(active ? "1" : "0");
    return r;
}

void Quay::fromRow(const Row& r)
{
    maQuay = r[0];
    maPHG = r[1];
    username = r[2];
    active = (r[3] != "0");
}

void Quay::linkRelations(DbContext* ctx)
{
    TaiKhoan* t = ctx->taiKhoan.findByKey(username);     // 1-1
    if (t != NULL)
    {
        taiKhoan.set(t);
        t->quay.set(this);
    }

    Phong* p = ctx->phong.findByKey(maPHG);              // N-1: Quay.maPHG -> Phong (khóa chính)
    if (p != NULL)
    {
        phong.set(p);
        p->quays.add(this);                              // chiều ngược lại: Phong.quays
    }
    // phieus được PhieuHangDoi tự thêm vào khi nó liên kết
}

void Quay::unlinkRelations()
{
    if (taiKhoan.hasValue()) taiKhoan->quay.clear();
    taiKhoan.clear();

    if (phong.hasValue()) phong->quays.remove(this);
    phong.clear();

    for (int i = 0; i < phieus.count(); i++) phieus.items()[i]->quay.clear();
    phieus.clear();
}

// Composition: phiếu không tồn tại độc lập khỏi quầy
void Quay::removeChildren(DbContext* ctx)
{
    MyList<PhieuHangDoi*> copy = phieus.items();
    for (int i = 0; i < copy.getSize(); i++) ctx->phieu.remove(copy[i]->getKey());
}
```

## models\Quay.h

```h
#ifndef QUAY_H
#define QUAY_H

#include "Nguoi.h"

class Phong;
class PhieuHangDoi;

// Quầy tiếp nhận (đồng thời là nhân viên quầy đăng nhập bằng username)
class Quay : public Nguoi
{
private:
    string maQuay;
    string maPHG;
    bool active;

public:
    // ----- Quan hệ (hai chiều) -----
    // (taiKhoan: ASSOCIATION 1-1, kế thừa từ Nguoi)
    Reference<Phong>         phong;        // quầy thuộc phòng nào (phía "N" của Phong ◆— Quay)
    Collection<PhieuHangDoi> phieus;       // COMPOSITION 1-N: xóa quầy -> xóa phiếu

    Quay();
    Quay(const string& maQuay, const string& maPHG, const string& username, bool active);

    string getMaQuay() const { return maQuay; }
    string getMaPHG() const { return maPHG; }
    bool isActive() const { return active; }

    // Nghiệp vụ trên đối tượng (duyệt phieus trên RAM)
    MyList<PhieuHangDoi*> waitingTickets() const;   // phiếu WAITING, sắp tăng dần theo số thứ tự
    PhieuHangDoi* servingTicket() const;            // phiếu đang SERVING (NULL nếu không có)
    int countWaiting() const;

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    string getKey() const { return maQuay; }
    void linkRelations(DbContext* ctx);
    void unlinkRelations();
    void removeChildren(DbContext* ctx);
};

typedef Quay NhanVienQuay;

#endif
```

## models\SinhVien.cpp

```cpp
#include "SinhVien.h"
#include "../orm/DbContext.h"

SinhVien::SinhVien() {}

SinhVien::SinhVien(const string& ma, const string& u) : Nguoi(u), maSV(ma) {}

string SinhVien::tableName() const { return "SinhVien"; }

Row SinhVien::columns() const { return split("maSV|username", '|'); }

Row SinhVien::toRow() const
{
    Row r;
    r.add(maSV);
    r.add(username);
    return r;
}

void SinhVien::fromRow(const Row& r)
{
    maSV = r[0];
    username = r[1];
}

void SinhVien::linkRelations(DbContext* ctx)
{
    TaiKhoan* t = ctx->taiKhoan.findByKey(username);     // 1-1: SinhVien.username -> TaiKhoan (khóa chính)
    if (t != NULL)
    {
        taiKhoan.set(t);
        t->sinhVien.set(this);                           // chiều ngược lại
    }
    // dangKys, phieus được các DangKy / PhieuHangDoi tự thêm vào khi chúng liên kết
}

void SinhVien::unlinkRelations()
{
    if (taiKhoan.hasValue()) taiKhoan->sinhVien.clear();
    taiKhoan.clear();

    // Association: phiếu vẫn tồn tại (thuộc quầy) nhưng không còn sinh viên
    for (int i = 0; i < phieus.count(); i++) phieus.items()[i]->sinhVien.clear();
    phieus.clear();
}

// Composition: hồ sơ không tồn tại độc lập khỏi sinh viên
void SinhVien::removeChildren(DbContext* ctx)
{
    MyList<DangKy*> copy = dangKys.items();              // sao chép vì danh sách thay đổi khi xóa
    for (int i = 0; i < copy.getSize(); i++) ctx->dangKy.remove(copy[i]->getKey());
}
```

## models\SinhVien.h

```h
#ifndef SINHVIEN_H
#define SINHVIEN_H

#include "Nguoi.h"

class DangKy;
class PhieuHangDoi;

class SinhVien : public Nguoi
{
private:
    string maSV;

public:
    // ----- Quan hệ (hai chiều) -----
    // (taiKhoan: ASSOCIATION 1-1, kế thừa từ Nguoi)
    Collection<DangKy>       dangKys;      // COMPOSITION 1-N: xóa sinh viên -> xóa hồ sơ
    Collection<PhieuHangDoi> phieus;       // ASSOCIATION 1-N: phiếu sinh viên đã lấy

    SinhVien();
    SinhVien(const string& maSV, const string& username);

    string getMaSV() const { return maSV; }

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    string getKey() const { return maSV; }
    void linkRelations(DbContext* ctx);
    void unlinkRelations();
    void removeChildren(DbContext* ctx);
};

#endif
```

## models\TaiKhoan.cpp

```cpp
#include "TaiKhoan.h"
#include "../orm/DbContext.h"

TaiKhoan::TaiKhoan() {}

TaiKhoan::TaiKhoan(const string& u, const string& p, const string& r)
    : username(u), password(p), role(r) {}

string TaiKhoan::tableName() const { return "TaiKhoan"; }

Row TaiKhoan::columns() const { return split("username|password|role", '|'); }

Row TaiKhoan::toRow() const
{
    Row r;
    r.add(username);
    r.add(password);
    r.add(role);
    return r;
}

void TaiKhoan::fromRow(const Row& r)
{
    username = r[0];
    password = r[1];
    role = r[2];
}

// Xóa tài khoản: các đối tượng trỏ tới nó không còn tài khoản
void TaiKhoan::unlinkRelations()
{
    if (sinhVien.hasValue()) sinhVien->taiKhoan.clear();
    if (phong.hasValue())    phong->taiKhoan.clear();
    if (quay.hasValue())     quay->taiKhoan.clear();
    sinhVien.clear();
    phong.clear();
    quay.clear();
}
```

## models\TaiKhoan.h

```h
#ifndef TAIKHOAN_H
#define TAIKHOAN_H

#include "../orm/Entity.h"
#include "../orm/Navigation.h"

class SinhVien;
class Phong;
class Quay;

class TaiKhoan : public Entity
{
private:
    string username;
    string password;
    string role;

public:
    // ----- ASSOCIATION 1-1 (chiều ngược): tài khoản này thuộc về ai? -----
    // Được SinhVien / Phong / Quay gán khi chúng liên kết tới tài khoản
    Reference<SinhVien> sinhVien;
    Reference<Phong>    phong;
    Reference<Quay>     quay;

    TaiKhoan();
    TaiKhoan(const string& username, const string& password, const string& role);

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getRole() const { return role; }

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    string getKey() const { return username; }
    void unlinkRelations();
};

#endif
```

## models\VanBan.cpp

```cpp
#include "VanBan.h"
#include "../orm/DbContext.h"

VanBan::VanBan() {}

VanBan::VanBan(const string& ma, const string& ten, const string& mp)
    : maVB(ma), tenVB(ten), maPHG(mp) {}

string VanBan::tableName() const { return "VanBan"; }

Row VanBan::columns() const { return split("maVB|tenVB|maPHG", '|'); }

Row VanBan::toRow() const
{
    Row r;
    r.add(maVB);
    r.add(tenVB);
    r.add(maPHG);
    return r;
}

void VanBan::fromRow(const Row& r)
{
    maVB = r[0];
    tenVB = r[1];
    maPHG = r[2];
}

void VanBan::linkRelations(DbContext* ctx)
{
    Phong* p = ctx->phong.findByKey(maPHG);              // N-1
    if (p != NULL)
    {
        phong.set(p);
        p->vanBans.add(this);                            // chiều ngược lại: Phong.vanBans
    }
    // dangKys, phieus được DangKy / PhieuHangDoi tự thêm vào khi chúng liên kết
}

void VanBan::unlinkRelations()
{
    if (phong.hasValue()) phong->vanBans.remove(this);
    phong.clear();

    for (int i = 0; i < dangKys.count(); i++) dangKys.items()[i]->vanBan.clear();
    dangKys.clear();
    for (int i = 0; i < phieus.count(); i++) phieus.items()[i]->vanBan.clear();
    phieus.clear();
}
```

## models\VanBan.h

```h
#ifndef VANBAN_H
#define VANBAN_H

#include "../orm/Entity.h"
#include "../orm/Navigation.h"

class Phong;
class DangKy;
class PhieuHangDoi;

class VanBan : public Entity
{
private:
    string maVB;
    string tenVB;
    string maPHG;

public:
    // ----- Quan hệ (hai chiều) -----
    Reference<Phong>         phong;        // AGGREGATION: văn bản do phòng nào cấp
    Collection<DangKy>       dangKys;      // ASSOCIATION 1-N: các hồ sơ đăng ký văn bản này
    Collection<PhieuHangDoi> phieus;       // ASSOCIATION 1-N: các phiếu hàng đợi yêu cầu văn bản này

    VanBan();
    VanBan(const string& maVB, const string& tenVB, const string& maPHG);

    string getMaVB() const { return maVB; }
    string getTenVB() const { return tenVB; }
    string getMaPHG() const { return maPHG; }

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    string getKey() const { return maVB; }
    void linkRelations(DbContext* ctx);
    void unlinkRelations();
};

#endif
```

## orm\Database.cpp

```cpp
#include "Database.h"
#include <fstream>

Database::Database(const string& f) : folder(f) {}

bool Database::tableExists(const string& table) const
{
    ifstream f((folder + table + ".txt").c_str());
    return f.good();
}

MyList<string> Database::readLines(const string& fileName) const
{
    MyList<string> r;
    ifstream f((folder + fileName).c_str());
    if (!f) return r;                       // file chưa tồn tại -> danh sách rỗng

    string line;
    bool first = true;
    while (getline(f, line))
    {
        // bỏ BOM UTF-8 nếu file được lưu từ Notepad
        if (first && line.size() >= 3 &&
            (unsigned char)line[0] == 0xEF && (unsigned char)line[1] == 0xBB && (unsigned char)line[2] == 0xBF)
            line = line.substr(3);
        first = false;

        line = trim(line);
        if (!line.empty()) r.add(line);
    }
    return r;
}

bool Database::writeLines(const string& fileName, const MyList<string>& lines) const
{
    ofstream f((folder + fileName).c_str(), ios::trunc);
    if (!f) return false;
    for (int i = 0; i < lines.getSize(); i++) f << lines[i] << "\n";
    return true;
}

MyList<Row> Database::readTable(const string& table) const
{
    MyList<Row> rows;
    MyList<string> lines = readLines(table + ".txt");
    for (int i = 0; i < lines.getSize(); i++) rows.add(split(lines[i], '|'));
    return rows;
}

bool Database::writeTable(const string& table, const MyList<Row>& rows) const
{
    MyList<string> lines;
    for (int i = 0; i < rows.getSize(); i++)
    {
        string line;
        for (int j = 0; j < rows[i].getSize(); j++)
        {
            string v = rows[i][j];
            for (size_t k = 0; k < v.size(); k++)    // '|' là ký tự phân cách -> thay bằng '/'
                if (v[k] == '|') v[k] = '/';
            if (j > 0) line += "|";
            line += v;
        }
        lines.add(line);
    }
    return writeLines(table + ".txt", lines);
}
```

## orm\Database.h

```h
#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "Entity.h"
using namespace std;

// Tầng thấp nhất của ORM: DUY NHẤT nơi mở file TXT.
// Mỗi "bảng" là một file  <folder>/<tên bảng>.txt
class Database
{
private:
    string folder;

public:
    Database(const string& folder);

    // ----- Làm việc theo bảng -----
    bool tableExists(const string& table) const;
    MyList<Row> readTable(const string& table) const;
    bool writeTable(const string& table, const MyList<Row>& rows) const;

    // ----- File thô (PrintList.txt, CurrentServing.txt...) -----
    MyList<string> readLines(const string& fileName) const;
    bool writeLines(const string& fileName, const MyList<string>& lines) const;
};

#endif
```

## orm\DbContext.cpp

```cpp
#include "DbContext.h"

DbContext::DbContext(const string& folder)
    : database_(folder),
      taiKhoan(&database_, this),
      sinhVien(&database_, this),
      phong(&database_, this),
      quay(&database_, this),
      vanBan(&database_, this),
      dangKy(&database_, this),
      phieu(&database_, this)
{
}

void DbContext::loadAll()
{
    // Bước 1: file -> RAM (tạo đối tượng, chưa có quan hệ)
    taiKhoan.load();
    sinhVien.load();
    phong.load();
    quay.load();
    vanBan.load();
    dangKy.load();
    phieu.load();

    // Bước 2: dựng quan hệ hai chiều giữa các đối tượng bằng khóa ngoại
    sinhVien.linkAll();      // SinhVien  <-> TaiKhoan
    phong.linkAll();         // Phong     <-> TaiKhoan
    quay.linkAll();          // Quay      <-> TaiKhoan, Phong
    vanBan.linkAll();        // VanBan    <-> Phong
    dangKy.linkAll();        // DangKy    <-> SinhVien, VanBan
    phieu.linkAll();         // PhieuHangDoi <-> Quay, SinhVien, VanBan
}
```

## orm\DbContext.h

```h
#ifndef DBCONTEXT_H
#define DBCONTEXT_H

#include "Database.h"
#include "DbSet.h"
#include "../models/TaiKhoan.h"
#include "../models/SinhVien.h"
#include "../models/Phong.h"
#include "../models/Quay.h"
#include "../models/VanBan.h"
#include "../models/DangKy.h"
#include "../models/PhieuHangDoi.h"

// Gom toàn bộ các bảng của hệ thống. Tầng Business chỉ làm việc với lớp này.
// Toàn bộ dữ liệu nằm trên RAM sau khi gọi loadAll().
class DbContext
{
private:
    Database database_;

public:
    DbSet<TaiKhoan>     taiKhoan;
    DbSet<SinhVien>     sinhVien;
    DbSet<Phong>        phong;
    DbSet<Quay>         quay;
    DbSet<VanBan>       vanBan;
    DbSet<DangKy>       dangKy;
    DbSet<PhieuHangDoi> phieu;

    DbContext(const string& folder);

    // Nạp TẤT CẢ bảng lên RAM rồi dựng quan hệ giữa các đối tượng. Gọi một lần lúc khởi động.
    void loadAll();

    // Không cho sao chép (các DbSet giữ con trỏ tới database_ và DbContext)
    DbContext(const DbContext&) = delete;
    DbContext& operator=(const DbContext&) = delete;

    Database& database() { return database_; }
};

#endif
```

## orm\DbSet.h

```h
#ifndef DBSET_H
#define DBSET_H

#include <cstddef>
#include "../templates/Repository.h"
#include "Database.h"
#include "Entity.h"

// Đại diện cho MỘT BẢNG chứa các đối tượng kiểu T (T kế thừa Entity,
// có constructor mặc định) và GIỮ TOÀN BỘ chúng trên RAM.
//
//  - load()  : đọc file TXT -> tạo đối tượng (gọi MỘT LẦN lúc khởi động, xem DbContext::loadAll)
//  - get     : findAll / findByKey / findBy ... chỉ duyệt trên RAM, KHÔNG đọc lại file
//  - modify  : service sửa thẳng đối tượng trên RAM rồi gọi update() -> ghi file
//  - add/remove: sửa RAM + cập nhật quan hệ hai chiều + ghi file
//
// DbSet là chủ sở hữu duy nhất của các đối tượng (new trong load/add, delete khi remove/hủy).
// Nơi khác chỉ giữ con trỏ T* (không delete).
template<class T>
class DbSet : public Repository<T>
{
private:
    Database* db;
    DbContext* context;          // truyền cho entity khi liên kết quan hệ
    MyList<T*> items;

    string table() const
    {
        T sample;
        return sample.tableName();
    }

    int columnIndex(const string& column) const
    {
        T sample;
        Row cols = sample.columns();
        for (int i = 0; i < cols.getSize(); i++)
            if (cols[i] == column) return i;
        return -1;
    }

    void clearItems()
    {
        for (int i = 0; i < items.getSize(); i++) delete items[i];
        items.clear();
    }

public:
    DbSet(Database* database, DbContext* ctx) : db(database), context(ctx) {}
    ~DbSet() { clearItems(); }

    // Không cho sao chép (sở hữu con trỏ)
    DbSet(const DbSet&) = delete;
    DbSet& operator=(const DbSet&) = delete;

    // ---------- Giao diện Repository ----------
    // file -> RAM (chưa liên kết quan hệ). Chỉ gọi lúc khởi động,
    // vì gọi lại sẽ làm các con trỏ quan hệ cũ bị treo.
    void load()
    {
        clearItems();
        T sample;
        int n = sample.columns().getSize();

        MyList<Row> rows = db->readTable(table());
        for (int i = 0; i < rows.getSize(); i++)
        {
            Row r = rows[i];
            while (r.getSize() < n) r.add("");      // dòng thiếu cột -> bù rỗng
            T* obj = new T();
            obj->fromRow(r);
            items.add(obj);
        }
    }

    // RAM -> file
    void save()
    {
        MyList<Row> rows;
        for (int i = 0; i < items.getSize(); i++) rows.add(items[i]->toRow());
        db->writeTable(table(), rows);
    }

    // Thiết lập quan hệ hai chiều cho mọi đối tượng (gọi sau khi TẤT CẢ bảng đã load)
    void linkAll()
    {
        for (int i = 0; i < items.getSize(); i++) items[i]->linkRelations(context);
    }

    void add(T* obj)                                 // INSERT
    {
        items.add(obj);
        obj->linkRelations(context);                 // gắn vào các đối tượng liên quan (hai chiều)
        save();
    }

    void update(T* obj)                              // UPDATE: obj đã được sửa trên RAM -> ghi file
    {
        (void)obj;
        save();
    }

    void remove(const string& key)                   // DELETE theo khóa chính
    {
        T* obj = findByKey(key);
        if (obj == NULL) return;

        obj->removeChildren(context);                // Composition: xóa các đối tượng con trước
        obj->unlinkRelations();                      // gỡ liên kết hai chiều

        for (int i = 0; i < items.getSize(); i++)
        {
            if (items[i] == obj) { items.remove(i); break; }
        }
        delete obj;
        save();
    }

    // ---------- Truy vấn trên RAM ----------
    const MyList<T*>& all() const { return items; }  // SELECT *
    int count() const { return items.getSize(); }

    T* findByKey(const string& key) const            // WHERE khóa = key; NULL nếu không có
    {
        for (int i = 0; i < items.getSize(); i++)
            if (items[i]->getKey() == key) return items[i];
        return NULL;
    }

    MyList<T*> findBy(const string& column, const string& value) const   // WHERE cột = giá trị
    {
        MyList<T*> result;
        int idx = columnIndex(column);
        if (idx < 0) return result;
        for (int i = 0; i < items.getSize(); i++)
        {
            Row r = items[i]->toRow();
            if (r[idx] == value) result.add(items[i]);
        }
        return result;
    }

    T* findOneBy(const string& column, const string& value) const
    {
        MyList<T*> r = findBy(column, value);
        return r.isEmpty() ? NULL : r[0];
    }

    int countBy(const string& column, const string& value) const
    {
        return findBy(column, value).getSize();
    }

    // Sinh khóa tiếp theo: nextId("DK", 3) -> DK001, DK002, ...
    string nextId(const string& prefix, int width) const
    {
        int mx = 0;
        for (int i = 0; i < items.getSize(); i++)
        {
            string key = items[i]->getKey();
            if (key.size() > prefix.size())
            {
                int n = toInt(key.substr(prefix.size()), 0);
                if (n > mx) mx = n;
            }
        }
        return prefix + pad(mx + 1, width);
    }
};

#endif
```

## orm\Entity.h

```h
#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include "../templates/MyList.h"
#include "../utils/Utils.h"
using namespace std;

// Một dòng dữ liệu = danh sách giá trị các cột (dạng chuỗi)
typedef MyList<string> Row;

class DbContext;        // khai báo trước, Entity không cần biết chi tiết

// Lớp cơ sở cho mọi đối tượng được ORM ánh xạ vào file TXT.
// Mỗi đối tượng được tạo DUY NHẤT MỘT LẦN trên RAM (do DbSet sở hữu);
// các đối tượng liên kết với nhau bằng CON TRỎ -> không cho sao chép.
class Entity
{
public:
    Entity() {}
    virtual ~Entity() {}
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    // ----- Entity Mapping: object <-> dòng trong file TXT -----
    virtual string tableName() const = 0;            // tên bảng (= tên file không đuôi)
    virtual Row columns() const = 0;                 // tên các cột, theo thứ tự
    virtual Row toRow() const = 0;                   // object  -> dòng dữ liệu
    virtual void fromRow(const Row& row) = 0;        // dòng dữ liệu -> object
    virtual string getKey() const = 0;               // giá trị khóa chính

    // ----- Object Relationship (thiết lập / gỡ liên kết hai chiều trên RAM) -----
    // linkRelations  : tìm các đối tượng liên quan theo khóa ngoại, gắn hai chiều
    //                  (this -> liên quan  và  liên quan -> this)
    // unlinkRelations: gỡ mọi liên kết trỏ tới this (gọi trước khi xóa this)
    // removeChildren : Composition - xóa các đối tượng con phụ thuộc vòng đời của this
    virtual void linkRelations(DbContext*) {}
    virtual void unlinkRelations() {}
    virtual void removeChildren(DbContext*) {}
};

#endif
```

## orm\Navigation.h

```h
#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <cstddef>
#include <string>
#include "../templates/MyList.h"
using namespace std;

// =====================================================================
// Reference<T>: Navigation Property trỏ tới MỘT đối tượng (N-1 hoặc 1-1).
// KHÔNG sở hữu đối tượng: chỉ giữ con trỏ tới đối tượng nằm trong DbSet (RAM).
// =====================================================================
template<class T>
class Reference
{
private:
    T* value;

public:
    Reference() : value(NULL) {}

    void set(T* p) { value = p; }
    void clear() { value = NULL; }

    T* get() const { return value; }              // NULL nếu không có
    T* operator->() const { return value; }       // kiểm tra hasValue() nếu có thể NULL
    bool hasValue() const { return value != NULL; }
};

// =====================================================================
// Collection<T>: Navigation Property trỏ tới NHIỀU đối tượng (1-N).
// KHÔNG sở hữu các đối tượng. Không chứa trùng con trỏ.
// =====================================================================
template<class T>
class Collection
{
private:
    MyList<T*> list;

public:
    int indexOf(T* p) const
    {
        for (int i = 0; i < list.getSize(); i++)
            if (list[i] == p) return i;
        return -1;
    }

    void add(T* p)
    {
        if (p != NULL && indexOf(p) < 0) list.add(p);
    }

    void remove(T* p)
    {
        int i = indexOf(p);
        if (i >= 0) list.remove(i);
    }

    bool contains(T* p) const { return indexOf(p) >= 0; }
    void clear() { list.clear(); }

    const MyList<T*>& items() const { return list; }
    int count() const { return list.getSize(); }
};

#endif
```

## presentation\CounterUI.cpp

```cpp
#include "CounterUI.h"
#include <iostream>

Counter::Counter(TaiKhoan* a, AppContext& c) : User(a), ctx(c), quay(NULL), ok(false)
{
    quay = account->quay.get();                     // Navigation 1-1: TaiKhoan -> Quay (đủ quan hệ phong, phieus)
    ok = (quay != NULL);
}

// Hiển thị toàn bộ hồ sơ của một sinh viên (dùng cho option 2: tra cứu theo MSSV)
void Counter::hienThiHoSoSV(const string& maSV)
{
    MyList<DangKy*> ds = ctx.reg.getBySinhVien(maSV);
    cout << "\n--- Thông tin sinh viên MSSV " << maSV << " ---\n";
    if (ds.isEmpty()) cout << "Sinh viên chưa có hồ sơ nào.\n";
    for (int i = 0; i < ds.getSize(); i++)
    {
        printDangKy(ds[i]);
        cout << endl;
    }
}

// UC10 + UC11: gọi số, chỉ hiển thị yêu cầu của tờ số sinh viên đã lấy (giấy tờ / tư vấn)
void Counter::goiSoTiepTheo()
{
    PhieuHangDoi* p = NULL;
    string msg;
    if (!ctx.queue.callNext(quay, p, msg))
    {
        cout << msg << endl;
        pauseScreen();
        return;
    }
    cout << "\n>>> MỜI SỐ " << p->getDisplay() << " ĐẾN QUẦY " << quay->getMaQuay() << " <<<\n";
    cout << "    MSSV    : " << p->getMaSV() << endl;
    cout << "    Yêu cầu : " << p->tenYeuCau() << endl;
    pauseScreen();
}

void Counter::xemHoSo()
{
    string maSV = readLine("Nhập MSSV (Enter = sinh viên đang phục vụ): ");
    if (maSV.empty())
    {
        PhieuHangDoi* cur = quay->servingTicket();
        if (cur == NULL)
        {
            cout << "Quầy chưa phục vụ sinh viên nào. Hãy gọi số trước.\n";
            pauseScreen();
            return;
        }
        maSV = cur->getMaSV();
    }
    hienThiHoSoSV(maSV);
    pauseScreen();
}

// UC12: danh sách giấy cần xử lý trả hiển thị riêng ở đây
void Counter::xacNhanTraGiay()
{
    PhieuHangDoi* cur = quay->servingTicket();
    if (cur == NULL)
    {
        cout << "Quầy chưa phục vụ sinh viên nào. Hãy gọi số trước.\n";
        pauseScreen();
        return;
    }

    // Giấy của sinh viên đang phục vụ, thuộc phòng của quầy, đã có ngày nhận, chưa trả
    MyList<DangKy*> ds = ctx.reg.getForReturn(cur->getMaSV(), quay->getMaPHG());
    cout << "\n=== GIẤY CẦN TRẢ - MSSV " << cur->getMaSV() << " ===\n";
    if (ds.isEmpty())
    {
        cout << "Sinh viên không có giấy nào cần trả tại phòng này.\n";
        pauseScreen();
        return;
    }
    for (int i = 0; i < ds.getSize(); i++)
    {
        cout << i + 1 << ". ";
        printDangKy(ds[i]);
    }

    string line = readLine("Chọn các giấy cần trả (VD: 1 2, hoặc ALL, Enter để hủy): ");
    MyList<int> sel = parseSelection(line, ds.getSize());
    if (sel.isEmpty()) { cout << "Không có giấy nào được chọn.\n"; pauseScreen(); return; }

    for (int k = 0; k < sel.getSize(); k++)
    {
        string msg;
        ctx.reg.confirmReceive(ds[sel[k]]->getMaDK(), cur->getMaSV(), msg);
        cout << msg << endl;
    }
    pauseScreen();
}

void Counter::showMenu()
{
    if (!ok)
    {
        cout << "Tài khoản chưa được gắn với quầy nào.\n";
        pauseScreen();
        return;
    }
    while (true)
    {
        clearScreen();
        PhieuHangDoi* cur = quay->servingTicket();

        Phong* ph = quay->phong.get();              // Navigation N-1: Quay -> Phong
        cout << "====== QUẦY " << quay->getMaQuay();
        if (ph != NULL) cout << " (" << ph->getTenPhong() << ")";
        cout << " ======\n";
        if (cur != NULL)
        {
            cout << "Đang phục vụ: " << cur->getDisplay() << " (MSSV " << cur->getMaSV() << ")\n";
            cout << "Yêu cầu     : " << cur->tenYeuCau() << endl;
        }
        else
        {
            cout << "Đang phục vụ: ---\n";
        }
        cout << "Đang chờ    : " << quay->countWaiting() << " người\n\n";
        cout << "1. Gọi số tiếp theo\n";
        cout << "2. Xem hồ sơ sinh viên\n";
        cout << "3. Xác nhận trả giấy\n";
        cout << "4. Đăng xuất\n";
        int c = readInt("Chọn: ");
        if (c == 1) goiSoTiepTheo();
        else if (c == 2) xemHoSo();
        else if (c == 3) xacNhanTraGiay();
        else if (c == 4) return;
        else { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); }
    }
}
```

## presentation\CounterUI.h

```h
#ifndef COUNTERUI_H
#define COUNTERUI_H

#include "User.h"

class Counter : public User
{
private:
    AppContext& ctx;
    Quay* quay;                         // quầy của tài khoản (đối tượng trên RAM)
    bool ok;

    void hienThiHoSoSV(const string& maSV);
    void goiSoTiepTheo();
    void xemHoSo();
    void xacNhanTraGiay();

public:
    Counter(TaiKhoan* a, AppContext& c);
    void showMenu();
};

#endif
```

## presentation\DashboardUI.cpp

```cpp
#include "DashboardUI.h"
#include <iostream>

void DashboardUI::run(AppContext& ctx)
{
    while (true)
    {
        // Reload từ FILE mỗi 3 giây (không dùng RAM, không ghi đè file)
        MyList<CounterDisplay> ds = ctx.dash.readCurrent();

        clearScreen();
        cout << "=================================================\n";
        cout << "              DASHBOARD HÀNG ĐỢI\n";
        cout << "=================================================\n";
        for (int i = 0; i < ds.getSize(); i++)
        {
            cout << " QUAY " << ds[i].maQuay << " : "
                 << (ds[i].active ? ds[i].soHienThi : string("DONG"));

            if (ds[i].active && !ds[i].danhSachCho.isEmpty())
            {
                cout << "Tiep theo: ";
                for (int j = 0; j < ds[i].danhSachCho.getSize(); j++)
                {
                    if (j > 0) cout << ", ";
                    cout << ds[i].danhSachCho[j];
                }
                cout << endl;
            }
            else if (ds[i].active)
            {
                cout << "Trong" << endl;
            }
            cout << "-------------------------------------------------\n";
        }
        cout << "(Tu dong cap nhat moi 3 giay - nhan Enter de thoat)\n";

        if (waitKey(3000)) break;
    }
}
```

## presentation\DashboardUI.h

```h
#ifndef DASHBOARDUI_H
#define DASHBOARDUI_H

#include "../business/AppContext.h"

// UC09: Dashboard công khai, tự reload mỗi 3 giây từ CurrentServing.txt
class DashboardUI
{
public:
    static void run(AppContext& ctx);
};

#endif
```

## presentation\DepartmentUI.cpp

```cpp
#include "DepartmentUI.h"
#include <iostream>

Department::Department(TaiKhoan* a, AppContext& c) : User(a), ctx(c), phong(NULL), ok(false)
{
    phong = account->phong.get();                   // Navigation 1-1: TaiKhoan -> Phong
    ok = (phong != NULL);
}

void Department::listWithIndex(MyList<DangKy*>& ds)
{
    for (int i = 0; i < ds.getSize(); i++)
    {
        cout << i + 1 << ". ";
        printDangKy(ds[i]);
    }
}

void Department::xemChoDuyet()
{
    MyList<DangKy*> ds = ctx.reg.getPending(phong->getMaPHG());
    cout << "\n=== HỒ SƠ CHỜ DUYỆT ===\n";
    if (ds.isEmpty()) cout << "Không có hồ sơ nào chờ duyệt.\n";
    listWithIndex(ds);
    pauseScreen();
}

// UC03
void Department::duyetHoSo()
{
    MyList<DangKy*> ds = ctx.reg.getPending(phong->getMaPHG());
    cout << "\n=== DUYỆT HỒ SƠ ===\n";
    if (ds.isEmpty()) { cout << "Không có hồ sơ nào chờ duyệt.\n"; pauseScreen(); return; }
    listWithIndex(ds);

    string line = readLine("Chọn các hồ sơ (VD: 1 2 4, hoặc ALL, Enter để hủy): ");
    MyList<int> sel = parseSelection(line, ds.getSize());
    if (sel.isEmpty()) { cout << "Không có hồ sơ nào được chọn.\n"; pauseScreen(); return; }

    for (int k = 0; k < sel.getSize(); k++)
    {
        const DangKy* d = ds[sel[k]];
        cout << "\n--- Đơn " << d->getMaDK() << " (" << d->tenVanBan()
             << ", MSSV " << d->getMaSV() << ") ---\n";
        cout << "1. Đồng ý\n2. Từ chối\n";
        int c = readInt("Chọn: ");

        string msg;
        if (c == 1)
        {
            ctx.reg.approve(d->getMaDK(), msg);
        }
        else if (c == 2)
        {
            string lyDo = readLine("Nhập lý do từ chối: ");
            while (lyDo.empty()) lyDo = readLine("Lý do không được để trống. Nhập lại: ");
            ctx.reg.reject(d->getMaDK(), lyDo, msg);
        }
        else
        {
            msg = "Bỏ qua đơn " + d->getMaDK() + ".";
        }
        cout << msg << endl;
    }
    pauseScreen();
}

// UC04
void Department::xuatDanhSachIn()
{
    int n = ctx.reg.exportPrintList(phong->getMaPHG());
    if (n < 0) cout << "Không ghi được file PrintList.txt\n";
    else if (n == 0) cout << "Không có hồ sơ nào cần in.\n";
    else cout << "Đã xuất " << n << " hồ sơ ra file PrintList.txt (thư mục data).\n";
    pauseScreen();
}

// UC05 + UC06: ngày in / ngày nhận = thời điểm bấm cập nhật (không cần nhập)
void Department::capNhatNgay(bool ngayIn)
{
    MyList<DangKy*> ds = ngayIn ? ctx.reg.getForPrint(phong->getMaPHG())
                               : ctx.reg.getForReceive(phong->getMaPHG());
    cout << (ngayIn ? "\n=== CẬP NHẬT NGÀY IN ===\n" : "\n=== CẬP NHẬT NGÀY NHẬN ===\n");
    if (ds.isEmpty())
    {
        cout << (ngayIn ? "Không có hồ sơ nào đang chờ in.\n"
                        : "Không có hồ sơ nào đã in để cập nhật ngày nhận.\n");
        pauseScreen();
        return;
    }
    listWithIndex(ds);

    string line = readLine("Chọn các hồ sơ (VD: 1 3, hoặc ALL, Enter để hủy): ");
    MyList<int> sel = parseSelection(line, ds.getSize());
    if (sel.isEmpty()) { cout << "Không có hồ sơ nào được chọn.\n"; pauseScreen(); return; }

    // Thời điểm chọn cập nhật: lấy một lần cho cả lượt chọn
    string thoiGian = nowDateTime();
    cout << "\nThời điểm cập nhật: " << thoiGian << endl;
    for (int k = 0; k < sel.getSize(); k++)
    {
        string msg;
        if (ngayIn) ctx.reg.setNgayIn(ds[sel[k]]->getMaDK(), thoiGian, msg);
        else ctx.reg.setNgayNhan(ds[sel[k]]->getMaDK(), thoiGian, msg);
        cout << msg << endl;
    }
    pauseScreen();
}

void Department::showMenu()
{
    if (!ok)
    {
        cout << "Tài khoản chưa được gắn với phòng ban nào.\n";
        pauseScreen();
        return;
    }
    while (true)
    {
        clearScreen();
        cout << "====== " << phong->getTenPhong() << " ======\n";
        cout << "1. Xem hồ sơ chờ duyệt\n";
        cout << "2. Duyệt hồ sơ\n";
        cout << "3. Xuất danh sách in\n";
        cout << "4. Cập nhật ngày in\n";
        cout << "5. Cập nhật ngày nhận\n";
        cout << "6. Đăng xuất\n";
        int c = readInt("Chọn: ");
        if (c == 1) xemChoDuyet();
        else if (c == 2) duyetHoSo();
        else if (c == 3) xuatDanhSachIn();
        else if (c == 4) capNhatNgay(true);
        else if (c == 5) capNhatNgay(false);
        else if (c == 6) return;
        else { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); }
    }
}
```

## presentation\DepartmentUI.h

```h
#ifndef DEPARTMENTUI_H
#define DEPARTMENTUI_H

#include "User.h"

class Department : public User
{
private:
    AppContext& ctx;
    Phong* phong;                       // phòng của tài khoản (đối tượng trên RAM)
    bool ok;

    void listWithIndex(MyList<DangKy*>& ds);
    void xemChoDuyet();
    void duyetHoSo();
    void xuatDanhSachIn();
    void capNhatNgay(bool ngayIn);      // true: ngày in, false: ngày nhận

public:
    Department(TaiKhoan* a, AppContext& c);
    void showMenu();
};

#endif
```

## presentation\MainUI.cpp

```cpp
#include "MainUI.h"
#include "StudentUI.h"
#include "DepartmentUI.h"
#include "CounterUI.h"
#include "DashboardUI.h"
#include <iostream>

MainUI::MainUI(AppContext& c) : ctx(c) {}

void MainUI::dangNhap()
{
    cout << "\n--- ĐĂNG NHẬP ---\n";
    string u = readLine("Username: ");
    string p = readLine("Password: ");

    TaiKhoan* acc = ctx.auth.login(u, p);
    if (acc == NULL)
    {
        cout << "Sai username hoặc password.\n";
        pauseScreen();
        return;
    }

    // Đa hình: tạo đối tượng theo Role
    User* user = NULL;
    if (acc->getRole() == ROLE_STUDENT) user = new Student(acc, ctx);
    else if (acc->getRole() == ROLE_PHONG) user = new Department(acc, ctx);
    else if (acc->getRole() == ROLE_QUAY) user = new Counter(acc, ctx);

    if (user != NULL)
    {
        user->showMenu();
        delete user;
    }
    else
    {
        cout << "Vai trò không hợp lệ: " << acc->getRole() << endl;
        pauseScreen();
    }
}

// UC07: Guest lấy số thứ tự: nhập MSSV -> chọn phòng -> chọn giấy tờ (hoặc "Tư vấn")
void MainUI::layThuTu()
{
    cout << "\n--- LẤY SỐ THỨ TỰ ---\n";
    string mssv = readLine("Nhập MSSV: ");

    string msg;
    if (!ctx.queue.canTakeTicket(mssv, msg)) { cout << msg << endl; pauseScreen(); return; }

    MyList<Phong*> phongs = ctx.doc.getAllPhong();
    if (phongs.isEmpty()) { cout << "Chưa có phòng ban nào.\n"; pauseScreen(); return; }

    cout << "\n--- Chọn phòng ---\n";
    for (int i = 0; i < phongs.getSize(); i++) cout << i + 1 << ". " << phongs[i]->getTenPhong() << endl;
    int p = readInt("Chọn phòng (0 = quay lại): ");
    if (p == 0) return;
    if (p < 1 || p > phongs.getSize()) { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); return; }
    Phong* ph = phongs[p - 1];

    // Option 1 luôn là "Tư vấn" (nếu không chọn giấy nào thì tư vấn tại quầy của phòng)
    MyList<VanBan*> vbs = ctx.doc.getVanBanByPhong(ph->getMaPHG());
    cout << "\n--- " << ph->getTenPhong() << " ---\n";
    cout << "1. Tư vấn\n";
    for (int i = 0; i < vbs.getSize(); i++) cout << i + 2 << ". " << vbs[i]->getTenVB() << endl;
    int v = readInt("Chọn nội dung cần giải quyết (0 = quay lại): ");
    if (v == 0) return;
    if (v < 1 || v > vbs.getSize() + 1) { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); return; }
    string maVB = (v == 1) ? REQ_CONSULT : vbs[v - 2]->getMaVB();

    PhieuHangDoi* t = NULL;
    if (ctx.queue.takeTicket(mssv, ph->getMaPHG(), maVB, t, msg))
    {
        cout << "\n+---------------------------+\n";
        cout << "|        VÉ HÀNG ĐỢI        |\n";
        cout << "+---------------------------+\n";
        cout << "  MSSV      : " << t->getMaSV() << endl;
        cout << "  Số thứ tự : " << t->getDisplay() << endl;
        cout << "  Phòng     : " << ph->getTenPhong() << endl;
        cout << "  Quầy      : " << t->getMaQuay() << endl;
        cout << "  Yêu cầu   : " << t->tenYeuCau() << endl;
        cout << "  Thời gian : " << nowDateTime() << endl;
        cout << "+---------------------------+\n";
    }
    else
    {
        cout << msg << endl;
    }
    pauseScreen();
}

void MainUI::run()
{
    while (true)
    {
        clearScreen();
        cout << "====================================\n";
        cout << " STUDENT DOCUMENT MANAGEMENT SYSTEM\n";
        cout << "====================================\n\n";
        cout << "1. Đăng nhập\n";
        cout << "2. Lấy số thứ tự\n";
        cout << "3. Dashboard công khai\n";
        cout << "0. Thoát\n";
        int c = readInt("Chọn: ");
        if (c == 1) dangNhap();
        else if (c == 2) layThuTu();
        else if (c == 3) DashboardUI::run(ctx);
        else if (c == 0) return;
        else { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); }
    }
}
```

## presentation\MainUI.h

```h
#ifndef MAINUI_H
#define MAINUI_H

#include "../business/AppContext.h"

class MainUI
{
private:
    AppContext& ctx;

    void dangNhap();
    void layThuTu();

public:
    MainUI(AppContext& c);
    void run();
};

#endif
```

## presentation\StudentUI.cpp

```cpp
#include "StudentUI.h"
#include <iostream>

Student::Student(TaiKhoan* a, AppContext& c) : User(a), ctx(c)
{
    SinhVien* sv = account->sinhVien.get();         // Navigation 1-1: TaiKhoan -> SinhVien
    maSV = (sv != NULL) ? sv->getMaSV() : string("");
}

// UC01
void Student::dangKyGiayTo()
{
    MyList<Phong*> ds = ctx.doc.getAllPhong();
    if (ds.isEmpty()) { cout << "Chưa có phòng ban nào.\n"; pauseScreen(); return; }

    cout << "\n--- Chọn phòng ---\n";
    for (int i = 0; i < ds.getSize(); i++) cout << i + 1 << ". " << ds[i]->getTenPhong() << endl;
    int p = readInt("Chọn phòng (0 = quay lại): ");
    if (p < 1 || p > ds.getSize()) return;

    MyList<VanBan*> vb = ctx.doc.getVanBanByPhong(ds[p - 1]->getMaPHG());
    if (vb.isEmpty()) { cout << "Phòng này chưa có văn bản nào.\n"; pauseScreen(); return; }

    cout << "\n--- " << ds[p - 1]->getTenPhong() << " ---\n";
    for (int i = 0; i < vb.getSize(); i++) cout << i + 1 << ". " << vb[i]->getTenVB() << endl;
    int v = readInt("Chọn văn bản (0 = quay lại): ");
    if (v < 1 || v > vb.getSize()) return;

    string yn = toUpper(readLine("Xác nhận đăng ký \"" + vb[v - 1]->getTenVB() + "\"? (Y/N): "));
    if (yn != "Y") { cout << "Đã hủy.\n"; pauseScreen(); return; }

    string msg;
    ctx.reg.registerDocument(maSV, vb[v - 1]->getMaVB(), msg);
    cout << msg << endl;
    pauseScreen();
}

// UC02
void Student::xemHoSo()
{
    MyList<DangKy*> ds = ctx.reg.getBySinhVien(maSV);
    cout << "\n=== HỒ SƠ ĐÃ ĐĂNG KÝ ===\n";
    if (ds.isEmpty()) cout << "Bạn chưa đăng ký hồ sơ nào.\n";
    for (int i = 0; i < ds.getSize(); i++)
    {
        printDangKy(ds[i]);
        cout << endl;
    }
    pauseScreen();
}

void Student::showMenu()
{
    if (maSV.empty())
    {
        cout << "Tài khoản chưa được gắn với sinh viên nào.\n";
        pauseScreen();
        return;
    }
    while (true)
    {
        clearScreen();
        cout << "====== SINH VIÊN: " << maSV << " ======\n";
        cout << "1. Đăng ký giấy tờ\n";
        cout << "2. Xem hồ sơ đã đăng ký\n";
        cout << "3. Đăng xuất\n";
        int c = readInt("Chọn: ");
        if (c == 1) dangKyGiayTo();
        else if (c == 2) xemHoSo();
        else if (c == 3) return;
        else { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); }
    }
}
```

## presentation\StudentUI.h

```h
#ifndef STUDENTUI_H
#define STUDENTUI_H

#include "User.h"

class Student : public User
{
private:
    AppContext& ctx;
    string maSV;

    void dangKyGiayTo();
    void xemHoSo();

public:
    Student(TaiKhoan* a, AppContext& c);
    void showMenu();
};

#endif
```

## presentation\User.cpp

```cpp
#include "User.h"
#include <iostream>

User::User(TaiKhoan* a) : account(a) {}

void printDangKy(const DangKy* d)
{
    cout << d->getMaDK() << " | " << d->tenVanBan() << " | MSSV: " << d->getMaSV() << endl;
    cout << "   Trạng thái: " << d->getTrangThai() << " | Ngày đăng ký: " << d->getNgayDK() << endl;
    if (!d->getNgayIn().empty()) cout << "   Ngày in: " << d->getNgayIn() << endl;
    if (!d->getNgayNhan().empty()) cout << "   Ngày nhận: " << d->getNgayNhan() << endl;
    if (!d->getLyDo().empty()) cout << "   Lý do từ chối: " << d->getLyDo() << endl;
}
```

## presentation\User.h

```h
#ifndef USER_H
#define USER_H

#include "../business/AppContext.h"

// Lớp trừu tượng: đa hình qua showMenu()
class User
{
protected:
    TaiKhoan* account;      // tài khoản đăng nhập (đối tượng trên RAM, đã có sẵn các quan hệ)

public:
    User(TaiKhoan* a);
    virtual ~User() {}
    virtual void showMenu() = 0;
};

// In một hồ sơ đăng ký ra màn hình
void printDangKy(const DangKy* d);

#endif
```

## templates\MyList.h

```h
#ifndef MYLIST_H
#define MYLIST_H

#include <stdexcept>

// Danh sách động tự cài đặt (mảng tự giãn nở)
template<class T>
class MyList
{
private:
    T* data;
    int size;
    int capacity;

    void grow()
    {
        int newCap = capacity * 2;
        T* nd = new T[newCap];
        for (int i = 0; i < size; i++) nd[i] = data[i];
        delete[] data;
        data = nd;
        capacity = newCap;
    }

public:
    MyList() : data(new T[4]), size(0), capacity(4) {}

    MyList(const MyList& o) : data(new T[o.capacity]), size(o.size), capacity(o.capacity)
    {
        for (int i = 0; i < size; i++) data[i] = o.data[i];
    }

    MyList& operator=(const MyList& o)
    {
        if (this != &o)
        {
            T* nd = new T[o.capacity];
            for (int i = 0; i < o.size; i++) nd[i] = o.data[i];
            delete[] data;
            data = nd;
            size = o.size;
            capacity = o.capacity;
        }
        return *this;
    }

    ~MyList() { delete[] data; }

    void add(const T& value)
    {
        if (size == capacity) grow();
        data[size++] = value;
    }

    void remove(int index)
    {
        if (index < 0 || index >= size) throw std::out_of_range("MyList::remove");
        for (int i = index; i < size - 1; i++) data[i] = data[i + 1];
        size--;
    }

    T& get(int index)
    {
        if (index < 0 || index >= size) throw std::out_of_range("MyList::get");
        return data[index];
    }

    const T& get(int index) const
    {
        if (index < 0 || index >= size) throw std::out_of_range("MyList::get");
        return data[index];
    }

    T& operator[](int index) { return get(index); }
    const T& operator[](int index) const { return get(index); }

    int getSize() const { return size; }
    bool isEmpty() const { return size == 0; }
    void clear() { size = 0; }
};

#endif
```

## templates\MyQueue.h

```h
#ifndef MYQUEUE_H
#define MYQUEUE_H

#include <stdexcept>

// Hàng đợi vòng (circular queue) tự cài đặt, tự giãn nở
template<class T>
class MyQueue
{
private:
    T* data;
    int front;
    int rear;
    int count;
    int capacity;

    void grow()
    {
        T* nd = new T[capacity * 2];
        for (int i = 0; i < count; i++) nd[i] = data[(front + i) % capacity];
        delete[] data;
        data = nd;
        front = 0;
        rear = count;
        capacity *= 2;
    }

public:
    MyQueue() : data(new T[8]), front(0), rear(0), count(0), capacity(8) {}

    MyQueue(const MyQueue& o) : data(new T[o.capacity]), front(0), rear(o.count), count(o.count), capacity(o.capacity)
    {
        for (int i = 0; i < count; i++) data[i] = o.data[(o.front + i) % o.capacity];
        if (rear == capacity) rear = 0;
    }

    MyQueue& operator=(const MyQueue& o)
    {
        if (this != &o)
        {
            T* nd = new T[o.capacity];
            for (int i = 0; i < o.count; i++) nd[i] = o.data[(o.front + i) % o.capacity];
            delete[] data;
            data = nd;
            front = 0;
            count = o.count;
            capacity = o.capacity;
            rear = (count == capacity) ? 0 : count;
        }
        return *this;
    }

    ~MyQueue() { delete[] data; }

    void enqueue(const T& item)
    {
        if (count == capacity) grow();
        data[rear] = item;
        rear = (rear + 1) % capacity;
        count++;
    }

    void dequeue()
    {
        if (count == 0) throw std::out_of_range("MyQueue::dequeue");
        front = (front + 1) % capacity;
        count--;
    }

    T& peek()
    {
        if (count == 0) throw std::out_of_range("MyQueue::peek");
        return data[front];
    }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
};

#endif
```

## templates\Repository.h

```h
#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>
using namespace std;

// Giao diện Repository tổng quát (Repository Pattern).
// DbSet<T> của ORM là bản cài đặt cụ thể của giao diện này.
// Repository giữ các đối tượng T* trên RAM; thay đổi trên RAM được ghi về file TXT.
template<class T>
class Repository
{
public:
    virtual ~Repository() {}
    virtual void load() = 0;                       // file -> RAM
    virtual void save() = 0;                       // RAM  -> file
    virtual void add(T* obj) = 0;                  // INSERT (nhận quyền sở hữu obj)
    virtual void update(T* obj) = 0;               // UPDATE (obj đã được sửa trên RAM)
    virtual void remove(const string& key) = 0;    // DELETE theo khóa chính
};

#endif
```

## utils\Utils.cpp

```cpp
#include "Utils.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif

namespace
{
    long daysFromCivil(int y, int m, int d)
    {
        y -= (m <= 2);
        long era = (y >= 0 ? y : y - 399) / 400;
        unsigned yoe = (unsigned)(y - era * 400);
        unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
        unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
        return era * 146097 + (long)doe - 719468;
    }

    bool isValidDate(int d, int m, int y)
    {
        if (m < 1 || m > 12 || d < 1 || y < 2000 || y > 2100) return false;
        int dm[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
        return d <= dm[m - 1] + ((m == 2 && leap) ? 1 : 0);
    }
}

// ---------- Chuỗi ----------
MyList<string> split(const string& s, char delim)
{
    MyList<string> r;
    string cur;
    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == delim) { r.add(cur); cur.clear(); }
        else cur += s[i];
    }
    r.add(cur);
    return r;
}

string trim(const string& s)
{
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string toUpper(string s)
{
    for (size_t i = 0; i < s.size(); i++)
        if (s[i] >= 'a' && s[i] <= 'z') s[i] -= 32;
    return s;
}

string pad(int n, int width)
{
    string s = to_string(n);
    while ((int)s.size() < width) s = "0" + s;
    return s;
}

int toInt(const string& s, int def)
{
    if (s.empty()) return def;
    for (size_t i = 0; i < s.size(); i++)
        if (s[i] < '0' || s[i] > '9') return def;
    return atoi(s.c_str());
}

// ---------- Ngày tháng ----------
bool normalizeDate(string& s)
{
    int d, m, y;
    s = trim(s);
    if (sscanf(s.c_str(), "%d/%d/%d", &d, &m, &y) != 3) return false;
    if (!isValidDate(d, m, y)) return false;
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d", d, m, y);
    s = buf;
    return true;
}

long dateToDays(const string& s)
{
    int d, m, y;
    if (sscanf(s.c_str(), "%d/%d/%d", &d, &m, &y) != 3) return -1;
    return daysFromCivil(y, m, d);
}

string today()
{
    time_t t = time(0);
    tm* l = localtime(&t);
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d", l->tm_mday, l->tm_mon + 1, l->tm_year + 1900);
    return string(buf);
}

string nowDateTime()
{
    time_t t = time(0);
    tm* l = localtime(&t);
    char buf[32];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d %02d:%02d:%02d",
             l->tm_mday, l->tm_mon + 1, l->tm_year + 1900, l->tm_hour, l->tm_min, l->tm_sec);
    return string(buf);
}

// Chấp nhận cả "dd/mm/yyyy" (giờ = 00:00:00) lẫn "dd/mm/yyyy HH:MM:SS"
long long dateTimeToSeconds(const string& s)
{
    int d, m, y, h = 0, mi = 0, sec = 0;
    int n = sscanf(s.c_str(), "%d/%d/%d %d:%d:%d", &d, &m, &y, &h, &mi, &sec);
    if (n < 3) return -1;
    return (long long)daysFromCivil(y, m, d) * 86400LL + h * 3600 + mi * 60 + sec;
}

// ---------- Nhập xuất ----------
string readLine(const string& prompt)
{
    cout << prompt;
    string s;
    if (!getline(cin, s)) { cout << endl; exit(0); }
    return trim(s);
}

int readInt(const string& prompt)
{
    while (true)
    {
        string s = readLine(prompt);
        if (toInt(s, -1) != -1) return atoi(s.c_str());
        cout << "Vui lòng nhập một số nguyên không âm.\n";
    }
}

void pauseScreen() { readLine("\nNhấn Enter để tiếp tục..."); }

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

MyList<int> parseSelection(const string& line, int maxIdx)
{
    MyList<int> r;
    if (toUpper(trim(line)) == "ALL")
    {
        for (int i = 0; i < maxIdx; i++) r.add(i);
        return r;
    }
    stringstream ss(line);
    string tok;
    while (ss >> tok)
    {
        int v = toInt(tok, -1);
        if (v >= 1 && v <= maxIdx) r.add(v - 1);
    }
    return r;
}

#ifdef _WIN32
bool waitKey(int ms)
{
    int t = 0;
    while (t < ms)
    {
        if (_kbhit()) { _getch(); return true; }
        Sleep(100);
        t += 100;
    }
    return false;
}
#else
bool waitKey(int ms)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0)
    {
        string tmp;
        getline(cin, tmp);
        return true;
    }
    return false;
}
#endif
```

## utils\Utils.h

```h
#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "../templates/MyList.h"
using namespace std;

// ---------- Chuỗi ----------
MyList<string> split(const string& s, char delim);   // giữ các trường rỗng
string trim(const string& s);
string toUpper(string s);
string pad(int n, int width);                        // pad(5,3) -> "005"
int toInt(const string& s, int def = 0);            // sai định dạng -> def

// ---------- Ngày tháng (dd/mm/yyyy) / thời gian (dd/mm/yyyy HH:MM:SS) ----------
bool normalizeDate(string& s);                       // kiểm tra + chuẩn hóa
long dateToDays(const string& s);                    // số ngày từ 1/1/1970, -1 nếu sai
string today();                                      // dd/mm/yyyy
string nowDateTime();                                // dd/mm/yyyy HH:MM:SS (thời điểm hiện tại)
long long dateTimeToSeconds(const string& s);        // "dd/mm/yyyy[ HH:MM:SS]" -> giây từ 1/1/1970, -1 nếu sai

// ---------- Nhập xuất console ----------
string readLine(const string& prompt);
int readInt(const string& prompt);
void pauseScreen();
void clearScreen();
MyList<int> parseSelection(const string& line, int maxIdx);  // "1 2 4" hoặc "ALL" -> chỉ số 0-based
bool waitKey(int ms);                                // chờ ms; true nếu người dùng bấm Enter

#endif
```
