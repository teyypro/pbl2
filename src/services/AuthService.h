#ifndef AUTH_SERVICE_H
#define AUTH_SERVICE_H

#include "UserRepository.h"
#include "PatientRepository.h"
#include "Helpers.h"

class AuthService {
private:
    UserRepository userRepo;
    PatientRepository patientRepo;

public:
    // Dang nhap -> tra ve User (id=0 neu that bai)
    User login(const string& username, const string& password) {
        return userRepo.findByLogin(username, password);
    }

    // Dang ky benh nhan moi
    bool registerPatient(const string& username, const string& password,
                         const string& fullName, const string& dob,
                         const string& gender, const string& phone,
                         const string& email, const string& address) {
        // Kiem tra username da ton tai chua
        User existing = userRepo.findByUsername(username);
        if (existing.id != 0) {
            cout << "Username da ton tai!" << endl;
            return false;
        }

        // Tao user
        User user;
        user.username = username;
        user.password_hash = password; // Luu tru don gian
        user.full_name = fullName;
        user.email = email;
        user.phone = phone;
        user.role = "patient";
        user.status = "active";

        int userId = userRepo.create(user);
        if (userId <= 0) {
            cout << "Tao tai khoan that bai!" << endl;
            return false;
        }

        // Tao patient
        Patient patient;
        patient.user_id = userId;
        patient.patient_code = Helpers::generateCode("BN");
        patient.full_name = fullName;
        patient.dob = dob;
        patient.gender = gender;
        patient.phone = phone;
        patient.email = email;
        patient.address = address;
        patient.status = "active";

        int patientId = patientRepo.create(patient);
        if (patientId <= 0) {
            cout << "Tao ho so benh nhan that bai!" << endl;
            return false;
        }

        cout << "Dang ky thanh cong! Ma benh nhan: " << patient.patient_code << endl;
        return true;
    }
};

#endif
