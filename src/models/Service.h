#ifndef SERVICE_H
#define SERVICE_H

#include <string>
using namespace std;

struct Service {
    int id = 0;
    string code;
    string name;
    string description;
    double price = 0;
    string unit;
    string category;
    int department_id = 0;
    string status;     // active, inactive

    // Join field
    string department_name;
};

#endif