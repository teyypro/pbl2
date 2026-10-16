#pragma once
#include "BaseView.h"
#include "../services/RequestService.h"
#include "../templates/MyStack.h"

using namespace std;
class StudentView : public BaseView {
private:
    enum class ScreenId { MENU, DOCUMENTS, CREATE, REQUESTS, DETAILS };
    const User& user;
    RequestService& requests;
    MyStack<ScreenId> history;
    ScreenId screen = ScreenId::MENU;
    string selectedRequest;
    void GoTo(ScreenId target);
    void Back();
    void Create();
public:
    StudentView(const User& user, RequestService& requests);
    void showMenu() override;
};
