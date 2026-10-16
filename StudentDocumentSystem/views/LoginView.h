#pragma once
#include "BaseView.h"
#include "../services/AuthService.h"

using namespace std;
class LoginView : public BaseView {
private:
    AuthService& auth;
    unique_ptr<User>& session;
public:
    LoginView(AuthService& auth, unique_ptr<User>& session);
    void showMenu() override;
};
