#include "ProcessingStaff.h"

using namespace std;
ProcessingStaff::ProcessingStaff(const string& id, const string& username,
               const string& name, Department department)
    : User(id, username, name, UserRole::PROCESSING_STAFF), department(department) {}
Department ProcessingStaff::GetDepartment() const { return department; }
