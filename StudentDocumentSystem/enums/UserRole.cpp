#include "UserRole.h"
#include <stdexcept>

using namespace std;

string ToString(UserRole value) {
    switch (value) {
    case UserRole::STUDENT: return "STUDENT";
    case UserRole::PROCESSING_STAFF: return "PROCESSING_STAFF";
    case UserRole::COUNTER_STAFF: return "COUNTER_STAFF";
    }
    throw runtime_error("Gia tri enum khong hop le.");
}
UserRole ParseUserRole(const string& value) {
    if (value == "STUDENT") return UserRole::STUDENT;
    if (value == "PROCESSING_STAFF") return UserRole::PROCESSING_STAFF;
    if (value == "COUNTER_STAFF") return UserRole::COUNTER_STAFF;
    throw runtime_error("UserRole khong hop le: " + value);
}
