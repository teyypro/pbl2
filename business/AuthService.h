#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../data/Repositories.h"

class AuthService
{
private:
    TaiKhoanRepo& repo;

public:
    AuthService(TaiKhoanRepo& r);

    // Đăng nhập: tra cứu TaiKhoan.txt
    bool login(const string& username, const string& password, TaiKhoan& out);
};

#endif
