#ifndef PHONG_H
#define PHONG_H

#include "Nguoi.h"

class VanBan;
class Quay;

// Phòng ban (đồng thời là nhân viên phòng đăng nhập bằng username)
class Phong : public Nguoi
{
private:
    string maPHG;
    string tenPhong;

public:
    // ----- Quan hệ (hai chiều) -----
    // (taiKhoan: ASSOCIATION 1-1, kế thừa từ Nguoi)
    Collection<VanBan> vanBans;            // AGGREGATION 1-N: văn bản phòng cấp (xóa phòng, văn bản vẫn còn)
    Collection<Quay>   quays;              // COMPOSITION 1-N: quầy thuộc phòng (xóa phòng -> xóa quầy)

    Phong();
    Phong(const string& maPHG, const string& tenPhong, const string& username);

    string getMaPHG() const { return maPHG; }
    string getTenPhong() const { return tenPhong; }

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    string getKey() const { return maPHG; }
    void linkRelations(DbContext* ctx);
    void unlinkRelations();
    void removeChildren(DbContext* ctx);
};

typedef Phong NhanVienPhong;

#endif
