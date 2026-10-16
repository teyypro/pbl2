#include "DashboardView.h"
#include "ViewHelper.h"
#include <conio.h>
#include <io.h>
#include <thread>
#include <chrono>
#include <iostream>

using namespace std;
DashboardView::DashboardView(QueueService& queue) : queue(queue) {}
void DashboardView::PrintDepartment(const string& title, const QueueSnapshot& snapshot) {
    cout << "\n" << title << "\nDang phuc vu: " << (snapshot.current.empty() ? "---" : snapshot.current)
        << "\nCho tiep theo (" << snapshot.waiting.Size() << " ve dang cho):\n";
    for (int i = 0; i < snapshot.waiting.Size() && i < 5; ++i) cout << snapshot.waiting.GetByIndex(i) << '\n';
}
void DashboardView::showMenu() {
    bool interactive = _isatty(_fileno(stdin)) != 0;
    while (true) {
        try {
            DashboardSnapshot snapshot = queue.GetDashboard();
            cout << "\n----------------------------------------\n";
            cout << "DASHBOARD - " << snapshot.updatedAt << "\nCap nhat moi 5 giay. Nhan Q de quay lai.\n";
            PrintDepartment("CONG TAC SINH VIEN", snapshot.ctsv);
            PrintDepartment("DAO TAO", snapshot.daotao);
            cout << flush;
        } catch (const exception& error) { ViewHelper::Error(error); }
        if (!interactive) return;
        for (int i = 0; i < 50; ++i) {
            if (_kbhit()) { int key = _getch(); if (key == 'q' || key == 'Q' || key == 27) return; }
            this_thread::sleep_for(chrono::milliseconds(100));
        }
    }
}
