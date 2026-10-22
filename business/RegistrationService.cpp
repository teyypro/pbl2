#include "RegistrationService.h"

RegistrationService::RegistrationService(DbContext& d) : db(d) {}

// UC01: Đăng ký giấy tờ -> tạo hồ sơ PENDING
bool RegistrationService::registerDocument(const string& maSV, const string& maVB, string& msg)
{
    SinhVien sv;
    if (!db.sinhVien.findByKey(maSV, sv)) { msg = "Sinh viên không tồn tại."; return false; }

    VanBan vb;
    if (!db.vanBan.findByKey(maVB, vb, false)) { msg = "Văn bản không tồn tại."; return false; }

    const MyList<DangKy>& mine = sv.dangKys.items();     // Navigation 1-N: hồ sơ của sinh viên
    for (int i = 0; i < mine.getSize(); i++)
    {
        if (mine[i].getMaVB() == maVB &&
            (mine[i].getTrangThai() == ST_PENDING || mine[i].getTrangThai() == ST_APPROVED))
        {
            msg = "Bạn đã có hồ sơ " + mine[i].getMaDK() + " cho văn bản này đang được xử lý.";
            return false;
        }
    }

    DangKy dk(db.dangKy.nextId("DK", 3), maSV, maVB, ST_PENDING, "", nowDateTime(), "", "");
    db.dangKy.add(dk);
    msg = "Đăng ký thành công. Mã đơn: " + dk.getMaDK();
    return true;
}

// UC02: Hồ sơ của một sinh viên.
// Dùng truy vấn của DbSet (thay vì sv.dangKys) để mỗi DangKy trả về có sẵn navigation vanBan
// (quy tắc nạp 1 cấp: phần tử của Collection không có navigation riêng).
MyList<DangKy> RegistrationService::getBySinhVien(const string& maSV)
{
    return db.dangKy.findBy("maSV", maSV);
}

// Lọc hồ sơ theo trạng thái và theo phòng (đi qua Navigation N-1: DangKy -> VanBan).
MyList<DangKy> RegistrationService::collect(const string& maPHG, const string& status, int mode)
{
    MyList<DangKy> list = db.dangKy.findBy("trangThai", status);
    MyList<DangKy> result;

    for (int i = 0; i < list.getSize(); i++)
    {
        VanBan* vb = list[i].vanBan.get();               // Navigation N-1
        if (vb == NULL || vb->getMaPHG() != maPHG) continue;

        bool coIn = !list[i].getNgayIn().empty();
        bool coNhan = !list[i].getNgayNhan().empty();

        if (mode == 1 && coIn) continue;
        if (mode == 2 && !coIn) continue;
        if (mode == 3 && (!coIn || coNhan)) continue;
        result.add(list[i]);
    }
    return result;
}

MyList<DangKy> RegistrationService::getPending(const string& maPHG)    { return collect(maPHG, ST_PENDING, 0); }
MyList<DangKy> RegistrationService::getForPrint(const string& maPHG)   { return collect(maPHG, ST_APPROVED, 1); }
MyList<DangKy> RegistrationService::getForReceive(const string& maPHG) { return collect(maPHG, ST_APPROVED, 3); }

bool RegistrationService::loadPending(const string& maDK, DangKy& dk, string& msg)
{
    if (!db.dangKy.findByKey(maDK, dk, false)) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
    if (dk.getTrangThai() != ST_PENDING)
    {
        msg = "Đơn " + maDK + " không ở trạng thái PENDING.";
        return false;
    }
    return true;
}

// UC03: Duyệt / từ chối
bool RegistrationService::approve(const string& maDK, string& msg)
{
    DangKy dk;
    if (!loadPending(maDK, dk, msg)) return false;
    dk.setTrangThai(ST_APPROVED);
    db.dangKy.update(dk);
    msg = "Đã duyệt đơn " + maDK + ".";
    return true;
}

bool RegistrationService::reject(const string& maDK, const string& reason, string& msg)
{
    DangKy dk;
    if (!loadPending(maDK, dk, msg)) return false;
    dk.setTrangThai(ST_REJECTED);
    dk.setLyDo(reason);
    db.dangKy.update(dk);
    msg = "Đã từ chối đơn " + maDK + ".";
    return true;
}

