#include "CounterView.h"
#include "ViewHelper.h"
#include "../utils/InputHelper.h"
#include <iostream>
#include <stdexcept>

using namespace std;
CounterView::CounterView(const User& user, CounterService& counter) : user(user), counter(counter) {}
void CounterView::showMenu() {
    while (cin) {
        try {
            cout << "\nQUAY NHAN GIAY - " << ToString(user.GetDepartment())
                << "\n1. Hang doi va chi tiet\n2. Goi so tiep\n3. Bo luot\n4. Xac nhan thu tien\n5. Hoan tat giao giay\n0. Dang xuat\n";
            int choice = InputHelper::Integer("Chon: ", 0, 5);
            if (choice == 0) return;
            if (choice == 1) {
                MyList<CounterInfo> tickets = counter.GetTickets(user);
                if (tickets.Empty()) cout << "Hang doi rong.\n";
                for (int i = 0; i < tickets.Size(); ++i) ViewHelper::CounterDetails(tickets.GetByIndex(i));
                continue;
            }
            if (choice == 2) { ViewHelper::CounterDetails(counter.CallNextTicket(user)); continue; }
            // Ghim ma ve cu the de khong tac dong nham nguoi vua duoc terminal khac goi.
            string ticketId = InputHelper::Text("Ma ve day du (vi du 20261021-CTSV-001): ");
            if (choice == 3) {
                if (InputHelper::Confirm("Sinh vien vang mat, bo luot?")) counter.SkipCurrentTicket(user, ticketId);
                else continue;
            } else {
                MyList<CounterInfo> tickets = counter.GetTickets(user);
                bool found = false;
                for (int i = 0; i < tickets.Size(); ++i) {
                    const CounterInfo& info = tickets.GetByIndex(i);
                    if (info.ticket.GetTicketId() == ticketId && info.ticket.GetStatus() == TicketStatus::SERVING) {
                        ViewHelper::CounterDetails(info); found = true; break;
                    }
                }
                if (!found) throw runtime_error("Ve khong dang phuc vu.");
                if (!InputHelper::Confirm("Da doi chieu MSSV va the sinh vien/CCCD?")) continue;
                if (choice == 4) {
                    if (!InputHelper::Confirm("Da thu du tien mat theo tong phi hien thi?")) continue;
                    counter.RecordPayment(user, ticketId, true);
                } else {
                    if (!InputHelper::Confirm("Xac nhan giao du giay va hoan tat?")) continue;
                    counter.CompleteCollection(user, ticketId, true);
                }
            }
            cout << "Da luu.\n";
        } catch (const exception& error) { ViewHelper::Error(error); }
    }
}
