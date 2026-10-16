#include "Application.h"
#include "LoginView.h"
#include "StudentView.h"
#include "ProcessingView.h"
#include "CounterView.h"
#include "GuestView.h"
#include "DashboardView.h"
#include "ViewHelper.h"
#include "../utils/InputHelper.h"
#include <iostream>

using namespace std;
Application::Application(const string& directory)
    : context(directory, clock), auth(context), requests(context), processing(context), queue(context), counter(context) {}
void Application::OpenSession() {
    LoginView login(auth, session);
    login.showMenu();
    if (!session) return;
    unique_ptr<BaseView> view;
    switch (session->GetRole()) {
    case UserRole::STUDENT: view = make_unique<StudentView>(*session, requests); break;
    case UserRole::PROCESSING_STAFF: view = make_unique<ProcessingView>(*session, processing); break;
    case UserRole::COUNTER_STAFF: view = make_unique<CounterView>(*session, counter); break;
    }
    if (view) view->showMenu();
    view.reset();
    auth.Logout(session);
}
void Application::Run(bool dashboardOnly) {
    queue.Refresh();
    if (dashboardOnly) { DashboardView dashboard(queue); dashboard.showMenu(); return; }
    while (cin) {
        try {
            cout << "\nQUAN LY DANG KY VA NHAN GIAY TO SINH VIEN\n1. Dang nhap / dang ky\n2. Guest - lay so\n3. Dashboard\n0. Thoat\n";
            int choice = InputHelper::Integer("Chon: ", 0, 3);
            if (choice == 0) return;
            if (choice == 1) OpenSession();
            if (choice == 2) { GuestView guest(queue); guest.showMenu(); }
            if (choice == 3) { DashboardView dashboard(queue); dashboard.showMenu(); }
        } catch (const exception& error) { ViewHelper::Error(error); }
    }
}
