#ifndef APPOINTMENT_REPOSITORY_H
#define APPOINTMENT_REPOSITORY_H

#include "../database/FileManager.h"
#include "../models/Appointment.h"
#include <vector>

class AppointmentRepository {
private:
    FileManager fm;
    string filename = "appointments.txt";

    Appointment parseLine(const string& line) {
        Appointment a;
        vector<string> f = FileManager::split(line);
        if (f.size() >= 11) {
            a.id = stoi(f[0]);
            a.appointment_code = f[1];
            a.patient_id = stoi(f[2]);
            a.doctor_id = stoi(f[3]);
            a.department_id = stoi(f[4]);
            a.service_id = stoi(f[5]);
            a.scheduled_at = f[6];
            a.status = f[7];
            a.cancel_reason = f[8];
            a.created_by = stoi(f[9]);
            a.note = f[10];
        }
        return a;
    }

    string toLine(const Appointment& a) {
        return to_string(a.id) + "|" + a.appointment_code + "|" + to_string(a.patient_id) + "|"
             + to_string(a.doctor_id) + "|" + to_string(a.department_id) + "|"
             + to_string(a.service_id) + "|" + a.scheduled_at + "|" + a.status + "|"
             + a.cancel_reason + "|" + to_string(a.created_by) + "|" + a.note;
    }

    void loadNames(vector<Appointment>& apts) {
        FileManager fmHelper;
        vector<string> patientLines = fmHelper.readAllLines("patients.txt");
        vector<string> userLines = fmHelper.readAllLines("users.txt");
        vector<string> doctorLines = fmHelper.readAllLines("doctors.txt");
        vector<string> deptLines = fmHelper.readAllLines("departments.txt");
        vector<string> svcLines = fmHelper.readAllLines("services.txt");

        for (auto& apt : apts) {
            // Ten benh nhan
            for (auto& pl : patientLines) {
                vector<string> pf = FileManager::split(pl);
                if (pf.size() >= 4 && stoi(pf[0]) == apt.patient_id) {
                    apt.patient_name = pf[3];
                    break;
                }
            }
            // Ten bac si (tu doctors -> user_id -> users.full_name)
            int userId = 0;
            for (auto& dl : doctorLines) {
                vector<string> df = FileManager::split(dl);
                if (df.size() >= 2 && stoi(df[0]) == apt.doctor_id) {
                    userId = stoi(df[1]);
                    break;
                }
            }
            for (auto& ul : userLines) {
                vector<string> uf = FileManager::split(ul);
                if (uf.size() >= 4 && stoi(uf[0]) == userId) {
                    apt.doctor_name = uf[3];
                    break;
                }
            }
            // Ten khoa
            for (auto& dl : deptLines) {
                vector<string> df = FileManager::split(dl);
                if (df.size() >= 3 && stoi(df[0]) == apt.department_id) {
                    apt.department_name = df[2];
                    break;
                }
            }
            // Ten dich vu
            for (auto& sl : svcLines) {
                vector<string> sf = FileManager::split(sl);
                if (sf.size() >= 3 && stoi(sf[0]) == apt.service_id) {
                    apt.service_name = sf[2];
                    break;
                }
            }
        }
    }

public:
    AppointmentRepository() {}

    vector<Appointment> findAll() {
        vector<Appointment> apts;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            apts.push_back(parseLine(line));
        }
        loadNames(apts);
        return apts;
    }

    Appointment findById(int id) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Appointment a = parseLine(line);
            if (a.id == id) {
                vector<Appointment> v = {a};
                loadNames(v);
                return v[0];
            }
        }
        return Appointment();
    }

    vector<Appointment> findByPatient(int patientId) {
        vector<Appointment> apts;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Appointment a = parseLine(line);
            if (a.patient_id == patientId) apts.push_back(a);
        }
        loadNames(apts);
        return apts;
    }

    vector<Appointment> findByDoctor(int doctorId) {
        vector<Appointment> apts;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Appointment a = parseLine(line);
            if (a.doctor_id == doctorId) apts.push_back(a);
        }
        loadNames(apts);
        return apts;
    }

    int create(const Appointment& apt) {
        Appointment a = apt;
        a.id = fm.getNextId(filename);
        fm.appendLine(filename, toLine(a));
        return a.id;
    }

    bool updateStatus(int id, const string& status, const string& reason = "") {
        string header = fm.readHeader(filename);
        vector<string> lines = fm.readAllLines(filename);
        vector<string> newLines;
        bool found = false;
        for (auto& line : lines) {
            Appointment a = parseLine(line);
            if (a.id == id) {
                a.status = status;
                if (!reason.empty()) a.cancel_reason = reason;
                newLines.push_back(toLine(a));
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
