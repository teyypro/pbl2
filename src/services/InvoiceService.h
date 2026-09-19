#ifndef INVOICE_SERVICE_H
#define INVOICE_SERVICE_H

#include "InvoiceRepository.h"
#include "MedicalRecordRepository.h"
#include "PrescriptionRepository.h"
#include "ServiceRepository.h"
#include "MedicineRepository.h"
#include "Helpers.h"
#include <ctime>

class InvoiceService {
private:
    InvoiceRepository invRepo;
    ServiceRepository svcRepo;
    PrescriptionRepository presRepo;

public:
    vector<Invoice> getAll() { return invRepo.findAll(); }
    vector<Invoice> getByPatient(int patientId) { return invRepo.findByPatient(patientId); }
    Invoice getById(int id) { return invRepo.findById(id); }

    // Tao hoa don tu dich vu kham
    int createInvoice(int patientId, int medicalRecordId, int prescriptionId,
                      int serviceId, int createdBy) {
        Service svc = svcRepo.findById(serviceId);

        Invoice inv;
        inv.invoice_code = Helpers::generateCode("INV");
        inv.patient_id = patientId;
        inv.medical_record_id = medicalRecordId;
        inv.prescription_id = prescriptionId;

        time_t now = time(nullptr);
        char buf[20];
        strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
        inv.invoice_date = string(buf);

        inv.created_by = createdBy;
        inv.payment_method = "cash";
        inv.payment_status = "pending";
        inv.note = "";

        // Tinh tien dich vu
        double total = 0;
        int invId = invRepo.create(inv);
        if (invId <= 0) return -1;

        // Them dich vu vao hoa don
        if (svc.id > 0) {
            InvoiceItem item;
            item.invoice_id = invId;
            item.item_type = "service";
            item.item_id = svc.id;
            item.item_name = svc.name;
            item.quantity = 1;
            item.unit_price = svc.price;
            item.discount = 0;
            item.amount = svc.price;
            invRepo.addItem(item);
            total += svc.price;
        }

        // Them thuoc tu don thuoc
        if (prescriptionId > 0) {
            Prescription pres = presRepo.findById(prescriptionId);
            for (auto& detail : pres.details) {
                InvoiceItem item;
                item.invoice_id = invId;
                item.item_type = "medicine";
                item.item_id = detail.medicine_id;
                item.item_name = detail.medicine_name;
                item.quantity = detail.quantity;
                item.unit_price = detail.medicine_price;
                item.discount = 0;
                item.amount = detail.medicine_price * detail.quantity;
                invRepo.addItem(item);
                total += item.amount;
            }
        }

        // Cap nhat tong tien
        inv.id = invId;
        inv.total_amount = total;
        inv.patient_pay = total;
        // Update lai file voi tong tien
        string header = FileManager().readHeader("invoices.txt");
        vector<string> lines = FileManager().readAllLines("invoices.txt");
        vector<string> newLines;
        for (auto& line : lines) {
            vector<string> f = FileManager::split(line);
            if (f.size() >= 1 && stoi(f[0]) == invId) {
                f[5] = to_string(total);
                f[8] = to_string(total);
                newLines.push_back(FileManager::join(f));
            } else {
                newLines.push_back(line);
            }
        }
        FileManager().writeAllLines("invoices.txt", header, newLines);

        cout << "Tao hoa don thanh cong! Ma: " << inv.invoice_code << " - Tong: " << Helpers::formatMoney(total) << endl;
        return invId;
    }

    bool payInvoice(int id, double amount, const string& method) {
        Invoice inv = invRepo.findById(id);
        if (inv.id == 0) return false;

        // Update status
        string header = FileManager().readHeader("invoices.txt");
        vector<string> lines = FileManager().readAllLines("invoices.txt");
        vector<string> newLines;
        for (auto& line : lines) {
            vector<string> f = FileManager::split(line);
            if (f.size() >= 1 && stoi(f[0]) == id) {
                f[9] = to_string(amount);
                f[10] = method;
                f[11] = "paid";
                newLines.push_back(FileManager::join(f));
            } else {
                newLines.push_back(line);
            }
        }
        FileManager().writeAllLines("invoices.txt", header, newLines);
        return true;
    }
};

#endif
