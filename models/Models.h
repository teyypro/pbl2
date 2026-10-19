#ifndef MODELS_H
#define MODELS_H

#include "../utils/Utils.h"

// ---------- Hằng số ----------
const string ROLE_STUDENT = "STUDENT";
const string ROLE_PHONG   = "PHONG";
const string ROLE_QUAY    = "QUAY";

const string ST_PENDING  = "PENDING";
const string ST_APPROVED = "APPROVED";
const string ST_REJECTED = "REJECTED";
const string ST_DONE     = "DONE";

const string Q_WAITING  = "WAITING";
const string Q_SERVING  = "SERVING";
const string Q_FINISHED = "FINISHED";

// Tách dòng và đảm bảo đủ n trường (thiếu thì bù chuỗi rỗng)
MyList<string> splitN(const string& line, int n);

// Không cho phép ký tự '|' trong dữ liệu người dùng nhập
string cleanField(string s);

// =====================================================
// Lớp cha
// =====================================================
class Nguoi
{
protected:
    string username;

public:
    Nguoi();
    Nguoi(const string& u);
    virtual ~Nguoi();
    string getUsername() const;
};

// =====================================================
// TaiKhoan
// =====================================================
class TaiKhoan
{
private:
    string username;
    string password;
    string role;

public:
    TaiKhoan();
    TaiKhoan(const string& u, const string& p, const string& r);

    string getUsername() const;
    string getPassword() const;
    string getRole() const;

    string getKey() const;
    string toLine() const;
    static TaiKhoan fromLine(const string& line);
};

// =====================================================
// SinhVien (kế thừa Nguoi)
// =====================================================
class SinhVien : public Nguoi
{
private:
    string maSV;

public:
    SinhVien();
    SinhVien(const string& ma, const string& u);

    string getMaSV() const;

    string getKey() const;
    string toLine() const;
    static SinhVien fromLine(const string& line);
};

// =====================================================
// Phong = NhanVienPhong (kế thừa Nguoi)
// =====================================================
class Phong : public Nguoi
{
private:
    string maPHG;
    string tenPhong;

public:
    Phong();
    Phong(const string& ma, const string& ten, const string& u);

    string getMaPHG() const;
    string getTenPhong() const;

    string getKey() const;
    string toLine() const;
    static Phong fromLine(const string& line);
};
typedef Phong NhanVienPhong;

// =====================================================
// Quay = NhanVienQuay (kế thừa Nguoi)
// =====================================================
class Quay : public Nguoi
{
private:
    string maQuay;
    string maPHG;
    bool active;

public:
    Quay();
    Quay(const string& mq, const string& mp, const string& u, bool a);

    string getMaQuay() const;
    string getMaPHG() const;
    bool isActive() const;

    string getKey() const;
    string toLine() const;
    static Quay fromLine(const string& line);
};
typedef Quay NhanVienQuay;

// =====================================================
// VanBan
// =====================================================
class VanBan
{
private:
    string maVB;
    string tenVB;
    string maPHG;

public:
    VanBan();
    VanBan(const string& ma, const string& ten, const string& mp);

    string getMaVB() const;
    string getTenVB() const;
    string getMaPHG() const;

    string getKey() const;
    string toLine() const;
    static VanBan fromLine(const string& line);
};

// =====================================================
// DangKy
// =====================================================
class DangKy
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
    DangKy();
    DangKy(const string& dk, const string& sv, const string& vb, const string& tt,
           const string& lydo, const string& ndk, const string& ni, const string& nn);

    string getMaDK() const;
    string getMaSV() const;
    string getMaVB() const;
    string getTrangThai() const;
    string getLyDo() const;
    string getNgayDK() const;
    string getNgayIn() const;
    string getNgayNhan() const;

    void setTrangThai(const string& v);
    void setLyDo(const string& v);
    void setNgayIn(const string& v);
    void setNgayNhan(const string& v);

    string getKey() const;
    string toLine() const;
    static DangKy fromLine(const string& line);
};

// =====================================================
// PhieuHangDoi
// =====================================================
class PhieuHangDoi
{
private:
    string maPhieu;
    int soThuTu;
    string maSV;
    string maQuay;
    string trangThai;

public:
    PhieuHangDoi();
    PhieuHangDoi(const string& p, int stt, const string& sv, const string& q, const string& tt);

    string getMaPhieu() const;
    int getSoThuTu() const;
    string getMaSV() const;
    string getMaQuay() const;
    string getTrangThai() const;
    void setTrangThai(const string& v);

    // Số hiển thị trên vé, ví dụ A015
    string getDisplay() const;

    string getKey() const;
    string toLine() const;
    static PhieuHangDoi fromLine(const string& line);
};

#endif
