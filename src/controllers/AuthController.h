#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include "AuthService.h"
#include "Helpers.h"

class AuthController {
private:
    AuthService authService;

public:
    // Dang nhap -> tra ve User
    User showLogin() {
        Helpers::printTitle("DANG NHAP");
        string username = Helpers::getInputString("Ten dang nhap: ");
        string password = Helpers::getInputString("Mat khau: ");

        User user = authService.login(username, password);
        if (user.id == 0) {
            cout << "Sai ten dang nhap hoac mat khau!" << endl;
        } else {
            cout << "Dang nhap thanh cong! Xin chao " << user.full_name << " (" << user.role << ")" << endl;
        }
        return user;
    }

    // Dang ky benh nhan
    void showRegister() {
        Helpers::printTitle("DANG KY TAI KHOAN BENH NHAN");
        string username = Helpers::getInputString("Ten dang nhap: ");
        string password = Helpers::getInputString("Mat khau: ");
        string fullName = Helpers::getLine("Ho ten: ");
        string dob = Helpers::getLine("Ngay sinh (YYYY-MM-DD): ");
        string gender = Helpers::getLine("Gioi tinh (male/female/other): ");
        string phone = Helpers::getLine("So dien thoai: ");
        string email = Helpers::getLine("Email: ");
        string address = Helpers::getLine("Dia chi: ");

        authService.registerPatient(username, password, fullName, dob, gender, phone, email, address);
    }
};

#endif
