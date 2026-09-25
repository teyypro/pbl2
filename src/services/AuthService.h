#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include "../repositories/UserRepository.h"
#include "../repositories/PatientRepository.h"
#include "../utils/Helpers.h"

class AuthService {
private:
    UserRepository userRepo;
    PatientRepository patientRepo;

public:
    // Dang nhap -> tra ve User (id=0 neu that bai)
    User login(const string& username, const string& password);

    // Dang ky benh nhan moi
    bool registerPatient(const string& username, const string& password,
                         const string& fullName, const string& dob,
                         const string& gender, const string& phone,
                         const string& email, const string& address);
};

#endif
