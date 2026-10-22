#include "SinhVien.h"
#include "../orm/DbContext.h"

SinhVien::SinhVien() {}

SinhVien::SinhVien(const string& ma, const string& u) : Nguoi(u), maSV(ma) {}

string SinhVien::tableName() const { return "SinhVien"; }

Row SinhVien::columns() const { return split("maSV|username", '|'); }

Row SinhVien::toRow() const
{
    Row r;
    r.add(maSV);
    r.add(username);
    return r;
}

void SinhVien::fromRow(const Row& r)
{
    maSV = r[0];
    username = r[1];
}

void SinhVien::loadRelations(DbContext* ctx)
{
    taiKhoan.load(&ctx->taiKhoan, username);             // 1-1: SinhVien.username -> TaiKhoan (khóa chính)
    dangKys.load(&ctx->dangKy, "maSV", maSV);            // 1-N: DangKy.maSV == SinhVien.maSV
    phieus.load(&ctx->phieu, "maSV", maSV);              // 1-N: PhieuHangDoi.maSV == SinhVien.maSV
}