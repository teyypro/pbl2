#ifndef PHONG_H
#define PHONG_H

#include "Nguoi.h"
#include "../orm/Navigation.h"

class TaiKhoan;
class VanBan;
class Quay;

// Phòng ban (đồng thời là nhân viên phòng đăng nhập bằng username)
class Phong : public Nguoi
{
private:
    string maPHG;
    string tenPhong;

public:
    // ----- Navigation Property -----
    Reference<TaiKhoan> taiKhoan;          // 1-1
    Collection<VanBan>  vanBans;           // 1-N: các văn bản phòng cấp
    Collection<Quay>    quays;             // 1-N: các quầy thuộc phòng

    Phong();
    Phong(const string& maPHG, const string& tenPhong, const string& username);

    string getMaPHG() const { return maPHG; }
    string getTenPhong() const { return tenPhong; }

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    void loadRelations(DbContext* ctx);
};

typedef Phong NhanVienPhong;

#endif