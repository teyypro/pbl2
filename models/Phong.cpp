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

void Phong::linkRelations(DbContext* ctx)
{
    TaiKhoan* t = ctx->taiKhoan.findByKey(username);     // 1-1
    if (t != NULL)
    {
        taiKhoan.set(t);
        t->phong.set(this);                              // chiều ngược lại
    }
    // vanBans, quays được VanBan / Quay tự thêm vào khi chúng liên kết
}

void Phong::unlinkRelations()
{
    if (taiKhoan.hasValue()) taiKhoan->phong.clear();
    taiKhoan.clear();

    // Aggregation: văn bản vẫn tồn tại nhưng không còn thuộc phòng nào
    for (int i = 0; i < vanBans.count(); i++) vanBans.items()[i]->phong.clear();
    vanBans.clear();
}

// Composition: quầy không tồn tại độc lập khỏi phòng
void Phong::removeChildren(DbContext* ctx)
{
    MyList<Quay*> copy = quays.items();
    for (int i = 0; i < copy.getSize(); i++) ctx->quay.remove(copy[i]->getKey());
}
