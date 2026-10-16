#include "LoginView.h"
#include "ViewHelper.h"
#include "../utils/InputHelper.h"
#include <iostream>

using namespace std;
LoginView::LoginView(AuthService& auth, unique_ptr<User>& session) : auth(auth), session(session) {}
void LoginView::showMenu() {
    while (!session && cin) {
        try {
            cout << "\nTAI KHOAN\n1. Dang nhap\n2. Dang ky sinh vien\n0. Quay lai\n";
            int choice = InputHelper::Integer("Chon: ", 0, 2);
            if (choice == 0) return;
            string username = InputHelper::Text(choice == 1 ? "Tai khoan: " : "MSSV: ");
            string password = InputHelper::Password("Mat khau: ");
            if (choice == 1) {
                session = auth.Login(username, password);
                cout << "Xin chao " << session->GetFullName() << ".\n";
            } else {
                string confirmation = InputHelper::Password("Nhap lai mat khau: ");
                auth.RegisterStudent(username, password, confirmation);
                cout << "Dang ky thanh cong. Hay dang nhap.\n";
            }
        } catch (const exception& error) { ViewHelper::Error(error); }
    }
}
