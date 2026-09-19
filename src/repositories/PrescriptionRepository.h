#ifndef PRESCRIPTION_REPOSITORY_H
#define PRESCRIPTION_REPOSITORY_H

#include "FileManager.h"
#include "Prescription.h"
#include <vector>

class PrescriptionRepository {
private:
    FileManager fm;
    string filename = "prescriptions.txt";
    string detailFile = "prescription_details.txt";

    Prescription parseLine(const string& line) {
        Prescription p;
        vector<string> f = FileManager::split(line);
        if (f.size() >= 8) {
            p.id = stoi(f[0]);
            p.prescription_code = f[1];
            p.medical_record_id = stoi(f[2]);
            p.patient_id = stoi(f[3]);
            p.doctor_id = stoi(f[4]);
            p.prescribed_at = f[5];
            p.note = f[6];
            p.status = f[7];
        }
        return p;
    }

    string toLine(const Prescription& p) {
        return to_string(p.id) + "|" + p.prescription_code + "|" + to_string(p.medical_record_id) + "|"
             + to_string(p.patient_id) + "|" + to_string(p.doctor_id) + "|"
             + p.prescribed_at + "|" + p.note + "|" + p.status;
    }

    PrescriptionDetail parseDetailLine(const string& line) {
        PrescriptionDetail d;
        vector<string> f = FileManager::split(line);
        if (f.size() >= 14) {
            d.id = stoi(f[0]);
            d.prescription_id = stoi(f[1]);
            d.medicine_id = stoi(f[2]);
            d.dosage = f[3];
            d.quantity = stoi(f[4]);
            d.days = stoi(f[5]);
            d.frequency = f[6];
            d.route = f[7];
            d.morning = (f[8] == "1");
            d.noon = (f[9] == "1");
            d.evening = (f[10] == "1");
            d.night = (f[11] == "1");
            d.note = f[12];
            d.status = f[13];
        }
        return d;
    }

    string detailToLine(const PrescriptionDetail& d) {
        return to_string(d.id) + "|" + to_string(d.prescription_id) + "|" + to_string(d.medicine_id) + "|"
             + d.dosage + "|" + to_string(d.quantity) + "|" + to_string(d.days) + "|"
             + d.frequency + "|" + d.route + "|"
             + (d.morning ? "1" : "0") + "|" + (d.noon ? "1" : "0") + "|"
             + (d.evening ? "1" : "0") + "|" + (d.night ? "1" : "0") + "|"
             + d.note + "|" + d.status;
    }

    void loadDetails(Prescription& p) {
        p.details.clear();
        vector<string> lines = fm.readAllLines(detailFile);
        FileManager fmMed;
        vector<string> medLines = fmMed.readAllLines("medicines.txt");

        for (auto& line : lines) {
            PrescriptionDetail d = parseDetailLine(line);
            if (d.prescription_id == p.id) {
                // Load ten thuoc
                for (auto& ml : medLines) {
                    vector<string> mf = FileManager::split(ml);
                    if (mf.size() >= 6 && stoi(mf[0]) == d.medicine_id) {
                        d.medicine_name = mf[2];
                        d.medicine_unit = mf[4];
                        d.medicine_price = stod(mf[5]);
                        break;
                    }
                }
                p.details.push_back(d);
            }
        }
    }

public:
    PrescriptionRepository() {}

    vector<Prescription> findAll() {
        vector<Prescription> prescriptions;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Prescription p = parseLine(line);
            loadDetails(p);
            prescriptions.push_back(p);
        }
        return prescriptions;
    }

    Prescription findById(int id) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Prescription p = parseLine(line);
            if (p.id == id) {
                loadDetails(p);
                return p;
            }
        }
        return Prescription();
    }

    Prescription findByMedicalRecord(int recordId) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Prescription p = parseLine(line);
            if (p.medical_record_id == recordId) {
                loadDetails(p);
                return p;
            }
        }
        return Prescription();
    }

    int create(const Prescription& prescription) {
        Prescription p = prescription;
        p.id = fm.getNextId(filename);
        fm.appendLine(filename, toLine(p));
        return p.id;
    }

    int addDetail(const PrescriptionDetail& detail) {
        PrescriptionDetail d = detail;
        d.id = fm.getNextId(detailFile);
        fm.appendLine(detailFile, detailToLine(d));
        return d.id;
    }

    bool updateStatus(int id, const string& status) {
        string header = fm.readHeader(filename);
        vector<string> lines = fm.readAllLines(filename);
        vector<string> newLines;
        bool found = false;
        for (auto& line : lines) {
            Prescription p = parseLine(line);
            if (p.id == id) {
                p.status = status;
                newLines.push_back(toLine(p));
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
