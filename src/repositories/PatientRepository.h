#ifndef PATIENT_REPOSITORY_H
#define PATIENT_REPOSITORY_H

#include "../database/FileManager.h"
#include "../models/Patient.h"
#include <vector>

class PatientRepository {
private:
    FileManager fm;
    string filename = "patients.txt";

    Patient parseLine(const string& line);
    string toLine(const Patient& p);

public:
    PatientRepository();

    vector<Patient> findAll();
    Patient findById(int id);
    Patient findByUserId(int userId);
    Patient findByCode(const string& code);
    int create(const Patient& patient);
    bool update(const Patient& patient);
    bool remove(int id);
};

#endif
