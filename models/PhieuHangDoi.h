#ifndef PHIEUHANGDOI_H
#define PHIEUHANGDOI_H

#include "../orm/Entity.h"
#include "../orm/Navigation.h"

class SinhVien;
class Quay;
class VanBan;

class PhieuHangDoi : public Entity
{
private:
    string maPhieu;
    int soThuTu;
    string maSV;
    string maQuay;
    string trangThai;
    string maVB;                           // yêu cầu: "TUVAN" hoặc mã văn bản cần giải quyết

public:
    // ----- Quan hệ (hai chiều) -----
    Reference<SinhVien> sinhVien;          // ASSOCIATION: phiếu của sinh viên nào
    Reference<Quay>     quay;              // COMPOSITION (phía con): phiếu thuộc quầy nào
    Reference<VanBan>   vanBan;            // ASSOCIATION: văn bản cần giải quyết (NULL nếu là tư vấn)

    PhieuHangDoi();
    PhieuHangDoi(const string& maPhieu, int soThuTu, const string& maSV,
                 const string& maQuay, const string& trangThai, const string& maVB);

    string getMaPhieu() const { return maPhieu; }
    int getSoThuTu() const { return soThuTu; }
    string getMaSV() const { return maSV; }
    string getMaQuay() const { return maQuay; }
    string getTrangThai() const { return trangThai; }
    string getMaVB() const { return maVB; }
    void setTrangThai(const string& v) { trangThai = v; }

    string getDisplay() const;      // số hiển thị trên vé, ví dụ A015
    bool isConsult() const;         // true nếu là phiếu tư vấn
    string tenYeuCau() const;       // "Tư vấn" hoặc tên văn bản (qua navigation vanBan)

    // ORM
    string tableName() const;
    Row columns() const;
    Row toRow() const;
    void fromRow(const Row& row);
    string getKey() const { return maPhieu; }
    void linkRelations(DbContext* ctx);
    void unlinkRelations();
};

#endif
