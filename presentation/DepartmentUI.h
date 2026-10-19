#ifndef DEPARTMENTUI_H
#define DEPARTMENTUI_H

#include "User.h"

class Department : public User
{
private:
    AppContext& ctx;
    Phong phong;
    bool ok;

    void listWithIndex(MyList<DangKy>& ds);

    void xemChoDuyet();

    void duyetHoSo();

    void xuatDanhSachIn();

    // ngayIn = true: cập nhật ngày in; false: cập nhật ngày nhận
    void capNhatNgay(bool ngayIn);

public:
    Department(const TaiKhoan& a, AppContext& c);

    void showMenu();
};

#endif
