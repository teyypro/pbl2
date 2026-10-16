#include "User.h"

using namespace std;
User::User(const string& id, const string& username,
           const string& name, UserRole role)
    : userId(id), username(username), fullName(name), role(role) {}
const string& User::GetUserId() const { return userId; }
const string& User::GetUsername() const { return username; }
const string& User::GetFullName() const { return fullName; }
UserRole User::GetRole() const { return role; }
Department User::GetDepartment() const { return Department::NONE; }
string User::GetStudentId() const { return ""; }
