#ifndef MAINUI_H
#define MAINUI_H

#include "../business/AppContext.h"

class MainUI
{
private:
    AppContext& ctx;

    void dangNhap();
    void layThuTu();

public:
    MainUI(AppContext& c);
    void run();
};

#endif
