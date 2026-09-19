#include <iostream>
#include <string>

#include "controllers/AuthController.h"
#include "controllers/AdminController.h"
#include "controllers/DoctorController.h"
#include "controllers/ReceptionistController.h"
#include "controllers/PatientController.h"
#include "utils/Helpers.h"

using namespace std;

// =============================================
// HÀM HIỂN THỊ MENU CHÍNH
// =============================================

void showMainMenu() {
    Helpers::printTitle("HE THONG QUAN LY PHONG KHAM");
    cout << "1. Dang nhap" << endl;
    cout << "2. Dang ky (Benh nhan)" << endl;
    cout << "0. Thoat" << endl;
    Helpers::printLine();
}

int main() {
    AuthController authCtrl;
    int choice;

    while (true) {
        showMainMenu();
        choice = Helpers::getInputInt("Chon: ");

        switch (choice) {
            case 1: {
                User user = authCtrl.showLogin();
                if (user.id > 0) {
                    // Dieu huong theo role
                    if (user.role == "admin") {
                        AdminController adminCtrl;
                        adminCtrl.showMenu();
                    } else if (user.role == "doctor") {
                        DoctorController doctorCtrl(user);
                        doctorCtrl.showMenu();
                    } else if (user.role == "receptionist") {
                        ReceptionistController recepCtrl(user);
                        recepCtrl.showMenu();
                    } else if (user.role == "patient") {
                        PatientController patientCtrl(user);
                        patientCtrl.showMenu();
                    }
                }
                break;
            }
            case 2:
                authCtrl.showRegister();
                break;
            case 0:
                cout << "Tam biet!" << endl;
                return 0;
            default:
                cout << "Lua chon khong hop le! Vui long chon lai." << endl;
        }

        cout << endl;
    }

    return 0;
}
