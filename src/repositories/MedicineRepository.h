#ifndef MEDICINE_REPOSITORY_H
#define MEDICINE_REPOSITORY_H

#include "../database/FileManager.h"
#include "../models/Medicine.h"
#include <vector>

class MedicineRepository {
private:
    FileManager fm;
    string filename = "medicines.txt";

    Medicine parseLine(const string& line) {
        Medicine m;
        vector<string> f = FileManager::split(line);
        if (f.size() >= 12) {
            m.id = stoi(f[0]);
            m.code = f[1];
            m.name = f[2];
            m.active_ingredient = f[3];
            m.unit = f[4];
            m.price = stod(f[5]);
            m.stock_quantity = stoi(f[6]);
            m.min_stock = stoi(f[7]);
            m.expiry_date = f[8];
            m.manufacturer = f[9];
            m.requires_prescription = (f[10] == "1");
            m.status = f[11];
        }
        return m;
    }

    string toLine(const Medicine& m) {
        return to_string(m.id) + "|" + m.code + "|" + m.name + "|" + m.active_ingredient + "|"
             + m.unit + "|" + to_string(m.price) + "|" + to_string(m.stock_quantity) + "|"
             + to_string(m.min_stock) + "|" + m.expiry_date + "|" + m.manufacturer + "|"
             + (m.requires_prescription ? "1" : "0") + "|" + m.status;
    }

public:
    MedicineRepository() {}

    vector<Medicine> findAll() {
        vector<Medicine> meds;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            meds.push_back(parseLine(line));
        }
        return meds;
    }

    Medicine findById(int id) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Medicine m = parseLine(line);
            if (m.id == id) return m;
        }
        return Medicine();
    }

    int create(const Medicine& med) {
        Medicine m = med;
        m.id = fm.getNextId(filename);
        fm.appendLine(filename, toLine(m));
        return m.id;
    }

    bool update(const Medicine& med) {
        string header = fm.readHeader(filename);
        vector<string> lines = fm.readAllLines(filename);
        vector<string> newLines;
        bool found = false;
        for (auto& line : lines) {
            Medicine m = parseLine(line);
            if (m.id == med.id) {
                newLines.push_back(toLine(med));
                found = true;
            } else {
                newLines.push_back(line);
            }
        }
        if (found) fm.writeAllLines(filename, header, newLines);
        return found;
    }

    bool remove(int id) {
        Medicine m = findById(id);
        if (m.id == 0) return false;
        m.status = "discontinued";
        return update(m);
    }

    bool updateStock(int id, int quantityUsed) {
        Medicine m = findById(id);
        if (m.id == 0 || m.stock_quantity < quantityUsed) return false;
        m.stock_quantity -= quantityUsed;
        if (m.stock_quantity == 0) m.status = "out_of_stock";
        return update(m);
    }
};

#endif
