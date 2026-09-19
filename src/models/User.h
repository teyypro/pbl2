#ifndef USER_H
#define USER_H

#include <string>
using namespace std;

struct User {
    int id = 0;
    string username;
    string password_hash;
    string full_name;
    string email;
    string phone;
    string role;       // admin, receptionist, doctor, patient
    string status;     // active, locked, pending
};

#endif