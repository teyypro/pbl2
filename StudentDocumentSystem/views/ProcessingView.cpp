#include "ProcessingView.h"
#include "ViewHelper.h"
#include "../utils/InputHelper.h"
#include <iostream>
#include <stdexcept>

using namespace std;
ProcessingView::ProcessingView(const User& user, ProcessingService& processing) : user(user), processing(processing) {}
void ProcessingView::showMenu() {
    while (cin) {
        try {
            cout << "\nXU LY HO SO - " << ToString(user.GetDepartment())
                << "\n1. Danh sach don\n2. Chi tiet don\n3. Duyet\n4. Tu choi\n5. Danh dau da in\n6. Hen nhan\n0. Dang xuat\n";
            int choice = InputHelper::Integer("Chon: ", 0, 6);
            if (choice == 0) return;
            if (choice == 1) { ViewHelper::Requests(processing.GetDepartmentRequests(user)); continue; }
            string id = InputHelper::Text("Ma don: ");
            if (choice == 2) {
                MyList<RequestInfo> list = processing.GetDepartmentRequests(user);
                bool found = false;
                for (int i = 0; i < list.Size(); ++i) {
                    if (list.GetByIndex(i).request.GetRequestId() != id) continue;
                    ViewHelper::RequestDetails(list.GetByIndex(i)); found = true; break;
                }
                if (!found) throw runtime_error("Khong co don nay trong phong cua ban.");
                continue;
            }
            if (choice == 3) processing.ApproveRequest(user, id);
            if (choice == 4) processing.RejectRequest(user, id, InputHelper::Text("Ly do tu choi: "));
            if (choice == 5) processing.MarkPrinted(user, id);
            if (choice == 6) processing.SetReadyForCollection(user, id,
                InputHelper::Text("Hen nhan YYYY-MM-DD HH:MM:SS (Enter: nhan ngay): ", false));
            cout << "Da luu.\n";
        } catch (const exception& error) { ViewHelper::Error(error); }
    }
}
