#include "Quay.h"
#include "../orm/DbContext.h"

Quay::Quay() : active(true) {}

Quay::Quay(const string& mq, const string& mp, const string& u, bool a)
    : Nguoi(u), maQuay(mq), maPHG(mp), active(a) {}

string Quay::tableName() const { return "Quay"; }

Row Quay::columns() const { return split("maQuay|maPHG|username|active", '|'); }

Row Quay::toRow() const
{
    Row r;
    r.add(maQuay);
    r.add(maPHG);
    r.add(username);
    r.add(active ? "1" : "0");
    return r;
}

void Quay::fromRow(const Row& r)
{
    maQuay = r[0];
    maPHG = r[1];
    username = r[2];
    active = (r[3] != "0");
}

void Quay::loadRelations(DbContext* ctx)
{
    taiKhoan.load(&ctx->taiKhoan, username);             // 1-1
    phong.load(&ctx->phong, maPHG);                      // N-1: Quay.maPHG -> Phong (khóa chính)
    phieus.load(&ctx->phieu, "maQuay", maQuay);          // 1-N: PhieuHangDoi.maQuay == Quay.maQuay
}