#ifndef SINHVIEN_H
#define SINHVIEN_H

#include "Nguoi.h"

class DangKy;
class PhieuHangDoi;

class SinhVien : public Nguoi
{
private:
    string maSV;

public:
    // ----- Quan hệ (hai chiều) -----
    // (taiKhoan: ASSOCIATION 1-1, kế thừa từ Nguoi)
    Collection<DangKy>       dangKys;      // COMPOSITION 1-N: xóa sinh viên -> xóa hồ sơ
    Collection<PhieuHangDoi> phieus;       // ASSOCIATION 1-N: phiếu sinh viên đã lấy

    SinhVien();
    SinhVien(const string& maSV, const string& username);

    string getMaSV() const { return maSV; }

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    string getKey() const { return maSV; }
    void linkRelations(DbContext* ctx);
    void unlinkRelations();
    void removeChildren(DbContext* ctx);
};

#endif
