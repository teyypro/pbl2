#include "StudentUI.h"
#include <iostream>

Student::Student(const TaiKhoan& a, AppContext& c) : User(a), ctx(c)
{
    SinhVien* sv = account.sinhVien.get();          // Navigation 1-1: TaiKhoan -> SinhVien
    maSV = (sv != NULL) ? sv->getMaSV() : string("");
}

// UC01
void Student::dangKyGiayTo()
{
    MyList<Phong> ds = ctx.doc.getAllPhong();
    if (ds.isEmpty()) { cout << "Chưa có phòng ban nào.\n"; pauseScreen(); return; }

    cout << "\n--- Chọn phòng ---\n";
    for (int i = 0; i < ds.getSize(); i++) cout << i + 1 << ". " << ds[i].getTenPhong() << endl;
    int p = readInt("Chọn phòng (0 = quay lại): ");
    if (p < 1 || p > ds.getSize()) return;

    MyList<VanBan> vb = ctx.doc.getVanBanByPhong(ds[p - 1].getMaPHG());
    if (vb.isEmpty()) { cout << "Phòng này chưa có văn bản nào.\n"; pauseScreen(); return; }

    cout << "\n--- " << ds[p - 1].getTenPhong() << " ---\n";
    for (int i = 0; i < vb.getSize(); i++) cout << i + 1 << ". " << vb[i].getTenVB() << endl;
    int v = readInt("Chọn văn bản (0 = quay lại): ");
    if (v < 1 || v > vb.getSize()) return;

    string yn = toUpper(readLine("Xác nhận đăng ký \"" + vb[v - 1].getTenVB() + "\"? (Y/N): "));
    if (yn != "Y") { cout << "Đã hủy.\n"; pauseScreen(); return; }

    string msg;
    ctx.reg.registerDocument(maSV, vb[v - 1].getMaVB(), msg);
    cout << msg << endl;
    pauseScreen();
}

// UC02
void Student::xemHoSo()
{
    MyList<DangKy> ds = ctx.reg.getBySinhVien(maSV);
    cout << "\n=== HỒ SƠ ĐÃ ĐĂNG KÝ ===\n";
    if (ds.isEmpty()) cout << "Bạn chưa đăng ký hồ sơ nào.\n";
    for (int i = 0; i < ds.getSize(); i++)
    {
        printDangKy(ds[i]);
        cout << endl;
    }
    pauseScreen();
}

void Student::showMenu()
{
    if (maSV.empty())
    {
        cout << "Tài khoản chưa được gắn với sinh viên nào.\n";
        pauseScreen();
        return;
    }
    while (true)
    {
        clearScreen();
        cout << "====== SINH VIÊN: " << maSV << " ======\n";
        cout << "1. Đăng ký giấy tờ\n";
        cout << "2. Xem hồ sơ đã đăng ký\n";
        cout << "3. Đăng xuất\n";
        int c = readInt("Chọn: ");
        if (c == 1) dangKyGiayTo();
        else if (c == 2) xemHoSo();
        else if (c == 3) return;
        else { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); }
    }
}