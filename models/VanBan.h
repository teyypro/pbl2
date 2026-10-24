#ifndef VANBAN_H
#define VANBAN_H

#include "../orm/Entity.h"
#include "../orm/Navigation.h"

class Phong;
class DangKy;
class PhieuHangDoi;

class VanBan : public Entity
{
private:
    string maVB;
    string tenVB;
    string maPHG;

public:
    // ----- Quan hệ (hai chiều) -----
    Reference<Phong>         phong;        // AGGREGATION: văn bản do phòng nào cấp
    Collection<DangKy>       dangKys;      // ASSOCIATION 1-N: các hồ sơ đăng ký văn bản này
    Collection<PhieuHangDoi> phieus;       // ASSOCIATION 1-N: các phiếu hàng đợi yêu cầu văn bản này

    VanBan();
    VanBan(const string& maVB, const string& tenVB, const string& maPHG);

    string getMaVB() const { return maVB; }
    string getTenVB() const { return tenVB; }
    string getMaPHG() const { return maPHG; }

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    string getKey() const { return maVB; }
    void linkRelations(DbContext* ctx);
    void unlinkRelations();
};

#endif
