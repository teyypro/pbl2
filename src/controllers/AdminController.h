#ifndef ADMIN_CONTROLLER_H
#define ADMIN_CONTROLLER_H

#include "../repositories/DepartmentRepository.h"
#include "../repositories/DoctorRepository.h"
#include "../repositories/PatientRepository.h"
#include "../repositories/ServiceRepository.h"
#include "../repositories/MedicineRepository.h"
#include "../repositories/UserRepository.h"
#include "../utils/Helpers.h"

class AdminController {
private:
    DepartmentRepository deptRepo;
    DoctorRepository doctorRepo;
    PatientRepository patientRepo;
    ServiceRepository svcRepo;
    MedicineRepository medRepo;
    UserRepository userRepo;

    // ============ KHOA ============
    void listDepartments() {
        Helpers::printTitle("DANH SACH KHOA");
        auto depts = deptRepo.findAll();
        cout << left;
        printf("%-4s %-8s %-25s %-15s %-10s\n", "ID", "Ma", "Ten khoa", "Vi tri", "Trang thai");
        Helpers::printLine(65);
        for (auto& d : depts) {
            printf("%-4d %-8s %-25s %-15s %-10s\n", d.id, d.code.c_str(), d.name.c_str(),
                   d.floor_location.c_str(), d.status.c_str());
        }
    }

    void addDepartment() {
        Helpers::printTitle("THEM KHOA MOI");
        Department d;
        d.code = Helpers::getInputString("Ma khoa: ");
        d.name = Helpers::getLine("Ten khoa: ");
        d.description = Helpers::getLine("Mo ta: ");
        d.floor_location = Helpers::getLine("Vi tri (VD: Tang 2): ");
        d.head_doctor_id = 0;
        d.status = "active";

        int id = deptRepo.create(d);
        if (id > 0) cout << "Them khoa thanh cong! ID: " << id << endl;
        else cout << "Them khoa that bai!" << endl;
    }

    void manageDepartments() {
        int choice;
        do {
            Helpers::printTitle("QUAN LY KHOA");
            cout << "1. Xem danh sach khoa" << endl;
            cout << "2. Them khoa" << endl;
            cout << "0. Quay lai" << endl;
            choice = Helpers::getInputInt("Chon: ");
            switch (choice) {
                case 1: listDepartments(); break;
                case 2: addDepartment(); break;
                case 0: break;
                default: cout << "Lua chon khong hop le!" << endl;
            }
            if (choice != 0) Helpers::pauseScreen();
        } while (choice != 0);
    }

    // ============ BAC SI ============
    void listDoctors() {
        Helpers::printTitle("DANH SACH BAC SI");
        auto doctors = doctorRepo.findAll();
        printf("%-4s %-25s %-20s %-15s %-10s\n", "ID", "Ho ten", "Chuyen khoa", "Khoa", "Trang thai");
        Helpers::printLine(75);
        for (auto& d : doctors) {
            printf("%-4d %-25s %-20s %-15s %-10s\n", d.id, d.full_name.c_str(),
                   d.specialty.c_str(), d.department_name.c_str(), d.status.c_str());
        }
    }

    void addDoctor() {
        Helpers::printTitle("THEM BAC SI MOI");
        // Tao user truoc
        User u;
        u.username = Helpers::getInputString("Ten dang nhap: ");
        u.password_hash = Helpers::getLine("Mat khau: ");
        u.full_name = Helpers::getLine("Ho ten: ");
        u.email = Helpers::getLine("Email: ");
        u.phone = Helpers::getLine("So dien thoai: ");
        u.role = "doctor";
        u.status = "active";

        int userId = userRepo.create(u);
        if (userId <= 0) { cout << "Tao tai khoan that bai!" << endl; return; }

        // Tao doctor
        listDepartments();
        Doctor d;
        d.user_id = userId;
        d.department_id = Helpers::getInputInt("ID khoa: ");
        d.specialty = Helpers::getInputString("Chuyen khoa: ");
        d.dob = Helpers::getLine("Ngay sinh (YYYY-MM-DD): ");
        d.salary = Helpers::getInputDouble("Luong: ");
        d.years_experience = Helpers::getInputInt("So nam kinh nghiem: ");
        d.qualification = Helpers::getInputString("Trinh do: ");
        d.status = "active";

        int id = doctorRepo.create(d);
        if (id > 0) cout << "Them bac si thanh cong! ID: " << id << endl;
        else cout << "Them bac si that bai!" << endl;
    }

    void manageDoctors() {
        int choice;
        do {
            Helpers::printTitle("QUAN LY BAC SI");
            cout << "1. Xem danh sach bac si" << endl;
            cout << "2. Them bac si" << endl;
            cout << "0. Quay lai" << endl;
            choice = Helpers::getInputInt("Chon: ");
            switch (choice) {
                case 1: listDoctors(); break;
                case 2: addDoctor(); break;
                case 0: break;
                default: cout << "Lua chon khong hop le!" << endl;
            }
            if (choice != 0) Helpers::pauseScreen();
        } while (choice != 0);
    }

    // ============ BENH NHAN ============
    void listPatients() {
        Helpers::printTitle("DANH SACH BENH NHAN");
        auto patients = patientRepo.findAll();
        printf("%-4s %-10s %-25s %-12s %-8s %-15s\n", "ID", "Ma BN", "Ho ten", "Ngay sinh", "GT", "SDT");
        Helpers::printLine(80);
        for (auto& p : patients) {
            printf("%-4d %-10s %-25s %-12s %-8s %-15s\n", p.id, p.patient_code.c_str(),
                   p.full_name.c_str(), p.dob.c_str(), p.gender.c_str(), p.phone.c_str());
        }
    }

