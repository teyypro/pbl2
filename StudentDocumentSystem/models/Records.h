#pragma once
#include <string>
#include "../enums/UserRole.h"
#include "../enums/Department.h"

using namespace std;

// Ban ghi TXT, khong phai doi tuong phien dang nhap.
struct AccountRecord {
    string userId, username, passwordSalt, passwordHash, fullName;
    UserRole role = UserRole::STUDENT;
    Department department = Department::NONE;
};
struct StudentRecord {
    string studentId, fullName, className, faculty;
};
