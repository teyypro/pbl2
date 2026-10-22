#ifndef DANGKY_H
#define DANGKY_H

#include "../orm/Entity.h"
#include "../orm/Navigation.h"

class SinhVien;
class VanBan;

class DangKy : public Entity
{
private:
    string maDK;
    string maSV;
    string maVB;
    string trangThai;
    string lyDoTuChoi;
    string ngayDangKy;
    string ngayIn;
    string ngayNhan;

public:
    // ----- Navigation Property (N-1) -----
    Reference<SinhVien> sinhVien;          // hồ sơ của sinh viên nào
    Reference<VanBan>   vanBan;            // đăng ký văn bản nào

    DangKy();
    DangKy(const string& maDK, const string& maSV, const string& maVB, const string& trangThai,
           const string& lyDoTuChoi, const string& ngayDangKy, const string& ngayIn, const string& ngayNhan);

    string getMaDK() const { return maDK; }
    string getMaSV() const { return maSV; }
    string getMaVB() const { return maVB; }
    string getTrangThai() const { return trangThai; }
    string getLyDo() const { return lyDoTuChoi; }
    string getNgayDK() const { return ngayDangKy; }
    string getNgayIn() const { return ngayIn; }
    string getNgayNhan() const { return ngayNhan; }

    void setTrangThai(const string& v) { trangThai = v; }
    void setLyDo(const string& v) { lyDoTuChoi = v; }
    void setNgayIn(const string& v) { ngayIn = v; }
    void setNgayNhan(const string& v) { ngayNhan = v; }

    string tenVanBan() const;       // tên văn bản, lấy qua navigation vanBan

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    void loadRelations(DbContext* ctx);
};

#endif