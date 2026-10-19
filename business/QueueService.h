#ifndef QUEUESERVICE_H
#define QUEUESERVICE_H

#include "../data/Repositories.h"
#include "../templates/MyQueue.h"
#include "DashboardService.h"

// Quản lý hàng đợi nhiều quầy + chia tải tự động
class QueueService
{
private:
    PhieuRepo& phieuRepo;
    QuayRepo& quayRepo;
    SinhVienRepo& svRepo;
    DashboardService& dash;

    // Đếm số phiếu WAITING của một quầy (không load lại file)
    int countWaitingNoLoad(const string& maQuay);

public:
    QueueService(PhieuRepo& p, QuayRepo& q, SinhVienRepo& s, DashboardService& d);

    // UC07 + UC08: Guest nhập MSSV -> tạo phiếu, gán vào quầy ĐANG HOẠT ĐỘNG có ít người chờ nhất
    bool takeTicket(const string& maSV, PhieuHangDoi& out, string& msg);

    // UC10: Quầy gọi số tiếp theo (WAITING -> SERVING). Phiếu SERVING trước đó -> FINISHED
    bool callNext(const string& maQuay, PhieuHangDoi& out, string& msg);

    bool getServing(const string& maQuay, PhieuHangDoi& out);

    int countWaiting(const string& maQuay);

    bool findQuayByUsername(const string& username, Quay& out);
};

#endif
