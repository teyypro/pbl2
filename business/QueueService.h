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
