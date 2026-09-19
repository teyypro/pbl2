#ifndef DOCTOR_CONTROLLER_H
#define DOCTOR_CONTROLLER_H

#include "AppointmentService.h"
#include "MedicalRecordService.h"
#include "InvoiceService.h"
#include "DoctorRepository.h"
#include "MedicineRepository.h"
#include "User.h"
#include "Helpers.h"

class DoctorController {
private:
    AppointmentService aptService;
    MedicalRecordService mrService;
    InvoiceService invService;
    DoctorRepository doctorRepo;
    MedicineRepository medRepo;
    User currentUser;
    Doctor currentDoctor;

    void viewAppointments() {
        Helpers::printTitle("LICH HEN CUA TOI");
        auto apts = aptService.getByDoctor(currentDoctor.id);
        if (apts.empty()) { cout << "Khong co lich hen nao." << endl; return; }

        printf("%-4s %-12s %-20s %-20s %-15s %-10s\n", "ID", "Ma", "Benh nhan", "Dich vu", "Ngay kham", "TT");
        Helpers::printLine(85);
        for (auto& a : apts) {
            printf("%-4d %-12s %-20s %-20s %-15s %-10s\n", a.id, a.appointment_code.c_str(),
                   a.patient_name.c_str(), a.service_name.c_str(),
                   a.scheduled_at.c_str(), a.status.c_str());
        }
    }

    void createMedicalRecord() {
        Helpers::printTitle("TAO PHIEU KHAM");

        // Hien thi lich hen pending/confirmed
        viewAppointments();
        int aptId = Helpers::getInputInt("Nhap ID lich hen de kham: ");
        Appointment apt = aptService.getById(aptId);
        if (apt.id == 0) { cout << "Lich hen khong ton tai!" << endl; return; }

        cout << "\n--- Nhap thong tin kham ---" << endl;
        string symptoms = Helpers::getInputString("Trieu chung: ");
        string clinicalFindings = Helpers::getLine("Phat hien lam sang: ");
        string diagnosis = Helpers::getLine("Chan doan: ");
        string advice = Helpers::getLine("Loi khuyen: ");

        int recordId = mrService.createRecord(apt.patient_id, currentDoctor.id, aptId,
                                               symptoms, diagnosis, advice, clinicalFindings);

        if (recordId > 0) {
            // Hoi co ke don thuoc khong
            string choice = Helpers::getInputString("Ke don thuoc? (y/n): ");
            if (choice == "y" || choice == "Y") {
                createPrescription(recordId, apt.patient_id);
            }

            // Hoan thanh lich hen
            aptService.completeAppointment(aptId);
            mrService.finalizeRecord(recordId);

            // Tao hoa don
            Prescription pres = mrService.getPrescription(recordId);
            invService.createInvoice(apt.patient_id, recordId, pres.id, apt.service_id, currentUser.id);
        }
    }

    void createPrescription(int recordId, int patientId) {
        int presId = mrService.createPrescription(recordId, patientId, currentDoctor.id);
        if (presId <= 0) return;

        // Them thuoc vao don
        string addMore = "y";
        while (addMore == "y" || addMore == "Y") {
            // Hien thi danh sach thuoc
            auto meds = medRepo.findAll();
            printf("\n%-4s %-8s %-25s %-10s %-10s %-8s\n", "ID", "Ma", "Ten thuoc", "Don vi", "Gia", "Ton");
            Helpers::printLine(70);
            for (auto& m : meds) {
                printf("%-4d %-8s %-25s %-10s %-10s %-8d\n", m.id, m.code.c_str(), m.name.c_str(),
                       m.unit.c_str(), Helpers::formatMoney(m.price).c_str(), m.stock_quantity);
            }

            int medId = Helpers::getInputInt("Chon ID thuoc: ");
            string dosage = Helpers::getInputString("Lieu dung (VD: 1 vien): ");
            int quantity = Helpers::getInputInt("So luong: ");
            int days = Helpers::getInputInt("So ngay: ");
            string frequency = Helpers::getInputString("Tan suat (VD: 2 lan/ngay): ");

            cout << "Thoi diem uong:" << endl;
            string m = Helpers::getInputString("Sang? (y/n): ");
            string n = Helpers::getLine("Trua? (y/n): ");
            string e = Helpers::getLine("Chieu? (y/n): ");
            string ni = Helpers::getLine("Toi? (y/n): ");

            mrService.addMedicineToPrescription(presId, medId, dosage, quantity, days, frequency,
                                                 m == "y", n == "y", e == "y", ni == "y");

            cout << "Them thuoc thanh cong!" << endl;
            addMore = Helpers::getInputString("Them thuoc khac? (y/n): ");
        }
    }

    void viewMedicalRecords() {
        Helpers::printTitle("PHIEU KHAM CUA TOI");
        auto records = mrService.getByDoctor(currentDoctor.id);
        if (records.empty()) { cout << "Chua co phieu kham nao." << endl; return; }

        printf("%-4s %-12s %-20s %-15s %-30s %-10s\n", "ID", "Ma", "Benh nhan", "Ngay kham", "Chan doan", "TT");
        Helpers::printLine(95);
        for (auto& r : records) {
            printf("%-4d %-12s %-20s %-15s %-30s %-10s\n", r.id, r.record_code.c_str(),
                   r.patient_name.c_str(), r.visit_date.c_str(),
                   r.diagnosis.c_str(), r.status.c_str());
        }
    }

public:
    DoctorController(const User& user) : currentUser(user) {
        currentDoctor = doctorRepo.findByUserId(user.id);
    }

    void showMenu() {
        int choice;
        do {
            Helpers::printTitle("MENU BAC SI");
            cout << "Xin chao " << currentDoctor.full_name << " - " << currentDoctor.department_name << endl;
            Helpers::printLine();
            cout << "1. Xem lich hen" << endl;
            cout << "2. Kham benh (tao phieu kham)" << endl;
            cout << "3. Xem phieu kham da tao" << endl;
            cout << "0. Dang xuat" << endl;
            choice = Helpers::getInputInt("Chon: ");

            switch (choice) {
                case 1: viewAppointments(); Helpers::pauseScreen(); break;
                case 2: createMedicalRecord(); Helpers::pauseScreen(); break;
                case 3: viewMedicalRecords(); Helpers::pauseScreen(); break;
                case 0: cout << "Dang xuat..." << endl; break;
                default: cout << "Lua chon khong hop le!" << endl;
            }
        } while (choice != 0);
    }
};

#endif
