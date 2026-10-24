#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../orm/DbContext.h"

class AuthService
{
private:
    DbContext& db;

public:
    AuthService(DbContext& db);
    TaiKhoan* login(const string& username, const string& password);   // NULL nếu sai
};

#endif
