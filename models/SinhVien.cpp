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

void SinhVien::linkRelations(DbContext* ctx)
{
    TaiKhoan* t = ctx->taiKhoan.findByKey(username);     // 1-1: SinhVien.username -> TaiKhoan (khóa chính)
    if (t != NULL)
    {
        taiKhoan.set(t);
        t->sinhVien.set(this);                           // chiều ngược lại
    }
    // dangKys, phieus được các DangKy / PhieuHangDoi tự thêm vào khi chúng liên kết
}

void SinhVien::unlinkRelations()
{
    if (taiKhoan.hasValue()) taiKhoan->sinhVien.clear();
    taiKhoan.clear();

    // Association: phiếu vẫn tồn tại (thuộc quầy) nhưng không còn sinh viên
    for (int i = 0; i < phieus.count(); i++) phieus.items()[i]->sinhVien.clear();
    phieus.clear();
}

// Composition: hồ sơ không tồn tại độc lập khỏi sinh viên
void SinhVien::removeChildren(DbContext* ctx)
{
    MyList<DangKy*> copy = dangKys.items();              // sao chép vì danh sách thay đổi khi xóa
    for (int i = 0; i < copy.getSize(); i++) ctx->dangKy.remove(copy[i]->getKey());
}
