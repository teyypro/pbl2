#ifndef DOCTOR_H
#define DOCTOR_H

#include <string>
using namespace std;

struct Doctor {
    int id = 0;
    int user_id = 0;
    int department_id = 0;
    string specialty;
    string dob;
    double salary = 0;
    int years_experience = 0;
    string qualification;
    string status;     // active, inactive, on_leave

    // Join fields (tu bang users va departments)
    string full_name;
    string department_name;
};

#endif