#ifndef COUNTERUI_H
#define COUNTERUI_H

#include "User.h"

class Counter : public User
{
private:
    AppContext& ctx;
    Quay* quay;                         // quầy của tài khoản (đối tượng trên RAM)
    bool ok;

    void hienThiHoSoSV(const string& maSV);
    void goiSoTiepTheo();
    void xemHoSo();
    void xacNhanTraGiay();

public:
    Counter(TaiKhoan* a, AppContext& c);
    void showMenu();
};

#endif