    void managePatients() {
        int choice;
        do {
            Helpers::printTitle("QUAN LY BENH NHAN");
            cout << "1. Xem danh sach benh nhan" << endl;
            cout << "0. Quay lai" << endl;
            choice = Helpers::getInputInt("Chon: ");
            switch (choice) {
                case 1: listPatients(); break;
                case 0: break;
                default: cout << "Lua chon khong hop le!" << endl;
            }
            if (choice != 0) Helpers::pauseScreen();
        } while (choice != 0);
    }

    // ============ DICH VU ============
    void listServices() {
        Helpers::printTitle("DANH SACH DICH VU");
        auto services = svcRepo.findAll();
        printf("%-4s %-8s %-25s %-15s %-15s\n", "ID", "Ma", "Ten dich vu", "Gia", "Khoa");
        Helpers::printLine(70);
        for (auto& s : services) {
            printf("%-4d %-8s %-25s %-15s %-15s\n", s.id, s.code.c_str(), s.name.c_str(),
                   Helpers::formatMoney(s.price).c_str(), s.department_name.c_str());
        }
    }

    void addService() {
        Helpers::printTitle("THEM DICH VU MOI");
        listDepartments();
        Service s;
        s.code = Helpers::getInputString("Ma dich vu: ");
        s.name = Helpers::getLine("Ten dich vu: ");
        s.description = Helpers::getLine("Mo ta: ");
        s.price = Helpers::getInputDouble("Gia (VND): ");
        s.unit = Helpers::getInputString("Don vi (VD: Lan): ");
        s.category = Helpers::getLine("Loai (VD: Kham benh): ");
        s.department_id = Helpers::getInputInt("ID khoa: ");
        s.status = "active";

        int id = svcRepo.create(s);
        if (id > 0) cout << "Them dich vu thanh cong! ID: " << id << endl;
        else cout << "Them dich vu that bai!" << endl;
    }

    void manageServices() {
        int choice;
        do {
            Helpers::printTitle("QUAN LY DICH VU");
            cout << "1. Xem danh sach dich vu" << endl;
            cout << "2. Them dich vu" << endl;
            cout << "0. Quay lai" << endl;
            choice = Helpers::getInputInt("Chon: ");
            switch (choice) {
                case 1: listServices(); break;
                case 2: addService(); break;
                case 0: break;
                default: cout << "Lua chon khong hop le!" << endl;
            }
            if (choice != 0) Helpers::pauseScreen();
        } while (choice != 0);
    }

    // ============ THUOC ============
    void listMedicines() {
        Helpers::printTitle("DANH SACH THUOC");
        auto meds = medRepo.findAll();
        printf("%-4s %-8s %-25s %-10s %-10s %-8s\n", "ID", "Ma", "Ten thuoc", "Don vi", "Gia", "Ton kho");
        Helpers::printLine(70);
        for (auto& m : meds) {
            printf("%-4d %-8s %-25s %-10s %-10s %-8d\n", m.id, m.code.c_str(), m.name.c_str(),
                   m.unit.c_str(), Helpers::formatMoney(m.price).c_str(), m.stock_quantity);
        }
    }

    void addMedicine() {
        Helpers::printTitle("THEM THUOC MOI");
        Medicine m;
        m.code = Helpers::getInputString("Ma thuoc: ");
        m.name = Helpers::getLine("Ten thuoc: ");
        m.active_ingredient = Helpers::getLine("Hoat chat: ");
        m.unit = Helpers::getLine("Don vi (VD: Vien): ");
        m.price = Helpers::getInputDouble("Gia (VND): ");
        m.stock_quantity = Helpers::getInputInt("So luong ton kho: ");
        m.min_stock = Helpers::getInputInt("Ton kho toi thieu: ");
        m.manufacturer = Helpers::getInputString("Nha san xuat: ");
        m.requires_prescription = true;
        m.status = "available";

        int id = medRepo.create(m);
        if (id > 0) cout << "Them thuoc thanh cong! ID: " << id << endl;
        else cout << "Them thuoc that bai!" << endl;
    }

    void manageMedicines() {
        int choice;
        do {
            Helpers::printTitle("QUAN LY THUOC");
            cout << "1. Xem danh sach thuoc" << endl;
            cout << "2. Them thuoc" << endl;
            cout << "0. Quay lai" << endl;
            choice = Helpers::getInputInt("Chon: ");
            switch (choice) {
                case 1: listMedicines(); break;
                case 2: addMedicine(); break;
                case 0: break;
                default: cout << "Lua chon khong hop le!" << endl;
            }
            if (choice != 0) Helpers::pauseScreen();
        } while (choice != 0);
    }

public:
    void showMenu() {
        int choice;
        do {
            Helpers::printTitle("MENU ADMIN");
            cout << "1. Quan ly khoa" << endl;
            cout << "2. Quan ly bac si" << endl;
            cout << "3. Quan ly benh nhan" << endl;
            cout << "4. Quan ly dich vu" << endl;
            cout << "5. Quan ly thuoc" << endl;
            cout << "0. Dang xuat" << endl;
            choice = Helpers::getInputInt("Chon: ");

            switch (choice) {
                case 1: manageDepartments(); break;
                case 2: manageDoctors(); break;
                case 3: managePatients(); break;
                case 4: manageServices(); break;
                case 5: manageMedicines(); break;
                case 0: cout << "Dang xuat..." << endl; break;
                default: cout << "Lua chon khong hop le!" << endl;
            }
        } while (choice != 0);
    }
};

#endif
