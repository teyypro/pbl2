#ifndef INVOICE_REPOSITORY_H
#define INVOICE_REPOSITORY_H

#include "FileManager.h"
#include "Invoice.h"
#include <vector>

class InvoiceRepository {
private:
    FileManager fm;
    string filename = "invoices.txt";
    string itemFile = "invoice_items.txt";

    Invoice parseLine(const string& line) {
        Invoice inv;
        vector<string> f = FileManager::split(line);
        if (f.size() >= 15) {
            inv.id = stoi(f[0]);
            inv.invoice_code = f[1];
            inv.patient_id = stoi(f[2]);
            inv.medical_record_id = f[3].empty() ? 0 : stoi(f[3]);
            inv.prescription_id = f[4].empty() ? 0 : stoi(f[4]);
            inv.total_amount = stod(f[5]);
            inv.bhyt_discount = stod(f[6]);
            inv.other_discount = stod(f[7]);
            inv.patient_pay = stod(f[8]);
            inv.paid_amount = stod(f[9]);
            inv.payment_method = f[10];
            inv.payment_status = f[11];
            inv.invoice_date = f[12];
            inv.created_by = stoi(f[13]);
            inv.note = f[14];
        }
        return inv;
    }

    string toLine(const Invoice& inv) {
        return to_string(inv.id) + "|" + inv.invoice_code + "|" + to_string(inv.patient_id) + "|"
             + to_string(inv.medical_record_id) + "|" + to_string(inv.prescription_id) + "|"
             + to_string(inv.total_amount) + "|" + to_string(inv.bhyt_discount) + "|"
             + to_string(inv.other_discount) + "|" + to_string(inv.patient_pay) + "|"
             + to_string(inv.paid_amount) + "|" + inv.payment_method + "|"
             + inv.payment_status + "|" + inv.invoice_date + "|"
             + to_string(inv.created_by) + "|" + inv.note;
    }

    InvoiceItem parseItemLine(const string& line) {
        InvoiceItem item;
        vector<string> f = FileManager::split(line);
        if (f.size() >= 10) {
            item.id = stoi(f[0]);
            item.invoice_id = stoi(f[1]);
            item.item_type = f[2];
            item.item_id = stoi(f[3]);
            item.item_name = f[4];
            item.quantity = stoi(f[5]);
            item.unit_price = stod(f[6]);
            item.discount = stod(f[7]);
            item.amount = stod(f[8]);
            item.note = f[9];
        }
        return item;
    }

    string itemToLine(const InvoiceItem& item) {
        return to_string(item.id) + "|" + to_string(item.invoice_id) + "|" + item.item_type + "|"
             + to_string(item.item_id) + "|" + item.item_name + "|"
             + to_string(item.quantity) + "|" + to_string(item.unit_price) + "|"
             + to_string(item.discount) + "|" + to_string(item.amount) + "|" + item.note;
    }

    void loadItems(Invoice& inv) {
        inv.items.clear();
        vector<string> lines = fm.readAllLines(itemFile);
        for (auto& line : lines) {
            InvoiceItem item = parseItemLine(line);
            if (item.invoice_id == inv.id) {
                inv.items.push_back(item);
            }
        }
    }

    void loadPatientName(Invoice& inv) {
        FileManager fmP;
        vector<string> lines = fmP.readAllLines("patients.txt");
        for (auto& line : lines) {
            vector<string> f = FileManager::split(line);
            if (f.size() >= 4 && stoi(f[0]) == inv.patient_id) {
                inv.patient_name = f[3];
                return;
            }
        }
    }

public:
    InvoiceRepository() {}

    vector<Invoice> findAll() {
        vector<Invoice> invoices;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Invoice inv = parseLine(line);
            loadItems(inv);
            loadPatientName(inv);
            invoices.push_back(inv);
        }
        return invoices;
    }

    Invoice findById(int id) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Invoice inv = parseLine(line);
            if (inv.id == id) {
                loadItems(inv);
                loadPatientName(inv);
                return inv;
            }
        }
        return Invoice();
    }

    vector<Invoice> findByPatient(int patientId) {
        vector<Invoice> invoices;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            Invoice inv = parseLine(line);
            if (inv.patient_id == patientId) {
                loadItems(inv);
                loadPatientName(inv);
                invoices.push_back(inv);
            }
        }
        return invoices;
    }

    int create(const Invoice& invoice) {
        Invoice inv = invoice;
        inv.id = fm.getNextId(filename);
        fm.appendLine(filename, toLine(inv));
        return inv.id;
    }

    int addItem(const InvoiceItem& item) {
        InvoiceItem it = item;
        it.id = fm.getNextId(itemFile);
        fm.appendLine(itemFile, itemToLine(it));
        return it.id;
    }

    bool updateStatus(int id, const string& status) {
        string header = fm.readHeader(filename);
        vector<string> lines = fm.readAllLines(filename);
        vector<string> newLines;
        bool found = false;
        for (auto& line : lines) {
            Invoice inv = parseLine(line);
            if (inv.id == id) {
                inv.payment_status = status;
                newLines.push_back(toLine(inv));
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
