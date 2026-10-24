#ifndef DASHBOARDUI_H
#define DASHBOARDUI_H

#include "../business/AppContext.h"

// UC09: Dashboard công khai, tự reload mỗi 3 giây từ CurrentServing.txt
class DashboardUI
{
public:
    static void run(AppContext& ctx);
};

#endif
