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

void VanBan::linkRelations(DbContext* ctx)
{
    Phong* p = ctx->phong.findByKey(maPHG);              // N-1
    if (p != NULL)
    {
        phong.set(p);
        p->vanBans.add(this);                            // chiều ngược lại: Phong.vanBans
    }
    // dangKys, phieus được DangKy / PhieuHangDoi tự thêm vào khi chúng liên kết
}

void VanBan::unlinkRelations()
{
    if (phong.hasValue()) phong->vanBans.remove(this);
    phong.clear();

    for (int i = 0; i < dangKys.count(); i++) dangKys.items()[i]->vanBan.clear();
    dangKys.clear();
    for (int i = 0; i < phieus.count(); i++) phieus.items()[i]->vanBan.clear();
    phieus.clear();
}
