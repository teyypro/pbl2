#pragma once
#include <string>
#include "../enums/UserRole.h"
#include "../enums/Department.h"

using namespace std;

class User {
private:
    string userId;
    string username;
    string fullName;
    UserRole role;
public:
    User(const string& id, const string& username,
         const string& name, UserRole role);
    virtual ~User() = default;
    const string& GetUserId() const;
    const string& GetUsername() const;
    const string& GetFullName() const;
    UserRole GetRole() const;
    virtual Department GetDepartment() const;
    virtual string GetStudentId() const;
};
