#pragma once
#include "User.h"

using namespace std;
class CounterStaff : public User {
private:
    Department department;
public:
    CounterStaff(const string& id, const string& username,
           const string& name, Department department);
    Department GetDepartment() const override;
};
