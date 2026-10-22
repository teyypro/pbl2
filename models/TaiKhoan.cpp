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

// Quan hệ 1-1 (chiều ngược): khóa ngoại nằm ở bảng kia (cột username)
void TaiKhoan::loadRelations(DbContext* ctx)
{
    sinhVien.load(&ctx->sinhVien, "username", username);
    phong.load(&ctx->phong, "username", username);
    quay.load(&ctx->quay, "username", username);
}