#ifndef PATIENT_CONTROLLER_H
#define PATIENT_CONTROLLER_H

#include "../services/AppointmentService.h"
#include "../services/MedicalRecordService.h"
#include "../services/InvoiceService.h"
#include "../repositories/PatientRepository.h"
#include "../models/User.h"
#include "../utils/Helpers.h"

class PatientController {
private:
    AppointmentService aptService;
    MedicalRecordService mrService;
    InvoiceService invService;
    PatientRepository patientRepo;
    User currentUser;
    Patient currentPatient;

    void viewMyAppointments() {
        Helpers::printTitle("LICH HEN CUA TOI");
        auto apts = aptService.getByPatient(currentPatient.id);
        if (apts.empty()) { cout << "Ban chua co lich hen nao." << endl; return; }

        printf("%-4s %-12s %-20s %-20s %-15s %-10s\n", "ID", "Ma", "Bac si", "Dich vu", "Ngay kham", "TT");
        Helpers::printLine(85);
        for (auto& a : apts) {
            printf("%-4d %-12s %-20s %-20s %-15s %-10s\n", a.id, a.appointment_code.c_str(),
                   a.doctor_name.c_str(), a.service_name.c_str(),
                   a.scheduled_at.c_str(), a.status.c_str());
        }
    }

    void viewMyRecords() {
        Helpers::printTitle("HO SO KHAM BENH CUA TOI");
        auto records = mrService.getByPatient(currentPatient.id);
        if (records.empty()) { cout << "Chua co ho so kham benh." << endl; return; }

        printf("%-4s %-12s %-20s %-15s %-30s\n", "ID", "Ma", "Bac si", "Ngay kham", "Chan doan");
        Helpers::printLine(85);
        for (auto& r : records) {
            printf("%-4d %-12s %-20s %-15s %-30s\n", r.id, r.record_code.c_str(),
                   r.doctor_name.c_str(), r.visit_date.c_str(), r.diagnosis.c_str());
        }

        // Xem chi tiet
        int id = Helpers::getInputInt("\nNhap ID phieu kham de xem chi tiet (0 de bo qua): ");
        if (id > 0) {
            MedicalRecord r = mrService.getById(id);
            if (r.id > 0 && r.patient_id == currentPatient.id) {
                cout << "\n--- CHI TIET PHIEU KHAM ---" << endl;
                cout << "Ma: " << r.record_code << endl;
                cout << "Ngay kham: " << r.visit_date << endl;
                cout << "Trieu chung: " << r.symptoms << endl;
                cout << "Chan doan: " << r.diagnosis << endl;
                cout << "Loi khuyen: " << r.advice << endl;

                // Hien thi don thuoc
                Prescription pres = mrService.getPrescription(r.id);
                if (pres.id > 0) {
                    cout << "\n--- DON THUOC ---" << endl;
                    printf("%-25s %-15s %-8s %-8s %-20s\n", "Thuoc", "Lieu dung", "SL", "Ngay", "Tan suat");
                    Helpers::printLine(80);
                    for (auto& d : pres.details) {
                        printf("%-25s %-15s %-8d %-8d %-20s\n", d.medicine_name.c_str(),
                               d.dosage.c_str(), d.quantity, d.days, d.frequency.c_str());
                    }
                }
            }
        }
    }

    void viewMyInvoices() {
        Helpers::printTitle("HOA DON CUA TOI");
        auto invoices = invService.getByPatient(currentPatient.id);
        if (invoices.empty()) { cout << "Chua co hoa don." << endl; return; }

        printf("%-4s %-12s %-15s %-15s %-15s %-10s\n", "ID", "Ma HD", "Ngay", "Tong tien", "Da tra", "TT");
        Helpers::printLine(75);
        for (auto& inv : invoices) {
            printf("%-4d %-12s %-15s %-15s %-15s %-10s\n", inv.id, inv.invoice_code.c_str(),
                   inv.invoice_date.c_str(), Helpers::formatMoney(inv.total_amount).c_str(),
                   Helpers::formatMoney(inv.paid_amount).c_str(), inv.payment_status.c_str());
        }
    }

    void viewMyProfile() {
        Helpers::printTitle("THONG TIN CA NHAN");
        cout << "Ma benh nhan: " << currentPatient.patient_code << endl;
        cout << "Ho ten: " << currentPatient.full_name << endl;
        cout << "Ngay sinh: " << currentPatient.dob << endl;
        cout << "Gioi tinh: " << currentPatient.gender << endl;
        cout << "SDT: " << currentPatient.phone << endl;
        cout << "Email: " << currentPatient.email << endl;
        cout << "Dia chi: " << currentPatient.address << endl;
        cout << "BHYT: " << currentPatient.bhyt_code << endl;
        cout << "Nhom mau: " << currentPatient.blood_type << endl;
    }

public:
    PatientController(const User& user) : currentUser(user) {
        currentPatient = patientRepo.findByUserId(user.id);
    }

    void showMenu() {
        int choice;
        do {
            Helpers::printTitle("MENU BENH NHAN");
            cout << "Xin chao " << currentPatient.full_name << endl;
            Helpers::printLine();
            cout << "1. Xem lich hen" << endl;
            cout << "2. Xem ho so kham benh" << endl;
            cout << "3. Xem hoa don" << endl;
            cout << "4. Thong tin ca nhan" << endl;
            cout << "0. Dang xuat" << endl;
            choice = Helpers::getInputInt("Chon: ");

            switch (choice) {
                case 1: viewMyAppointments(); Helpers::pauseScreen(); break;
                case 2: viewMyRecords(); Helpers::pauseScreen(); break;
                case 3: viewMyInvoices(); Helpers::pauseScreen(); break;
                case 4: viewMyProfile(); Helpers::pauseScreen(); break;
                case 0: cout << "Dang xuat..." << endl; break;
                default: cout << "Lua chon khong hop le!" << endl;
            }
        } while (choice != 0);
    }
};

#endif
