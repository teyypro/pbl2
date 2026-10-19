#include "DashboardUI.h"

void DashboardUI::run(AppContext& ctx)
{
    while (true)
    {
        MyList<CounterDisplay> ds = ctx.dash.readCurrent();
        if (ds.isEmpty())
        {
            ctx.dash.refresh();
            ds = ctx.dash.readCurrent();
        }

        clearScreen();
        cout << "=====================================\n";
        cout << "        DASHBOARD HÀNG ĐỢI\n";
        cout << "=====================================\n";
        for (int i = 0; i < ds.getSize(); i++)
        {
            cout << " QUẦY " << ds[i].maQuay << " : " << (ds[i].active ? ds[i].soHienThi : string("ĐÓNG")) << endl;
        }
        cout << "=====================================\n";
        cout << "(Tự động cập nhật mỗi 3 giây - nhấn Enter để thoát)\n";

        if (waitKey(3000)) break;
    }
}
