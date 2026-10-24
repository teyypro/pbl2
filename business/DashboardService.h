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
