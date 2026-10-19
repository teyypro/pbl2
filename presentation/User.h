#ifndef USER_H
#define USER_H

#include "../business/AppContext.h"

// Lớp trừu tượng: đa hình qua showMenu()
class User
{
protected:
    TaiKhoan account;

public:
    User(const TaiKhoan& a);
    virtual ~User();
    virtual void showMenu() = 0;
};

// In một hồ sơ đăng ký
void printDangKy(RegistrationService& reg, const DangKy& d);

#endif
