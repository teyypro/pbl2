#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>
using namespace std;

struct Appointment {
    int id = 0;
    string appointment_code;
    int patient_id = 0;
    int doctor_id = 0;
    int department_id = 0;
    int service_id = 0;
    string scheduled_at;
    string status;     // pending, confirmed, completed, cancelled
    string cancel_reason;
    int created_by = 0;
    string note;

    // Join fields
    string patient_name;
    string doctor_name;
    string department_name;
    string service_name;
};

#endif