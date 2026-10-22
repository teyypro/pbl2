#include "Phong.h"
#include "../orm/DbContext.h"

Phong::Phong() {}

Phong::Phong(const string& ma, const string& ten, const string& u)
    : Nguoi(u), maPHG(ma), tenPhong(ten) {}

string Phong::tableName() const { return "Phong"; }

Row Phong::columns() const { return split("maPHG|tenPhong|username", '|'); }

Row Phong::toRow() const
{
    Row r;
    r.add(maPHG);
    r.add(tenPhong);
    r.add(username);
    return r;
}

void Phong::fromRow(const Row& r)
{
    maPHG = r[0];
    tenPhong = r[1];
    username = r[2];
}

void Phong::loadRelations(DbContext* ctx)
{
    taiKhoan.load(&ctx->taiKhoan, username);             // 1-1
    vanBans.load(&ctx->vanBan, "maPHG", maPHG);          // 1-N: VanBan.maPHG == Phong.maPHG
    quays.load(&ctx->quay, "maPHG", maPHG);              // 1-N: Quay.maPHG == Phong.maPHG
}