#ifndef VANBAN_H
#define VANBAN_H

#include "../orm/Entity.h"
#include "../orm/Navigation.h"

class Phong;
class DangKy;

class VanBan : public Entity
{
private:
    string maVB;
    string tenVB;
    string maPHG;

public:
    // ----- Navigation Property -----
    Reference<Phong>   phong;              // N-1: văn bản do phòng nào cấp
    Collection<DangKy> dangKys;            // 1-N: các hồ sơ đăng ký văn bản này

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
    void loadRelations(DbContext* ctx);
};

#endif