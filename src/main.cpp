#include <iostream>
#include <string>

using namespace std;

// =============================================
// HÀM HIỂN THỊ MENU CHÍNH
// =============================================
// Đây là điểm bắt đầu của chương trình.
// Người dùng sẽ thấy menu này đầu tiên khi chạy app.
// Sau khi đăng nhập, menu sẽ thay đổi tùy theo role (admin/doctor/patient/receptionist).

void showMainMenu() {
    cout << "========================================" << endl;
    cout << "   HE THONG QUAN LY PHONG KHAM" << endl;
    cout << "========================================" << endl;
    cout << "1. Dang nhap" << endl;
    cout << "2. Dang ky (Benh nhan)" << endl;
    cout << "0. Thoat" << endl;
    cout << "========================================" << endl;
    cout << "Chon: ";
}

int main() {
    int choice;

    while (true) {
        showMainMenu();
        cin >> choice;

        switch (choice) {
            case 1:
                cout << "[TODO] Chuc nang dang nhap - se lam o Giai doan 1" << endl;
                break;
            case 2:
                cout << "[TODO] Chuc nang dang ky - se lam o Giai doan 1" << endl;
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
