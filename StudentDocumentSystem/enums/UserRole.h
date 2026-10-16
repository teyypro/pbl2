#pragma once
#include <string>

using namespace std;

enum class UserRole { STUDENT, PROCESSING_STAFF, COUNTER_STAFF };
string ToString(UserRole value);
UserRole ParseUserRole(const string& value);
