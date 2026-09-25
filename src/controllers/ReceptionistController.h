#ifndef RECEPTIONIST_CONTROLLER_H
#define RECEPTIONIST_CONTROLLER_H

#include "../services/AppointmentService.h"
#include "../services/InvoiceService.h"
#include "../repositories/PatientRepository.h"
#include "../repositories/DoctorRepository.h"
#include "../repositories/DepartmentRepository.h"
#include "../repositories/ServiceRepository.h"
#include "../models/User.h"
#include "../utils/Helpers.h"

class ReceptionistController {
private:
    AppointmentService aptService;
    InvoiceService invService;
    PatientRepository patientRepo;
    DoctorRepository doctorRepo;
    DepartmentRepository deptRepo;
    ServiceRepository svcRepo;
    User currentUser;

    void listAppointments() {
        Helpers::printTitle("DANH SACH LICH HEN");
        auto apts = aptService.getAll();
        printf("%-4s %-12s %-20s %-20s %-15s %-10s\n", "ID", "Ma", "Benh nhan", "Bac si", "Ngay kham", "Trang thai");
        Helpers::printLine(85);
        for (auto& a : apts) {
            printf("%-4d %-12s %-20s %-20s %-15s %-10s\n", a.id, a.appointment_code.c_str(),
                   a.patient_name.c_str(), a.doctor_name.c_str(),
                   a.scheduled_at.c_str(), a.status.c_str());
        }
    }

    void createAppointment() {
        Helpers::printTitle("DAT LICH HEN MOI");

        // Chon benh nhan
        auto patients = patientRepo.findAll();
        printf("%-4s %-10s %-25s\n", "ID", "Ma BN", "Ho ten");
        Helpers::printLine(40);
        for (auto& p : patients) {
            printf("%-4d %-10s %-25s\n", p.id, p.patient_code.c_str(), p.full_name.c_str());
        }
        int patientId = Helpers::getInputInt("Chon ID benh nhan: ");

        // Chon khoa
        auto depts = deptRepo.findAll();
        printf("\n%-4s %-25s\n", "ID", "Ten khoa");
        Helpers::printLine(30);
        for (auto& d : depts) {
            printf("%-4d %-25s\n", d.id, d.name.c_str());
        }
        int deptId = Helpers::getInputInt("Chon ID khoa: ");

        // Chon bac si trong khoa
        auto doctors = doctorRepo.findByDepartment(deptId);
        printf("\n%-4s %-25s %-20s\n", "ID", "Ho ten", "Chuyen khoa");
        Helpers::printLine(50);
        for (auto& d : doctors) {
            printf("%-4d %-25s %-20s\n", d.id, d.full_name.c_str(), d.specialty.c_str());
        }
        int doctorId = Helpers::getInputInt("Chon ID bac si: ");

        // Chon dich vu
        auto services = svcRepo.findByDepartment(deptId);
        printf("\n%-4s %-25s %-15s\n", "ID", "Ten dich vu", "Gia");
        Helpers::printLine(45);
        for (auto& s : services) {
            printf("%-4d %-25s %-15s\n", s.id, s.name.c_str(), Helpers::formatMoney(s.price).c_str());
        }
        int serviceId = Helpers::getInputInt("Chon ID dich vu: ");

        string scheduledAt = Helpers::getInputString("Ngay kham (YYYY-MM-DD HH:MM): ");
        string note = Helpers::getLine("Ghi chu: ");

        aptService.createAppointment(patientId, doctorId, deptId, serviceId,
                                      scheduledAt, currentUser.id, note);
    }

    void cancelAppointment() {
        listAppointments();
        int id = Helpers::getInputInt("Nhap ID lich hen can huy: ");
        string reason = Helpers::getInputString("Ly do huy: ");
        if (aptService.cancelAppointment(id, reason)) {
            cout << "Huy lich hen thanh cong!" << endl;
        }
    }

    void listInvoices() {
        Helpers::printTitle("DANH SACH HOA DON");
        auto invoices = invService.getAll();
        printf("%-4s %-12s %-20s %-15s %-15s %-10s\n", "ID", "Ma HD", "Benh nhan", "Tong tien", "Da tra", "TT");
        Helpers::printLine(80);
        for (auto& inv : invoices) {
            printf("%-4d %-12s %-20s %-15s %-15s %-10s\n", inv.id, inv.invoice_code.c_str(),
                   inv.patient_name.c_str(), Helpers::formatMoney(inv.total_amount).c_str(),
                   Helpers::formatMoney(inv.paid_amount).c_str(), inv.payment_status.c_str());
        }
    }

    void payInvoice() {
        listInvoices();
        int id = Helpers::getInputInt("Nhap ID hoa don can thanh toan: ");
        Invoice inv = invService.getById(id);
        if (inv.id == 0) { cout << "Hoa don khong ton tai!" << endl; return; }

        cout << "Tong tien: " << Helpers::formatMoney(inv.patient_pay) << endl;
        string method = Helpers::getInputString("Phuong thuc (cash/card/bank_transfer): ");

        if (invService.payInvoice(id, inv.patient_pay, method)) {
            cout << "Thanh toan thanh cong!" << endl;
        }
    }

public:
    ReceptionistController(const User& user) : currentUser(user) {}

    void showMenu() {
        int choice;
        do {
            Helpers::printTitle("MENU LE TAN");
            cout << "1. Xem danh sach lich hen" << endl;
            cout << "2. Dat lich hen moi" << endl;
            cout << "3. Huy lich hen" << endl;
            cout << "4. Xem danh sach hoa don" << endl;
            cout << "5. Thanh toan hoa don" << endl;
            cout << "0. Dang xuat" << endl;
            choice = Helpers::getInputInt("Chon: ");

            switch (choice) {
                case 1: listAppointments(); Helpers::pauseScreen(); break;
                case 2: createAppointment(); Helpers::pauseScreen(); break;
                case 3: cancelAppointment(); Helpers::pauseScreen(); break;
                case 4: listInvoices(); Helpers::pauseScreen(); break;
                case 5: payInvoice(); Helpers::pauseScreen(); break;
                case 0: cout << "Dang xuat..." << endl; break;
                default: cout << "Lua chon khong hop le!" << endl;
            }
        } while (choice != 0);
    }
};

#endif
