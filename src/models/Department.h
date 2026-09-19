#ifndef DEPARTMENT_H
#define DEPARTMENT_H

#include <string>
using namespace std;

struct Department {
    int id = 0;
    string code;
    string name;
    string description;
    string floor_location;
    int head_doctor_id = 0;
    string status;     // active, inactive
};

#endif