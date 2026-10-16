#include "StudentView.h"
#include "ViewHelper.h"
#include "../utils/InputHelper.h"
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace std;
StudentView::StudentView(const User& user, RequestService& requests) : user(user), requests(requests) {}
void StudentView::GoTo(ScreenId target) { history.Push(screen); screen = target; }
void StudentView::Back() {
    if (!history.Empty()) { screen = history.Top(); history.Pop(); }
    else screen = ScreenId::MENU;
}
void StudentView::Create() {
    MyList<DocumentType> documents = requests.GetDocuments();
    string id = InputHelper::Text("Ma giay to (0 de quay lai): ");
    if (id == "0") { Back(); return; }
    int index = -1;
    for (int i = 0; i < documents.Size(); ++i) if (documents.GetByIndex(i).GetDocumentId() == id) index = i;
    if (index < 0) throw runtime_error("Ma giay to khong ton tai.");
    int quantity = InputHelper::Integer("So luong: ", 1, (numeric_limits<int>::max)());
    long long fee = documents.GetByIndex(index).GetFee();
    if (fee > (numeric_limits<long long>::max)() / quantity) throw runtime_error("Tong phi qua lon.");
    string note = InputHelper::Text("Ghi chu (co the bo trong): ", false);
    cout << "Tong phi: " << fee * quantity << " VND. Thu tien tai quay khi nhan giay.\n";
    if (InputHelper::Confirm("Lap don?")) {
        Request result = requests.CreateRequest(user, id, quantity, note);
        cout << "Da tao " << result.GetRequestId() << ", phi " << result.GetTotalFee() << " VND.\n";
    }
    Back();
}
void StudentView::showMenu() {
    while (cin) {
        try {
            if (screen == ScreenId::MENU) {
                cout << "\nSINH VIEN " << user.GetStudentId() << "\n1. Danh muc giay to / lap don\n2. Don cua toi\n0. Dang xuat\n";
                int choice = InputHelper::Integer("Chon: ", 0, 2);
                if (choice == 0) return;
                GoTo(choice == 1 ? ScreenId::DOCUMENTS : ScreenId::REQUESTS);
            } else if (screen == ScreenId::DOCUMENTS) {
                ViewHelper::Documents(requests.GetDocuments());
                if (InputHelper::Integer("1. Lap don; 0. Quay lai: ", 0, 1) == 1) GoTo(ScreenId::CREATE);
                else Back();
            } else if (screen == ScreenId::CREATE) Create();
            else if (screen == ScreenId::REQUESTS) {
                ViewHelper::Requests(requests.GetMyRequests(user));
                selectedRequest = InputHelper::Text("Ma don de xem chi tiet (0 de quay lai): ");
                if (selectedRequest == "0") Back();
                else GoTo(ScreenId::DETAILS);
            } else {
                MyList<RequestInfo> list = requests.GetMyRequests(user);
                bool found = false;
                for (int i = 0; i < list.Size(); ++i) {
                    if (list.GetByIndex(i).request.GetRequestId() != selectedRequest) continue;
                    ViewHelper::RequestDetails(list.GetByIndex(i)); found = true; break;
                }
                if (!found) cout << "Khong co don nay trong tai khoan cua ban.\n";
                InputHelper::Text("Nhan Enter de quay lai: ", false);
                Back();
            }
        } catch (const exception& error) { ViewHelper::Error(error); }
    }
}
