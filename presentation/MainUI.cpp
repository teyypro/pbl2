#include "MainUI.h"

void MainUI::dangNhap()
{
    cout << "\n--- ĐĂNG NHẬP ---\n";
    string u = readLine("Username: ");
    string p = readLine("Password: ");

    TaiKhoan acc;
    if (!ctx.auth.login(u, p, acc))
    {
        cout << "Sai username hoặc password.\n";
        pauseScreen();
        return;
    }

    // Đa hình: điều hướng theo Role
    User* user = NULL;
    if (acc.getRole() == ROLE_STUDENT) user = new Student(acc, ctx);
    else if (acc.getRole() == ROLE_PHONG) user = new Department(acc, ctx);
    else if (acc.getRole() == ROLE_QUAY) user = new Counter(acc, ctx);

    if (user != NULL)
    {
        user->showMenu();
        delete user;
    }
    else
    {
        cout << "Vai trò không hợp lệ: " << acc.getRole() << endl;
        pauseScreen();
    }
}

void MainUI::layThuTu()
{
    cout << "\n--- LẤY SỐ THỨ TỰ ---\n";
    string mssv = readLine("Nhập MSSV: ");
    PhieuHangDoi p;
    string msg;
    if (ctx.queue.takeTicket(mssv, p, msg))
    {
        cout << "\n+---------------------------+\n";
        cout << "|        VÉ HÀNG ĐỢI        |\n";
        cout << "+---------------------------+\n";
        cout << "  MSSV      : " << p.getMaSV() << endl;
        cout << "  Số thứ tự : " << p.getDisplay() << endl;
        cout << "  Quầy      : " << p.getMaQuay() << endl;
        cout << "+---------------------------+\n";
    }
    else
    {
        cout << msg << endl;
    }
    pauseScreen();
}

MainUI::MainUI(AppContext& c) : ctx(c) {}

void MainUI::run()
{
    while (true)
    {
        clearScreen();
        cout << "====================================\n";
        cout << " STUDENT DOCUMENT MANAGEMENT SYSTEM\n";
        cout << "====================================\n\n";
        cout << "1. Đăng nhập\n";
        cout << "2. Lấy số thứ tự\n";
        cout << "3. Dashboard công khai\n";
        cout << "0. Thoát\n";
        int c = readInt("Chọn: ");
        if (c == 1) dangNhap();
        else if (c == 2) layThuTu();
        else if (c == 3) DashboardUI::run(ctx);
        else if (c == 0) return;
        else { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); }
    }
}
