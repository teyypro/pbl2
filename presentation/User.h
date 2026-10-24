#ifndef USER_H
#define USER_H

#include "../business/AppContext.h"

// Lớp trừu tượng: đa hình qua showMenu()
class User
{
protected:
    TaiKhoan* account;      // tài khoản đăng nhập (đối tượng trên RAM, đã có sẵn các quan hệ)

public:
    User(TaiKhoan* a);
    virtual ~User() {}
    virtual void showMenu() = 0;
};

// In một hồ sơ đăng ký ra màn hình
void printDangKy(const DangKy* d);

#endif
