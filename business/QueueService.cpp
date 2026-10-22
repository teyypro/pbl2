#include "QueueService.h"
#include "../templates/MyQueue.h"

QueueService::QueueService(DbContext& d, DashboardService& ds) : db(d), dash(ds) {}

bool QueueService::getQuay(const string& maQuay, Quay& out)
{
    return db.quay.findByKey(maQuay, out);
}

// Navigation 1-N: Quay -> các PhieuHangDoi của quầy
MyList<PhieuHangDoi> QueueService::ticketsOf(const string& maQuay)
{
    MyList<PhieuHangDoi> result;
    Quay q;
    if (db.quay.findByKey(maQuay, q)) result = q.phieus.items();
    return result;
}

int QueueService::countWaiting(const string& maQuay)
{
    MyList<PhieuHangDoi> list = ticketsOf(maQuay);
    int c = 0;
    for (int i = 0; i < list.getSize(); i++)
        if (list[i].getTrangThai() == Q_WAITING) c++;
    return c;
}

bool QueueService::canTakeTicket(const string& maSV, string& msg)
{
    SinhVien sv;
    if (!db.sinhVien.findByKey(maSV, sv)) { msg = "MSSV không tồn tại."; return false; }

    const MyList<PhieuHangDoi>& mine = sv.phieus.items();    // Navigation 1-N: phiếu của sinh viên
    for (int i = 0; i < mine.getSize(); i++)
    {
        if (mine[i].getTrangThai() == Q_WAITING)
        {
            msg = "Bạn đang có số " + mine[i].getDisplay() + " chờ tại quầy " + mine[i].getMaQuay() + ".";
            return false;
        }
    }
    return true;
}

// UC07 + UC08: chọn phòng + nội dung (giấy tờ / tư vấn) -> gán vào quầy đang hoạt động
// của PHÒNG đó có ít người chờ nhất
bool QueueService::takeTicket(const string& maSV, const string& maPHG, const string& maVB,
                              PhieuHangDoi& out, string& msg)
{
    if (!canTakeTicket(maSV, msg)) return false;

    Phong ph;
    if (!db.phong.findByKey(maPHG, ph, false)) { msg = "Phòng không tồn tại."; return false; }

    // Nội dung: "Tư vấn" hoặc một văn bản thuộc đúng phòng đã chọn
    string request = REQ_CONSULT;
    if (maVB != REQ_CONSULT)
    {
        VanBan vb;
        if (!db.vanBan.findByKey(maVB, vb, false) || vb.getMaPHG() != maPHG)
        {
            msg = "Giấy tờ không thuộc phòng đã chọn.";
            return false;
        }
        request = maVB;
    }

    // Chỉ xét các quầy của phòng này đang hoạt động
    MyList<Quay> all = db.quay.findBy("maPHG", maPHG, false);
    MyList<Quay> active;
    for (int i = 0; i < all.getSize(); i++)
        if (all[i].isActive()) active.add(all[i]);

    if (active.isEmpty())
    {
        msg = "Hiện " + ph.getTenPhong() + " không có quầy nào đang hoạt động.";
        return false;
    }

    int best = 0;
    int bestCount = countWaiting(active[0].getMaQuay());
    for (int i = 1; i < active.getSize(); i++)
    {
        int c = countWaiting(active[i].getMaQuay());
        if (c < bestCount) { best = i; bestCount = c; }
    }

    string id = db.phieu.nextId("T", 4);                      // T0001, T0002, ...
    PhieuHangDoi p(id, toInt(id.substr(1)), maSV, active[best].getMaQuay(), Q_WAITING, request);
    db.phieu.add(p);

    if (!db.phieu.findByKey(id, out)) out = p;                // nạp lại để có navigation vanBan
    dash.refresh();
    return true;
}

// UC10: quầy gọi số tiếp theo (WAITING -> SERVING); phiếu SERVING trước đó -> FINISHED
// Nếu không còn ai chờ: kết thúc luôn phiếu đang SERVING hiện tại (nếu có) và báo trống.
bool QueueService::callNext(const string& maQuay, PhieuHangDoi& out, string& msg)
{
    MyList<PhieuHangDoi> list = ticketsOf(maQuay);

    // Phiếu WAITING vào hàng đợi theo thứ tự số thứ tự tăng dần
    MyList<PhieuHangDoi> waiting;
    for (int i = 0; i < list.getSize(); i++)
        if (list[i].getTrangThai() == Q_WAITING) waiting.add(list[i]);
    for (int a = 0; a < waiting.getSize(); a++)
        for (int b = a + 1; b < waiting.getSize(); b++)
            if (waiting[b].getSoThuTu() < waiting[a].getSoThuTu())
            {
                PhieuHangDoi tmp = waiting[a]; waiting[a] = waiting[b]; waiting[b] = tmp;
            }

    MyQueue<PhieuHangDoi> q;                                  // hàng đợi của quầy
    for (int i = 0; i < waiting.getSize(); i++) q.enqueue(waiting[i]);

    // Trường hợp 1: không còn ai chờ
    if (q.isEmpty())
    {
        bool hadServing = false;
        for (int i = 0; i < list.getSize(); i++)
        {
            if (list[i].getTrangThai() == Q_SERVING)
            {
                PhieuHangDoi old = list[i];
                old.setTrangThai(Q_FINISHED);
                db.phieu.update(old);
                hadServing = true;
            }
        }

        dash.refresh();
        msg = hadServing
            ? "Hàng đợi của quầy " + maQuay + " đã hết. Đã kết thúc phiếu đang phục vụ."
            : "Hàng đợi của quầy " + maQuay + " đang trống.";
        return false;
    }

    // Trường hợp 2: có người chờ -> phiếu SERVING cũ chuyển FINISHED
    for (int i = 0; i < list.getSize(); i++)
    {
        if (list[i].getTrangThai() == Q_SERVING)
        {
            PhieuHangDoi old = list[i];
            old.setTrangThai(Q_FINISHED);
            db.phieu.update(old);
        }
    }

    PhieuHangDoi next = q.peek();
    q.dequeue();
    next.setTrangThai(Q_SERVING);
    db.phieu.update(next);

    dash.refresh();
    if (!db.phieu.findByKey(next.getMaPhieu(), out)) out = next;   // có navigation vanBan
    return true;
}

bool QueueService::getServing(const string& maQuay, PhieuHangDoi& out)
{
    MyList<PhieuHangDoi> list = ticketsOf(maQuay);
    for (int i = 0; i < list.getSize(); i++)
    {
        if (list[i].getTrangThai() == Q_SERVING)
        {
            // phần tử của Collection không có navigation -> nạp lại từ DbSet
            if (!db.phieu.findByKey(list[i].getMaPhieu(), out)) out = list[i];
            return true;
        }
    }
    return false;
}