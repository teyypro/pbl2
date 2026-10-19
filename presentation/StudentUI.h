#ifndef STUDENTUI_H
#define STUDENTUI_H

#include "User.h"

class Student : public User
{
private:
    AppContext& ctx;
    string maSV;

    void dangKyGiayTo();

    void xemHoSo();

public:
    Student(const TaiKhoan& a, AppContext& c);

    void showMenu();
};

#endif
