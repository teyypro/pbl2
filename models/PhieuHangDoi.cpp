#include "PhieuHangDoi.h"
#include "Constants.h"
#include "../orm/DbContext.h"

PhieuHangDoi::PhieuHangDoi() : soThuTu(0) {}

PhieuHangDoi::PhieuHangDoi(const string& p, int stt, const string& sv, const string& q,
                           const string& tt, const string& vb)
    : maPhieu(p), soThuTu(stt), maSV(sv), maQuay(q), trangThai(tt), maVB(vb) {}

string PhieuHangDoi::getDisplay() const { return "A" + pad(soThuTu, 3); }

// Dòng cũ chưa có cột maVB (rỗng) được xem là tư vấn
bool PhieuHangDoi::isConsult() const { return maVB.empty() || maVB == REQ_CONSULT; }

string PhieuHangDoi::tenYeuCau() const
{
    if (isConsult()) return "Tư vấn";
    VanBan* vb = vanBan.get();
    if (vb != NULL) return vb->getTenVB();
    return maVB;                                         // chưa nạp quan hệ hoặc không tìm thấy
}

string PhieuHangDoi::tableName() const { return "PhieuHangDoi"; }

Row PhieuHangDoi::columns() const { return split("maPhieu|soThuTu|maSV|maQuay|trangThai|maVB", '|'); }

Row PhieuHangDoi::toRow() const
{
    Row r;
    r.add(maPhieu);
    r.add(to_string(soThuTu));
    r.add(maSV);
    r.add(maQuay);
    r.add(trangThai);
    r.add(maVB);
    return r;
}

void PhieuHangDoi::fromRow(const Row& r)
{
    maPhieu = r[0];
    soThuTu = toInt(r[1]);
    maSV = r[2];
    maQuay = r[3];
    trangThai = r[4];
    maVB = r[5];
}

void PhieuHangDoi::loadRelations(DbContext* ctx)
{
    sinhVien.load(&ctx->sinhVien, maSV);                 // N-1
    quay.load(&ctx->quay, maQuay);                       // N-1
    if (!isConsult()) vanBan.load(&ctx->vanBan, maVB);   // N-1 (tư vấn thì không có văn bản)
}