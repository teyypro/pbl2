#ifndef AUTH_CONTROLLER_H
#define AUTH_CONTROLLER_H

#include "../services/AuthService.h"
#include "../utils/Helpers.h"

class AuthController {
private:
    AuthService authService;

public:
    // Dang nhap -> tra ve User
    User showLogin();

    // Dang ky benh nhan
    void showRegister();
};

#endif
