#include "services/AuthService.h"

User AuthService::login(const string& username, const string& password) {
    return userRepo.findByLogin(username, password);
}

bool AuthService::registerPatient(const string& username, const string& password,
                                   const string& fullName, const string& dob,
                                   const string& gender, const string& phone,
                                   const string& email, const string& address) {
    User existing = userRepo.findByUsername(username);
    if (existing.id != 0) {
        cout << "Username da ton tai!" << endl;
        return false;
    }

    User user;
    user.username = username;
    user.password_hash = password;
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
