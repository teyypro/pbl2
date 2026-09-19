#ifndef PATIENT_REPOSITORY_H
#define PATIENT_REPOSITORY_H

#include "FileManager.h"
#include "Patient.h"
#include <vector>

class PatientRepository {
private:
    FileManager fm;
    string filename = "patients.txt";

    Patient parseLine(const string& line) {
        Patient p;
        vector<string> f = FileManager::split(line);
        if (f.size() >= 14) {
            p.id = stoi(f[0]);
            p.user_id = stoi(f[1]);
            p.patient_code = f[2];
            p.full_name = f[3];
            p.dob = f[4];
            p.gender = f[5];
            p.phone = f[6];
            p.email = f[7];
            p.address = f[8];
            p.bhyt_code = f[9];
            p.medical_history = f[10];
            p.allergies = f[11];
            p.blood_type = f[12];
            p.status = f[13];
        }
        return p;
    }

    string toLine(const Patient& p) {
        return to_string(p.id) + "|" + to_string(p.user_id) + "|" + p.patient_code + "|"
             + p.full_name + "|" + p.dob + "|" + p.gender + "|" + p.phone + "|"
             + p.email + "|" + p.address + "|" + p.bhyt_code + "|"
             + p.medical_history + "|" + p.allergies + "|" + p.blood_type + "|" + p.status;
    }

public:
    PatientRepository() {}

    vector<Patient> findAll() {
        vector<Patient> patients;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            patients.push_back(parseLine(line));
        }
        return patients;
    }

    Patient findById(int id) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Patient p = parseLine(line);
            if (p.id == id) return p;
        }
        return Patient();
    }

    Patient findByUserId(int userId) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Patient p = parseLine(line);
            if (p.user_id == userId) return p;
        }
        return Patient();
    }

    Patient findByCode(const string& code) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Patient p = parseLine(line);
            if (p.patient_code == code) return p;
        }
        return Patient();
    }

    int create(const Patient& patient) {
        Patient p = patient;
        p.id = fm.getNextId(filename);
        fm.appendLine(filename, toLine(p));
        return p.id;
    }

    bool update(const Patient& patient) {
        string header = fm.readHeader(filename);
        vector<string> lines = fm.readAllLines(filename);
        vector<string> newLines;
        bool found = false;
        for (auto& line : lines) {
            Patient p = parseLine(line);
            if (p.id == patient.id) {
                newLines.push_back(toLine(patient));
                found = true;
            } else {
                newLines.push_back(line);
            }
        }
        if (found) fm.writeAllLines(filename, header, newLines);
        return found;
    }

    bool remove(int id) {
        Patient p = findById(id);
        if (p.id == 0) return false;
        p.status = "inactive";
        return update(p);
    }
};

#endif
