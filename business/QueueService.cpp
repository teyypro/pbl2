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
