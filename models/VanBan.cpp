#include "VanBan.h"
#include "../orm/DbContext.h"

VanBan::VanBan() {}

VanBan::VanBan(const string& ma, const string& ten, const string& mp)
    : maVB(ma), tenVB(ten), maPHG(mp) {}

string VanBan::tableName() const { return "VanBan"; }

Row VanBan::columns() const { return split("maVB|tenVB|maPHG", '|'); }

Row VanBan::toRow() const
{
    Row r;
    r.add(maVB);
    r.add(tenVB);
    r.add(maPHG);
    return r;
}

void VanBan::fromRow(const Row& r)
{
    maVB = r[0];
    tenVB = r[1];
    maPHG = r[2];
}

void VanBan::loadRelations(DbContext* ctx)
{
    phong.load(&ctx->phong, maPHG);                      // N-1
    dangKys.load(&ctx->dangKy, "maVB", maVB);            // 1-N: DangKy.maVB == VanBan.maVB
}