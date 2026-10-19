#ifndef MAINUI_H
#define MAINUI_H

#include "StudentUI.h"
#include "DepartmentUI.h"
#include "CounterUI.h"
#include "DashboardUI.h"

class MainUI
{
private:
    AppContext& ctx;

    void dangNhap();

    // UC07: Guest lấy số thứ tự
    void layThuTu();

public:
    MainUI(AppContext& c);

    void run();
};

#endif
