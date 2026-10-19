#include "QueueService.h"

int QueueService::countWaitingNoLoad(const string& maQuay)
{
    int c = 0;
    const MyList<PhieuHangDoi>& tk = phieuRepo.getAll();
    for (int i = 0; i < tk.getSize(); i++)
        if (tk[i].getMaQuay() == maQuay && tk[i].getTrangThai() == Q_WAITING) c++;
    return c;
}

QueueService::QueueService(PhieuRepo& p, QuayRepo& q, SinhVienRepo& s, DashboardService& d)
    : phieuRepo(p), quayRepo(q), svRepo(s), dash(d) {}

bool QueueService::takeTicket(const string& maSV, PhieuHangDoi& out, string& msg)
{
    svRepo.load();
    quayRepo.load();
    phieuRepo.load();

    SinhVien sv;
    if (!svRepo.findByKey(maSV, sv)) { msg = "MSSV không tồn tại."; return false; }

    const MyList<PhieuHangDoi>& tk = phieuRepo.getAll();
    int mx = 0;
    for (int i = 0; i < tk.getSize(); i++)
    {
        if (tk[i].getSoThuTu() > mx) mx = tk[i].getSoThuTu();
        if (tk[i].getMaSV() == maSV && tk[i].getTrangThai() == Q_WAITING)
        {
            out = tk[i];
            msg = "Bạn đang có số " + tk[i].getDisplay() + " chờ tại quầy " + tk[i].getMaQuay() + ".";
            return false;
        }
    }

    string best = "";
    int bestCnt = 0;
    const MyList<Quay>& qs = quayRepo.getAll();
    for (int i = 0; i < qs.getSize(); i++)
    {
        if (!qs[i].isActive()) continue;
        int c = countWaitingNoLoad(qs[i].getMaQuay());
        if (best.empty() || c < bestCnt) { best = qs[i].getMaQuay(); bestCnt = c; }
    }
    if (best.empty()) { msg = "Hiện không có quầy nào đang hoạt động."; return false; }

    PhieuHangDoi p("T" + pad(mx + 1, 4), mx + 1, maSV, best, Q_WAITING);
    phieuRepo.add(p);
    out = p;
    return true;
}

bool QueueService::callNext(const string& maQuay, PhieuHangDoi& out, string& msg)
{
    phieuRepo.load();
    const MyList<PhieuHangDoi>& tk = phieuRepo.getAll();

    // Dựng MyQueue từ các phiếu WAITING của quầy (theo thứ tự số tăng dần)
    MyQueue<PhieuHangDoi> q;
    for (int i = 0; i < tk.getSize(); i++)
        if (tk[i].getMaQuay() == maQuay && tk[i].getTrangThai() == Q_WAITING) q.enqueue(tk[i]);

    if (q.isEmpty())
    {
        msg = "Hàng đợi của quầy " + maQuay + " đang trống.";
        return false;
    }

    for (int i = 0; i < tk.getSize(); i++)
    {
        if (tk[i].getMaQuay() == maQuay && tk[i].getTrangThai() == Q_SERVING)
        {
            PhieuHangDoi old = tk[i];
            old.setTrangThai(Q_FINISHED);
            phieuRepo.update(old);
        }
    }

    PhieuHangDoi next = q.peek();
    q.dequeue();
    next.setTrangThai(Q_SERVING);
    phieuRepo.update(next);
    dash.refresh();
    out = next;
    return true;
}

bool QueueService::getServing(const string& maQuay, PhieuHangDoi& out)
{
    phieuRepo.load();
    const MyList<PhieuHangDoi>& tk = phieuRepo.getAll();
    for (int i = 0; i < tk.getSize(); i++)
    {
        if (tk[i].getMaQuay() == maQuay && tk[i].getTrangThai() == Q_SERVING)
        {
            out = tk[i];
            return true;
        }
    }
    return false;
}

int QueueService::countWaiting(const string& maQuay)
{
    phieuRepo.load();
    return countWaitingNoLoad(maQuay);
}

bool QueueService::findQuayByUsername(const string& username, Quay& out)
{
    quayRepo.load();
    const MyList<Quay>& qs = quayRepo.getAll();
    for (int i = 0; i < qs.getSize(); i++)
    {
        if (qs[i].getUsername() == username) { out = qs[i]; return true; }
    }
    return false;
}
