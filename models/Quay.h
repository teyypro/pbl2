#ifndef QUAY_H
#define QUAY_H

#include "Nguoi.h"

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
    // ----- Quan hệ (hai chiều) -----
    // (taiKhoan: ASSOCIATION 1-1, kế thừa từ Nguoi)
    Reference<Phong>         phong;        // quầy thuộc phòng nào (phía "N" của Phong ◆— Quay)
    Collection<PhieuHangDoi> phieus;       // COMPOSITION 1-N: xóa quầy -> xóa phiếu

    Quay();
    Quay(const string& maQuay, const string& maPHG, const string& username, bool active);

    string getMaQuay() const { return maQuay; }
    string getMaPHG() const { return maPHG; }
    bool isActive() const { return active; }

    // Nghiệp vụ trên đối tượng (duyệt phieus trên RAM)
    MyList<PhieuHangDoi*> waitingTickets() const;   // phiếu WAITING, sắp tăng dần theo số thứ tự
    PhieuHangDoi* servingTicket() const;            // phiếu đang SERVING (NULL nếu không có)
    int countWaiting() const;

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    string getKey() const { return maQuay; }
    void linkRelations(DbContext* ctx);
    void unlinkRelations();
    void removeChildren(DbContext* ctx);
};

typedef Quay NhanVienQuay;

#endif
