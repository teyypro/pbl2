#ifndef SERVICE_REPOSITORY_H
#define SERVICE_REPOSITORY_H

#include "../database/FileManager.h"
#include "../models/Service.h"
#include <vector>

class ServiceRepository {
private:
    FileManager fm;
    string filename = "services.txt";

    Service parseLine(const string& line) {
        Service s;
        vector<string> f = FileManager::split(line);
        if (f.size() >= 9) {
            s.id = stoi(f[0]);
            s.code = f[1];
            s.name = f[2];
            s.description = f[3];
            s.price = stod(f[4]);
            s.unit = f[5];
            s.category = f[6];
            s.department_id = stoi(f[7]);
            s.status = f[8];
        }
        return s;
    }

    string toLine(const Service& s) {
        return to_string(s.id) + "|" + s.code + "|" + s.name + "|" + s.description + "|"
             + to_string(s.price) + "|" + s.unit + "|" + s.category + "|"
             + to_string(s.department_id) + "|" + s.status;
    }

    void loadDeptNames(vector<Service>& services) {
        FileManager fmDept;
        vector<string> deptLines = fmDept.readAllLines("departments.txt");
        for (auto& svc : services) {
            for (auto& dl : deptLines) {
                vector<string> df = FileManager::split(dl);
                if (df.size() >= 3 && stoi(df[0]) == svc.department_id) {
                    svc.department_name = df[2];
                    break;
                }
            }
        }
    }

public:
    ServiceRepository() {}

    vector<Service> findAll() {
        vector<Service> services;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            services.push_back(parseLine(line));
        }
        loadDeptNames(services);
        return services;
    }

    Service findById(int id) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Service s = parseLine(line);
            if (s.id == id) {
                vector<Service> v = {s};
                loadDeptNames(v);
                return v[0];
            }
        }
        return Service();
    }

    vector<Service> findByDepartment(int departmentId) {
        vector<Service> services;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Service s = parseLine(line);
            if (s.department_id == departmentId && s.status == "active") {
                services.push_back(s);
            }
        }
        loadDeptNames(services);
        return services;
    }

    int create(const Service& svc) {
        Service s = svc;
        s.id = fm.getNextId(filename);
        fm.appendLine(filename, toLine(s));
        return s.id;
    }

    bool update(const Service& svc) {
        string header = fm.readHeader(filename);
        vector<string> lines = fm.readAllLines(filename);
        vector<string> newLines;
        bool found = false;
        for (auto& line : lines) {
            Service s = parseLine(line);
            if (s.id == svc.id) {
                newLines.push_back(toLine(svc));
                found = true;
            } else {
                newLines.push_back(line);
            }
        }
        if (found) fm.writeAllLines(filename, header, newLines);
        return found;
    }

    bool remove(int id) {
        Service s = findById(id);
        if (s.id == 0) return false;
        s.status = "inactive";
        return update(s);
    }
};

#endif
