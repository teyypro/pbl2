#include "Quay.h"
#include "Constants.h"
#include "../orm/DbContext.h"

Quay::Quay() : active(true) {}

Quay::Quay(const string& mq, const string& mp, const string& u, bool a)
    : Nguoi(u), maQuay(mq), maPHG(mp), active(a) {}

MyList<PhieuHangDoi*> Quay::waitingTickets() const
{
    MyList<PhieuHangDoi*> cho;
    const MyList<PhieuHangDoi*>& all = phieus.items();
    for (int i = 0; i < all.getSize(); i++)
        if (all[i]->getTrangThai() == Q_WAITING) cho.add(all[i]);

    // thứ tự gọi thực tế là theo số thứ tự (sắp xếp nổi bọt)
    for (int a = 0; a < cho.getSize(); a++)
        for (int b = a + 1; b < cho.getSize(); b++)
            if (cho[b]->getSoThuTu() < cho[a]->getSoThuTu())
            {
                PhieuHangDoi* tmp = cho[a]; cho[a] = cho[b]; cho[b] = tmp;
            }
    return cho;
}

PhieuHangDoi* Quay::servingTicket() const
{
    const MyList<PhieuHangDoi*>& all = phieus.items();
    for (int i = 0; i < all.getSize(); i++)
        if (all[i]->getTrangThai() == Q_SERVING) return all[i];
    return NULL;
}

int Quay::countWaiting() const
{
    int c = 0;
    const MyList<PhieuHangDoi*>& all = phieus.items();
    for (int i = 0; i < all.getSize(); i++)
        if (all[i]->getTrangThai() == Q_WAITING) c++;
    return c;
}

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

void Quay::linkRelations(DbContext* ctx)
{
    TaiKhoan* t = ctx->taiKhoan.findByKey(username);     // 1-1
    if (t != NULL)
    {
        taiKhoan.set(t);
        t->quay.set(this);
    }

    Phong* p = ctx->phong.findByKey(maPHG);              // N-1: Quay.maPHG -> Phong (khóa chính)
    if (p != NULL)
    {
        phong.set(p);
        p->quays.add(this);                              // chiều ngược lại: Phong.quays
    }
    // phieus được PhieuHangDoi tự thêm vào khi nó liên kết
}

void Quay::unlinkRelations()
{
    if (taiKhoan.hasValue()) taiKhoan->quay.clear();
    taiKhoan.clear();

    if (phong.hasValue()) phong->quays.remove(this);
    phong.clear();

    for (int i = 0; i < phieus.count(); i++) phieus.items()[i]->quay.clear();
    phieus.clear();
}

// Composition: phiếu không tồn tại độc lập khỏi quầy
void Quay::removeChildren(DbContext* ctx)
{
    MyList<PhieuHangDoi*> copy = phieus.items();
    for (int i = 0; i < copy.getSize(); i++) ctx->phieu.remove(copy[i]->getKey());
}
