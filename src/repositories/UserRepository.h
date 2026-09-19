#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "FileManager.h"
#include "User.h"
#include <vector>

class UserRepository {
private:
    FileManager fm;
    string filename = "users.txt";

    User parseLine(const string& line) {
        User u;
        vector<string> f = FileManager::split(line);
        if (f.size() >= 8) {
            u.id = stoi(f[0]);
            u.username = f[1];
            u.password_hash = f[2];
            u.full_name = f[3];
            u.email = f[4];
            u.phone = f[5];
            u.role = f[6];
            u.status = f[7];
        }
        return u;
    }

    string toLine(const User& u) {
        return to_string(u.id) + "|" + u.username + "|" + u.password_hash + "|"
             + u.full_name + "|" + u.email + "|" + u.phone + "|" + u.role + "|" + u.status;
    }

public:
    UserRepository() {}

    User findByLogin(const string& username, const string& password) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            User u = parseLine(line);
            if (u.username == username && u.password_hash == password && u.status == "active") {
                return u;
            }
        }
        return User();
    }

    User findById(int id) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            User u = parseLine(line);
            if (u.id == id) return u;
        }
        return User();
    }

    User findByUsername(const string& username) {
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            User u = parseLine(line);
            if (u.username == username) return u;
        }
        return User();
    }

    vector<User> findAll() {
        vector<User> users;
        vector<string> lines = fm.readAllLines(filename);
        for (auto& line : lines) {
            users.push_back(parseLine(line));
        }
        return users;
    }

    int create(const User& user) {
        User u = user;
        u.id = fm.getNextId(filename);
        fm.appendLine(filename, toLine(u));
        return u.id;
    }

    bool update(const User& user) {
        string header = fm.readHeader(filename);
        vector<string> lines = fm.readAllLines(filename);
        vector<string> newLines;
        bool found = false;
        for (auto& line : lines) {
            User u = parseLine(line);
            if (u.id == user.id) {
                newLines.push_back(toLine(user));
                found = true;
            } else {
                newLines.push_back(line);
            }
        }
        if (found) fm.writeAllLines(filename, header, newLines);
        return found;
    }
};

#endif
