#ifndef TAIKHOAN_H
#define TAIKHOAN_H

#include "../orm/Entity.h"
#include "../orm/Navigation.h"

class SinhVien;
class Phong;
class Quay;

class TaiKhoan : public Entity
{
private:
    string username;
    string password;
    string role;

public:
    // ----- Navigation Property (1-1, chiều ngược): tài khoản này thuộc về ai? -----
    Reference<SinhVien> sinhVien;
    Reference<Phong>    phong;
    Reference<Quay>     quay;

    TaiKhoan();
    TaiKhoan(const string& username, const string& password, const string& role);

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getRole() const { return role; }

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    void loadRelations(DbContext* ctx);
};

#endif