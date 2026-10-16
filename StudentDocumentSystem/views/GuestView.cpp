#include "GuestView.h"
#include "ViewHelper.h"
#include "../utils/InputHelper.h"
#include <iostream>

using namespace std;
GuestView::GuestView(QueueService& queue) : queue(queue) {}
void GuestView::showMenu() {
    while (cin) {
        try {
            cout << "\nLAY SO NHAN GIAY\n";
            string studentId = InputHelper::Text("MSSV (0 de quay lai): ");
            if (studentId == "0") return;
            Department department = ViewHelper::ChooseDepartment();
            MyList<RequestInfo> eligible = queue.GetEligibleRequests(studentId, department);
            ViewHelper::Requests(eligible);
            if (eligible.Empty()) { cout << "Chua co don den hen hoac don da co ve dang hoat dong.\n"; continue; }
            string id = InputHelper::Text("Ma don can nhan (0 de huy): ");
            if (id == "0") continue;
            Ticket ticket = queue.GenerateTicket(studentId, id, department);
            cout << "\nSO CUA BAN: " << ticket.GetDisplayNumber() << "\nMa ve: " << ticket.GetTicketId()
                << "\nNgay: " << ticket.GetTicketDate() << ". Ve chi co hieu luc trong ngay.\n";
        } catch (const exception& error) { ViewHelper::Error(error); }
    }
}
