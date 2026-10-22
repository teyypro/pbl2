#ifndef QUEUESERVICE_H
#define QUEUESERVICE_H

#include "../orm/DbContext.h"
#include "../models/Constants.h"
#include "DashboardService.h"

// Hàng đợi nhiều quầy + chia tải tự động (theo từng phòng)
class QueueService
{
private:
    DbContext& db;
    DashboardService& dash;

    MyList<PhieuHangDoi> ticketsOf(const string& maQuay);   // các phiếu của một quầy

public:
    QueueService(DbContext& db, DashboardService& dash);

    bool getQuay(const string& maQuay, Quay& out);                            // Quay đầy đủ quan hệ

    // Kiểm tra sinh viên có được lấy số không (tồn tại, chưa có số đang chờ)
    bool canTakeTicket(const string& maSV, string& msg);

    // UC07 + UC08: lấy số theo PHÒNG và NỘI DUNG (maVB = REQ_CONSULT nếu chọn "Tư vấn")
    // Phiếu được gán vào quầy ĐANG HOẠT ĐỘNG của phòng đó có ít người chờ nhất
    bool takeTicket(const string& maSV, const string& maPHG, const string& maVB,
                    PhieuHangDoi& out, string& msg);

    bool callNext(const string& maQuay, PhieuHangDoi& out, string& msg);      // UC10
    bool getServing(const string& maQuay, PhieuHangDoi& out);                 // phiếu có đủ navigation
    int countWaiting(const string& maQuay);
};

#endif