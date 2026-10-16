#pragma once
#include <string>

using namespace std;

enum class Department { NONE, CTSV, DAOTAO };
string ToString(Department value);
Department ParseDepartment(const string& value);
