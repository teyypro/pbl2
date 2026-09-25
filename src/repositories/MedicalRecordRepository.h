#ifndef MEDICAL_RECORD_REPOSITORY_H
#define MEDICAL_RECORD_REPOSITORY_H

#include "../database/FileManager.h"
#include "../models/MedicalRecord.h"
#include <vector>

class MedicalRecordRepository {
private:
    FileManager fm;
    string filename = "medical_records.txt";

    MedicalRecord parseLine(const string& line) {
        MedicalRecord r;
        vector<string> f = FileManager::split(line);
        if (f.size() >= 17) {
            r.id = stoi(f[0]);
            r.record_code = f[1];
            r.patient_id = stoi(f[2]);
            r.doctor_id = stoi(f[3]);
            r.appointment_id = stoi(f[4]);
            r.visit_date = f[5];
            r.symptoms = f[6];
            r.clinical_findings = f[7];
            r.diagnosis = f[8];
            r.diagnosis_code = f[9];
            r.advice = f[10];
            r.follow_up_date = f[11];
            r.height = f[12].empty() ? 0 : stod(f[12]);
            r.weight = f[13].empty() ? 0 : stod(f[13]);
            r.blood_pressure = f[14];
            r.temperature = f[15].empty() ? 0 : stod(f[15]);
            r.status = f[16];
        }
        return r;
    }

    string toLine(const MedicalRecord& r) {
        return to_string(r.id) + "|" + r.record_code + "|" + to_string(r.patient_id) + "|"
             + to_string(r.doctor_id) + "|" + to_string(r.appointment_id) + "|"
             + r.visit_date + "|" + r.symptoms + "|" + r.clinical_findings + "|"
             + r.diagnosis + "|" + r.diagnosis_code + "|" + r.advice + "|"
             + r.follow_up_date + "|" + to_string(r.height) + "|" + to_string(r.weight) + "|"
             + r.blood_pressure + "|" + to_string(r.temperature) + "|" + r.status;
    }

    void loadNames(vector<MedicalRecord>& records) {
        FileManager fmHelper;
        vector<string> patientLines = fmHelper.readAllLines("patients.txt");
        vector<string> userLines = fmHelper.readAllLines("users.txt");
        vector<string> doctorLines = fmHelper.readAllLines("doctors.txt");

        for (auto& rec : records) {
            for (auto& pl : patientLines) {
                vector<string> pf = FileManager::split(pl);
                if (pf.size() >= 4 && stoi(pf[0]) == rec.patient_id) {
                    rec.patient_name = pf[3];
                    break;
                }
            }
            int userId = 0;
            for (auto& dl : doctorLines) {
                vector<string> df = FileManager::split(dl);
                if (df.size() >= 2 && stoi(df[0]) == rec.doctor_id) {
                    userId = stoi(df[1]);
                    break;
                }
            }
            for (auto& ul : userLines) {
                vector<string> uf = FileManager::split(ul);
                if (uf.size() >= 4 && stoi(uf[0]) == userId) {
                    rec.doctor_name = uf[3];
                    break;
                }
            }
        }
    }

public:
    MedicalRecordRepository() {}

    vector<MedicalRecord> findAll() {
        vector<MedicalRecord> records;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            records.push_back(parseLine(line));
        }
        loadNames(records);
        return records;
    }

    MedicalRecord findById(int id) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            MedicalRecord r = parseLine(line);
            if (r.id == id) {
                vector<MedicalRecord> v = {r};
                loadNames(v);
                return v[0];
            }
        }
        return MedicalRecord();
    }

    vector<MedicalRecord> findByPatient(int patientId) {
        vector<MedicalRecord> records;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            MedicalRecord r = parseLine(line);
            if (r.patient_id == patientId) records.push_back(r);
        }
        loadNames(records);
        return records;
    }

    vector<MedicalRecord> findByDoctor(int doctorId) {
        vector<MedicalRecord> records;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            MedicalRecord r = parseLine(line);
            if (r.doctor_id == doctorId) records.push_back(r);
        }
        loadNames(records);
        return records;
    }

    int create(const MedicalRecord& record) {
        MedicalRecord r = record;
        r.id = fm.getNextId(filename);
        fm.appendLine(filename, toLine(r));
        return r.id;
    }

    bool update(const MedicalRecord& record) {
        string header = fm.readHeader(filename);
        vector<string> lines = fm.readAllLines(filename);
        vector<string> newLines;
        bool found = false;
        for (auto& line : lines) {
            MedicalRecord r = parseLine(line);
            if (r.id == record.id) {
                newLines.push_back(toLine(record));
                found = true;
            } else {
                newLines.push_back(line);
            }
        }
        if (found) fm.writeAllLines(filename, header, newLines);
        return found;
    }
};

#endif
