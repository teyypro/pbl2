#include "MainUI.h"
#include "StudentUI.h"
#include "DepartmentUI.h"
#include "CounterUI.h"
#include "DashboardUI.h"
#include <iostream>

MainUI::MainUI(AppContext& c) : ctx(c) {}

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

    // Đa hình: tạo đối tượng theo Role
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

// UC07: Guest lấy số thứ tự: nhập MSSV -> chọn phòng -> chọn giấy tờ (hoặc "Tư vấn")
void MainUI::layThuTu()
{
    cout << "\n--- LẤY SỐ THỨ TỰ ---\n";
    string mssv = readLine("Nhập MSSV: ");

    string msg;
    if (!ctx.queue.canTakeTicket(mssv, msg)) { cout << msg << endl; pauseScreen(); return; }

    MyList<Phong> phongs = ctx.doc.getAllPhong();
    if (phongs.isEmpty()) { cout << "Chưa có phòng ban nào.\n"; pauseScreen(); return; }

    cout << "\n--- Chọn phòng ---\n";
    for (int i = 0; i < phongs.getSize(); i++) cout << i + 1 << ". " << phongs[i].getTenPhong() << endl;
    int p = readInt("Chọn phòng (0 = quay lại): ");
    if (p == 0) return;
    if (p < 1 || p > phongs.getSize()) { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); return; }
    Phong ph = phongs[p - 1];

    // Option 1 luôn là "Tư vấn" (nếu không chọn giấy nào thì tư vấn tại quầy của phòng)
    MyList<VanBan> vbs = ctx.doc.getVanBanByPhong(ph.getMaPHG());
    cout << "\n--- " << ph.getTenPhong() << " ---\n";
    cout << "1. Tư vấn\n";
    for (int i = 0; i < vbs.getSize(); i++) cout << i + 2 << ". " << vbs[i].getTenVB() << endl;
    int v = readInt("Chọn nội dung cần giải quyết (0 = quay lại): ");
    if (v == 0) return;
    if (v < 1 || v > vbs.getSize() + 1) { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); return; }
    string maVB = (v == 1) ? REQ_CONSULT : vbs[v - 2].getMaVB();

    PhieuHangDoi t;
    if (ctx.queue.takeTicket(mssv, ph.getMaPHG(), maVB, t, msg))
    {
        cout << "\n+---------------------------+\n";
        cout << "|        VÉ HÀNG ĐỢI        |\n";
        cout << "+---------------------------+\n";
        cout << "  MSSV      : " << t.getMaSV() << endl;
        cout << "  Số thứ tự : " << t.getDisplay() << endl;
        cout << "  Phòng     : " << ph.getTenPhong() << endl;
        cout << "  Quầy      : " << t.getMaQuay() << endl;
        cout << "  Yêu cầu   : " << t.tenYeuCau() << endl;
        cout << "  Thời gian : " << nowDateTime() << endl;
        cout << "+---------------------------+\n";
    }
    else
    {
        cout << msg << endl;
    }
    pauseScreen();
}

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