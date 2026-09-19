#ifndef PATIENT_H
#define PATIENT_H

#include <string>
using namespace std;

struct Patient {
    int id = 0;
    int user_id = 0;
    string patient_code;
    string full_name;
    string dob;
    string gender;
    string phone;
    string email;
    string address;
    string bhyt_code;
    string medical_history;
    string allergies;
    string blood_type;
    string status;     // active, inactive
};

#endif