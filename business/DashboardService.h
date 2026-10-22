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

    void refresh();                        // dựng lại CurrentServing.txt từ các phiếu SERVING
    MyList<CounterDisplay> readCurrent();  // đọc CurrentServing.txt
};

#endif