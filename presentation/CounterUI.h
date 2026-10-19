#ifndef COUNTERUI_H
#define COUNTERUI_H

#include "User.h"

class Counter : public User
{
private:
    AppContext& ctx;
    Quay quay;
    bool ok;

    void hienThiHoSoSV(const string& maSV);

    // UC10 + UC11
    void goiSoTiepTheo();

    void xemHoSo();

    // UC12
    void xacNhanTraGiay();

public:
    Counter(const TaiKhoan& a, AppContext& c);

    void showMenu();
};

#endif
