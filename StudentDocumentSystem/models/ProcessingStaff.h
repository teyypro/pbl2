#pragma once
#include "User.h"

using namespace std;
class ProcessingStaff : public User {
private:
    Department department;
public:
    ProcessingStaff(const string& id, const string& username,
           const string& name, Department department);
    Department GetDepartment() const override;
};
