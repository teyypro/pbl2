#ifndef PRESCRIPTION_H
#define PRESCRIPTION_H

#include <string>
#include <vector>
using namespace std;

struct PrescriptionDetail {
    int id = 0;
    int prescription_id = 0;
    int medicine_id = 0;
    string dosage;
    int quantity = 0;
    int days = 0;
    string frequency;
    string route;       // oral, injection, topical...
    bool morning = false;
    bool noon = false;
    bool evening = false;
    bool night = false;
    string note;
    string status;

    // Join field
    string medicine_name;
    string medicine_unit;
    double medicine_price = 0;
};

struct Prescription {
    int id = 0;
    string prescription_code;
    int medical_record_id = 0;
    int patient_id = 0;
    int doctor_id = 0;
    string prescribed_at;
    string note;
    string status;     // pending, dispensed, completed, cancelled

    // Join fields
    string patient_name;
    string doctor_name;

    // Chi tiet don thuoc
    vector<PrescriptionDetail> details;
};

#endif