// UC04: Xuất danh sách cần in ra data/PrintList.txt. Trả về số hồ sơ, -1 nếu lỗi ghi file
int RegistrationService::exportPrintList(const string& maPHG)
{
    MyList<DangKy> ds = getForPrint(maPHG);
    if (ds.isEmpty()) return 0;

    MyList<string> lines;
    lines.add("DANH SACH CAN IN - " + nowDateTime());
    lines.add("STT | MaDon | MSSV | TenVanBan | NgayDangKy");
    for (int i = 0; i < ds.getSize(); i++)
    {
        lines.add(to_string(i + 1) + " | " + ds[i].getMaDK() + " | " + ds[i].getMaSV() + " | " +
                  ds[i].tenVanBan() + " | " + ds[i].getNgayDK());
    }
    return db.database().writeLines("PrintList.txt", lines) ? ds.getSize() : -1;
}

// UC05: Cập nhật ngày in = thời điểm bấm cập nhật (dd/mm/yyyy HH:MM:SS)
bool RegistrationService::setNgayIn(const string& maDK, const string& thoiGian, string& msg)
{
    DangKy dk;
    if (!db.dangKy.findByKey(maDK, dk, false)) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
    if (dk.getTrangThai() != ST_APPROVED) { msg = "Đơn " + maDK + " chưa được duyệt (APPROVED)."; return false; }
    if (!dk.getNgayIn().empty()) { msg = "Đơn " + maDK + " đã có ngày in (" + dk.getNgayIn() + ")."; return false; }

    dk.setNgayIn(thoiGian);
    db.dangKy.update(dk);
    msg = "Đã cập nhật ngày in cho đơn " + maDK + ": " + thoiGian;
    return true;
}

// UC06: Cập nhật ngày nhận = thời điểm bấm cập nhật (không được trước ngày in)
bool RegistrationService::setNgayNhan(const string& maDK, const string& thoiGian, string& msg)
{
    DangKy dk;
    if (!db.dangKy.findByKey(maDK, dk, false)) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
    if (dk.getTrangThai() != ST_APPROVED) { msg = "Đơn " + maDK + " chưa được duyệt (APPROVED)."; return false; }
    if (dk.getNgayIn().empty()) { msg = "Đơn " + maDK + " chưa có ngày in."; return false; }
    if (!dk.getNgayNhan().empty()) { msg = "Đơn " + maDK + " đã có ngày nhận (" + dk.getNgayNhan() + ")."; return false; }
    if (dateTimeToSeconds(thoiGian) < dateTimeToSeconds(dk.getNgayIn()))
    {
        msg = "Ngày nhận không được trước ngày in (" + dk.getNgayIn() + ").";
        return false;
    }

    dk.setNgayNhan(thoiGian);
    db.dangKy.update(dk);
    msg = "Đã cập nhật ngày nhận cho đơn " + maDK + ": " + thoiGian;
    return true;
}

// UC12: các giấy của sinh viên cần xử lý trả tại phòng maPHG
// (đã duyệt, đã có ngày nhận, chưa trả, thuộc phòng của quầy)
MyList<DangKy> RegistrationService::getForReturn(const string& maSV, const string& maPHG)
{
    MyList<DangKy> list = db.dangKy.findBy("maSV", maSV);
    MyList<DangKy> result;
    for (int i = 0; i < list.getSize(); i++)
    {
        if (list[i].getTrangThai() != ST_APPROVED) continue;
        if (list[i].getNgayNhan().empty()) continue;
        VanBan* vb = list[i].vanBan.get();               // Navigation N-1
        if (vb == NULL || vb->getMaPHG() != maPHG) continue;
        result.add(list[i]);
    }
    return result;
}

// UC12: Xác nhận trả giấy. Điều kiện: ngày nhận <= hôm nay <= ngày nhận + 30 ngày
// (so sánh theo ngày, phần giờ không ảnh hưởng)
bool RegistrationService::confirmReceive(const string& maDK, const string& maSVDangPhucVu, string& msg)
{
    DangKy dk;
    if (!db.dangKy.findByKey(maDK, dk, false)) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
    if (dk.getMaSV() != maSVDangPhucVu) { msg = "Đơn này không thuộc sinh viên đang được phục vụ."; return false; }
    if (dk.getTrangThai() != ST_APPROVED)
    {
        msg = "Đơn chưa ở trạng thái APPROVED (hiện tại: " + dk.getTrangThai() + ").";
        return false;
    }
    if (dk.getNgayNhan().empty()) { msg = "Đơn chưa có ngày nhận."; return false; }

    long t = dateToDays(today());
    long n = dateToDays(dk.getNgayNhan());
    if (t < n) { msg = "Chưa đến ngày nhận (" + dk.getNgayNhan() + ")."; return false; }
    if (t > n + 30) { msg = "Đã quá hạn nhận giấy (hạn: ngày nhận + 30 ngày)."; return false; }

    dk.setTrangThai(ST_DONE);
    db.dangKy.update(dk);
    msg = "Đã nhận thành công. Đơn " + maDK + " chuyển sang DONE.";
    return true;
}