#ifndef MEDICAL_RECORD_H
#define MEDICAL_RECORD_H

#include <string>
using namespace std;

struct MedicalRecord {
    int id = 0;
    string record_code;
    int patient_id = 0;
    int doctor_id = 0;
    int appointment_id = 0;
    string visit_date;
    string symptoms;
    string clinical_findings;
    string diagnosis;
    string diagnosis_code;
    string advice;
    string follow_up_date;
    double height = 0;
    double weight = 0;
    string blood_pressure;
    double temperature = 0;
    string status;     // draft, finalized, completed

    // Join fields
    string patient_name;
    string doctor_name;
};

#endif