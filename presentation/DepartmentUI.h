#ifndef DEPARTMENTUI_H
#define DEPARTMENTUI_H

#include "User.h"

class Department : public User
{
private:
    AppContext& ctx;
    Phong* phong;                       // phòng của tài khoản (đối tượng trên RAM)
    bool ok;

    void listWithIndex(MyList<DangKy*>& ds);
    void xemChoDuyet();
    void duyetHoSo();
    void xuatDanhSachIn();
    void capNhatNgay(bool ngayIn);      // true: ngày in, false: ngày nhận

public:
    Department(TaiKhoan* a, AppContext& c);
    void showMenu();
};

#endif
