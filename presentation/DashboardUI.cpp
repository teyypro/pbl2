#include "DashboardUI.h"
#include <iostream>

void DashboardUI::run(AppContext& ctx)
{
    while (true)
    {
        ctx.dash.refresh();
        MyList<CounterDisplay> ds = ctx.dash.readCurrent();

        clearScreen();
        cout << "=================================================\n";
        cout << "              DASHBOARD HÀNG ĐỢI\n";
        cout << "=================================================\n";
        for (int i = 0; i < ds.getSize(); i++)
        {
            cout << " QUAY " << ds[i].maQuay << " : "
                 << (ds[i].active ? ds[i].soHienThi : string("DONG"));

            if (ds[i].active && !ds[i].danhSachCho.isEmpty())
            {
                cout << "Tiep theo: ";
                for (int j = 0; j < ds[i].danhSachCho.getSize(); j++)
                {
                    if (j > 0) cout << ", ";
                    cout << ds[i].danhSachCho[j];
                }
                cout << endl;
            }
            else if (ds[i].active)
            {
                cout << "Trong" << endl;
            }
            cout << "-------------------------------------------------\n";
        }
        cout << "(Tu dong cap nhat moi 3 giay - nhan Enter de thoat)\n";

        if (waitKey(3000)) break;
    }
}