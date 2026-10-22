#ifndef SINHVIEN_H
#define SINHVIEN_H

#include "Nguoi.h"
#include "../orm/Navigation.h"

class TaiKhoan;
class DangKy;
class PhieuHangDoi;

class SinhVien : public Nguoi
{
private:
    string maSV;

public:
    // ----- Navigation Property -----
    Reference<TaiKhoan>      taiKhoan;     // 1-1: tài khoản đăng nhập
    Collection<DangKy>       dangKys;      // 1-N: các hồ sơ đã đăng ký
    Collection<PhieuHangDoi> phieus;       // 1-N: các phiếu hàng đợi

    SinhVien();
    SinhVien(const string& maSV, const string& username);

    string getMaSV() const { return maSV; }

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    void loadRelations(DbContext* ctx);
};

#endif