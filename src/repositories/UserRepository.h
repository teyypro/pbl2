#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "../database/FileManager.h"
#include "../models/User.h"
#include <vector>

class UserRepository {
private:
    FileManager fm;
    string filename = "users.txt";

    User parseLine(const string& line);
    string toLine(const User& u);

public:
    UserRepository();

    User findByLogin(const string& username, const string& password);
    User findById(int id);
    User findByUsername(const string& username);
    vector<User> findAll();
    int create(const User& user);
    bool update(const User& user);
};

#endif
