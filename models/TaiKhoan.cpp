#include "TaiKhoan.h"
#include "../orm/DbContext.h"

TaiKhoan::TaiKhoan() {}

TaiKhoan::TaiKhoan(const string& u, const string& p, const string& r)
    : username(u), password(p), role(r) {}

string TaiKhoan::tableName() const { return "TaiKhoan"; }

Row TaiKhoan::columns() const { return split("username|password|role", '|'); }

Row TaiKhoan::toRow() const
{
    Row r;
    r.add(username);
    r.add(password);
    r.add(role);
    return r;
}

void TaiKhoan::fromRow(const Row& r)
{
    username = r[0];
    password = r[1];
    role = r[2];
}

// Xóa tài khoản: các đối tượng trỏ tới nó không còn tài khoản
void TaiKhoan::unlinkRelations()
{
    if (sinhVien.hasValue()) sinhVien->taiKhoan.clear();
    if (phong.hasValue())    phong->taiKhoan.clear();
    if (quay.hasValue())     quay->taiKhoan.clear();
    sinhVien.clear();
    phong.clear();
    quay.clear();
}
