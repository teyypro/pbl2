#ifndef DEPARTMENT_REPOSITORY_H
#define DEPARTMENT_REPOSITORY_H

#include "../database/FileManager.h"
#include "../models/Department.h"
#include <vector>

class DepartmentRepository {
private:
    FileManager fm;
    string filename = "departments.txt";

    Department parseLine(const string& line) {
        Department d;
        vector<string> f = FileManager::split(line);
        if (f.size() >= 7) {
            d.id = stoi(f[0]);
            d.code = f[1];
            d.name = f[2];
            d.description = f[3];
            d.floor_location = f[4];
            d.head_doctor_id = stoi(f[5]);
            d.status = f[6];
        }
        return d;
    }

    string toLine(const Department& d) {
        return to_string(d.id) + "|" + d.code + "|" + d.name + "|" + d.description + "|"
             + d.floor_location + "|" + to_string(d.head_doctor_id) + "|" + d.status;
    }

public:
    DepartmentRepository() {}

    vector<Department> findAll() {
        vector<Department> depts;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            depts.push_back(parseLine(line));
        }
        return depts;
    }

    Department findById(int id) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Department d = parseLine(line);
            if (d.id == id) return d;
        }
        return Department();
    }

    int create(const Department& dept) {
        Department d = dept;
        d.id = fm.getNextId(filename);
        fm.appendLine(filename, toLine(d));
        return d.id;
    }

    bool update(const Department& dept) {
        string header = fm.readHeader(filename);
        vector<string> lines = fm.readAllLines(filename);
        vector<string> newLines;
        bool found = false;
        for (auto& line : lines) {
            Department d = parseLine(line);
            if (d.id == dept.id) {
                newLines.push_back(toLine(dept));
                found = true;
            } else {
                newLines.push_back(line);
            }
        }
        if (found) fm.writeAllLines(filename, header, newLines);
        return found;
    }

    bool remove(int id) {
        Department d = findById(id);
        if (d.id == 0) return false;
        d.status = "inactive";
        return update(d);
    }
};

#endif
