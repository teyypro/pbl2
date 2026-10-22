#include "DangKy.h"
#include "../orm/DbContext.h"

DangKy::DangKy() {}

DangKy::DangKy(const string& dk, const string& sv, const string& vb, const string& tt,
               const string& lydo, const string& ndk, const string& ni, const string& nn)
    : maDK(dk), maSV(sv), maVB(vb), trangThai(tt), lyDoTuChoi(lydo),
      ngayDangKy(ndk), ngayIn(ni), ngayNhan(nn) {}

// Đi qua Navigation Property: không cần tự truy vấn bảng VanBan
string DangKy::tenVanBan() const
{
    VanBan* vb = vanBan.get();
    if (vb != NULL) return vb->getTenVB();
    return maVB;                                         // chưa nạp quan hệ hoặc không tìm thấy
}

string DangKy::tableName() const { return "DangKy"; }

Row DangKy::columns() const
{
    return split("maDK|maSV|maVB|trangThai|lyDoTuChoi|ngayDangKy|ngayIn|ngayNhan", '|');
}

Row DangKy::toRow() const
{
    Row r;
    r.add(maDK);
    r.add(maSV);
    r.add(maVB);
    r.add(trangThai);
    r.add(lyDoTuChoi);
    r.add(ngayDangKy);
    r.add(ngayIn);
    r.add(ngayNhan);
    return r;
}

void DangKy::fromRow(const Row& r)
{
    maDK = r[0];
    maSV = r[1];
    maVB = r[2];
    trangThai = r[3];
    lyDoTuChoi = r[4];
    ngayDangKy = r[5];
    ngayIn = r[6];
    ngayNhan = r[7];
}

void DangKy::loadRelations(DbContext* ctx)
{
    sinhVien.load(&ctx->sinhVien, maSV);                 // N-1: DangKy.maSV -> SinhVien
    vanBan.load(&ctx->vanBan, maVB);                     // N-1: DangKy.maVB -> VanBan
}