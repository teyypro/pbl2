#include "CounterStaff.h"

using namespace std;
CounterStaff::CounterStaff(const string& id, const string& username,
               const string& name, Department department)
    : User(id, username, name, UserRole::COUNTER_STAFF), department(department) {}
Department CounterStaff::GetDepartment() const { return department; }
