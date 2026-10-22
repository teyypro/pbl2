#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../orm/DbContext.h"

class AuthService
{
private:
    DbContext& db;

public:
    AuthService(DbContext& db);
    bool login(const string& username, const string& password, TaiKhoan& out);
};

#endif