#include "repositories/UserRepository.h"

UserRepository::UserRepository() {}

User UserRepository::parseLine(const string& line) {
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

string UserRepository::toLine(const User& u) {
    return to_string(u.id) + "|" + u.username + "|" + u.password_hash + "|"
         + u.full_name + "|" + u.email + "|" + u.phone + "|" + u.role + "|" + u.status;
}

User UserRepository::findByLogin(const string& username, const string& password) {
    vector<string> lines = fm.readAllLines(filename);
    for (auto& line : lines) {
        User u = parseLine(line);
        if (u.username == username && u.password_hash == password && u.status == "active") {
            return u;
        }
    }
    return User();
}

User UserRepository::findById(int id) {
    vector<string> lines = fm.readAllLines(filename);
    for (auto& line : lines) {
        User u = parseLine(line);
        if (u.id == id) return u;
    }
    return User();
}

User UserRepository::findByUsername(const string& username) {
    vector<string> lines = fm.readAllLines(filename);
    for (auto& line : lines) {
        User u = parseLine(line);
        if (u.username == username) return u;
    }
    return User();
}

vector<User> UserRepository::findAll() {
    vector<User> users;
    vector<string> lines = fm.readAllLines(filename);
    for (auto& line : lines) {
        users.push_back(parseLine(line));
    }
    return users;
}

int UserRepository::create(const User& user) {
    User u = user;
    u.id = fm.getNextId(filename);
    fm.appendLine(filename, toLine(u));
    return u.id;
}

bool UserRepository::update(const User& user) {
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
