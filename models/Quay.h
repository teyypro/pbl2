#ifndef QUAY_H
#define QUAY_H

#include "Nguoi.h"
#include "../orm/Navigation.h"

class TaiKhoan;
class Phong;
class PhieuHangDoi;

// Quầy tiếp nhận (đồng thời là nhân viên quầy đăng nhập bằng username)
class Quay : public Nguoi
{
private:
    string maQuay;
    string maPHG;
    bool active;

public:
    // ----- Navigation Property -----
    Reference<TaiKhoan>      taiKhoan;     // 1-1
    Reference<Phong>         phong;        // N-1: quầy thuộc phòng nào
    Collection<PhieuHangDoi> phieus;       // 1-N: các phiếu của quầy

    Quay();
    Quay(const string& maQuay, const string& maPHG, const string& username, bool active);

    string getMaQuay() const { return maQuay; }
    string getMaPHG() const { return maPHG; }
    bool isActive() const { return active; }

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    void loadRelations(DbContext* ctx);
};

typedef Quay NhanVienQuay;

#endif