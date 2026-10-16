#pragma once
#include "ServiceContext.h"
#include <memory>

using namespace std;
class AuthService {
private:
    ServiceContext& context;
public:
    explicit AuthService(ServiceContext& context);
    void RegisterStudent(const string& studentId, const string& password, const string& confirmation);
    unique_ptr<User> Login(const string& username, const string& password);
    void Logout(unique_ptr<User>& session);
};
