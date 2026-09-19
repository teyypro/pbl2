#ifndef DOCTOR_REPOSITORY_H
#define DOCTOR_REPOSITORY_H

#include "FileManager.h"
#include "Doctor.h"
#include "User.h"
#include <vector>

class DoctorRepository {
private:
    FileManager fm;
    string filename = "doctors.txt";

    Doctor parseLine(const string& line) {
        Doctor d;
        vector<string> f = FileManager::split(line);
        if (f.size() >= 9) {
            d.id = stoi(f[0]);
            d.user_id = stoi(f[1]);
            d.department_id = stoi(f[2]);
            d.specialty = f[3];
            d.dob = f[4];
            d.salary = stod(f[5]);
            d.years_experience = stoi(f[6]);
            d.qualification = f[7];
            d.status = f[8];
        }
        return d;
    }

    string toLine(const Doctor& d) {
        return to_string(d.id) + "|" + to_string(d.user_id) + "|" + to_string(d.department_id) + "|"
             + d.specialty + "|" + d.dob + "|" + to_string(d.salary) + "|"
             + to_string(d.years_experience) + "|" + d.qualification + "|" + d.status;
    }

    // Load ten bac si tu users.txt
    void loadNames(vector<Doctor>& doctors) {
        FileManager fmUser;
        vector<string> userLines = fmUser.readAllLines("users.txt");
        vector<string> deptLines = fmUser.readAllLines("departments.txt");

        for (auto& doc : doctors) {
            // Tim ten tu users
            for (auto& ul : userLines) {
                vector<string> uf = FileManager::split(ul);
                if (uf.size() >= 4 && stoi(uf[0]) == doc.user_id) {
                    doc.full_name = uf[3];
                    break;
                }
            }
            // Tim ten khoa tu departments
            for (auto& dl : deptLines) {
                vector<string> df = FileManager::split(dl);
                if (df.size() >= 3 && stoi(df[0]) == doc.department_id) {
                    doc.department_name = df[2];
                    break;
                }
            }
        }
    }

public:
    DoctorRepository() {}

    vector<Doctor> findAll() {
        vector<Doctor> doctors;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            doctors.push_back(parseLine(line));
        }
        loadNames(doctors);
        return doctors;
    }

    Doctor findById(int id) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Doctor d = parseLine(line);
            if (d.id == id) {
                vector<Doctor> v = {d};
                loadNames(v);
                return v[0];
            }
        }
        return Doctor();
    }

    Doctor findByUserId(int userId) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Doctor d = parseLine(line);
            if (d.user_id == userId) {
                vector<Doctor> v = {d};
                loadNames(v);
                return v[0];
            }
        }
        return Doctor();
    }

    vector<Doctor> findByDepartment(int departmentId) {
        vector<Doctor> doctors;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Doctor d = parseLine(line);
            if (d.department_id == departmentId && d.status == "active") {
                doctors.push_back(d);
            }
        }
        loadNames(doctors);
        return doctors;
    }

    int create(const Doctor& doctor) {
        Doctor d = doctor;
        d.id = fm.getNextId(filename);
        fm.appendLine(filename, toLine(d));
        return d.id;
    }

    bool update(const Doctor& doctor) {
        string header = fm.readHeader(filename);
        vector<string> lines = fm.readAllLines(filename);
        vector<string> newLines;
        bool found = false;
        for (auto& line : lines) {
            Doctor d = parseLine(line);
            if (d.id == doctor.id) {
                newLines.push_back(toLine(doctor));
                found = true;
            } else {
                newLines.push_back(line);
            }
        }
        if (found) fm.writeAllLines(filename, header, newLines);
        return found;
    }

    bool remove(int id) {
        Doctor d = findById(id);
        if (d.id == 0) return false;
        d.status = "inactive";
        return update(d);
    }
};

#endif
