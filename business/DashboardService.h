#ifndef DASHBOARDSERVICE_H
#define DASHBOARDSERVICE_H

#include "../data/Repositories.h"

struct CounterDisplay
{
    string maQuay;
    string soHienThi;
    bool active;
    CounterDisplay();
};

// Dashboard công khai: ghi / đọc CurrentServing.txt
class DashboardService
{
private:
    PhieuRepo& phieuRepo;
    QuayRepo& quayRepo;
    string path;

public:
    DashboardService(PhieuRepo& p, QuayRepo& q, const string& file);

    // Dựng lại CurrentServing.txt từ danh sách phiếu SERVING hiện tại
    void refresh();

    MyList<CounterDisplay> readCurrent();
};

#endif
