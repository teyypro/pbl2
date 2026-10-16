#include "Department.h"
#include <stdexcept>

using namespace std;

string ToString(Department value) {
    switch (value) {
    case Department::NONE: return "NONE";
    case Department::CTSV: return "CTSV";
    case Department::DAOTAO: return "DAOTAO";
    }
    throw runtime_error("Gia tri enum khong hop le.");
}
Department ParseDepartment(const string& value) {
    if (value == "NONE") return Department::NONE;
    if (value == "CTSV") return Department::CTSV;
    if (value == "DAOTAO") return Department::DAOTAO;
    throw runtime_error("Department khong hop le: " + value);
}
