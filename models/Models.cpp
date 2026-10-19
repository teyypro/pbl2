#include "Models.h"

MyList<string> splitN(const string& line, int n)
{
    MyList<string> f = split(line, '|');
    while (f.getSize() < n) f.add("");
    return f;
}

string cleanField(string s)
{
    for (size_t i = 0; i < s.size(); i++)
        if (s[i] == '|') s[i] = '/';
    return s;
}

Nguoi::Nguoi() {}

Nguoi::Nguoi(const string& u) : username(u) {}

Nguoi::~Nguoi() {}

string Nguoi::getUsername() const { return username; }

TaiKhoan::TaiKhoan() {}

TaiKhoan::TaiKhoan(const string& u, const string& p, const string& r) : username(u), password(p), role(r) {}

string TaiKhoan::getUsername() const { return username; }

string TaiKhoan::getPassword() const { return password; }

string TaiKhoan::getRole() const { return role; }

string TaiKhoan::getKey() const { return username; }

string TaiKhoan::toLine() const { return username + "|" + password + "|" + role; }

TaiKhoan TaiKhoan::fromLine(const string& line)
{
    MyList<string> f = splitN(line, 3);
    return TaiKhoan(f[0], f[1], f[2]);
}

SinhVien::SinhVien() {}

SinhVien::SinhVien(const string& ma, const string& u) : Nguoi(u), maSV(ma) {}

string SinhVien::getMaSV() const { return maSV; }

string SinhVien::getKey() const { return maSV; }

string SinhVien::toLine() const { return maSV + "|" + username; }

SinhVien SinhVien::fromLine(const string& line)
{
    MyList<string> f = splitN(line, 2);
    return SinhVien(f[0], f[1]);
}

Phong::Phong() {}

Phong::Phong(const string& ma, const string& ten, const string& u) : Nguoi(u), maPHG(ma), tenPhong(ten) {}

string Phong::getMaPHG() const { return maPHG; }

string Phong::getTenPhong() const { return tenPhong; }

string Phong::getKey() const { return maPHG; }

string Phong::toLine() const { return maPHG + "|" + tenPhong + "|" + username; }

Phong Phong::fromLine(const string& line)
{
    MyList<string> f = splitN(line, 3);
    return Phong(f[0], f[1], f[2]);
}

Quay::Quay() : active(true) {}

Quay::Quay(const string& mq, const string& mp, const string& u, bool a)
    : Nguoi(u), maQuay(mq), maPHG(mp), active(a) {}

string Quay::getMaQuay() const { return maQuay; }

string Quay::getMaPHG() const { return maPHG; }

bool Quay::isActive() const { return active; }

string Quay::getKey() const { return maQuay; }

string Quay::toLine() const { return maQuay + "|" + maPHG + "|" + username + "|" + (active ? "1" : "0"); }

Quay Quay::fromLine(const string& line)
{
    MyList<string> f = splitN(line, 4);
    return Quay(f[0], f[1], f[2], f[3] != "0");
}

VanBan::VanBan() {}

VanBan::VanBan(const string& ma, const string& ten, const string& mp) : maVB(ma), tenVB(ten), maPHG(mp) {}

string VanBan::getMaVB() const { return maVB; }

string VanBan::getTenVB() const { return tenVB; }

string VanBan::getMaPHG() const { return maPHG; }

string VanBan::getKey() const { return maVB; }

string VanBan::toLine() const { return maVB + "|" + tenVB + "|" + maPHG; }

VanBan VanBan::fromLine(const string& line)
{
    MyList<string> f = splitN(line, 3);
    return VanBan(f[0], f[1], f[2]);
}

DangKy::DangKy() {}

DangKy::DangKy(const string& dk, const string& sv, const string& vb, const string& tt,
       const string& lydo, const string& ndk, const string& ni, const string& nn)
    : maDK(dk), maSV(sv), maVB(vb), trangThai(tt), lyDoTuChoi(lydo),
      ngayDangKy(ndk), ngayIn(ni), ngayNhan(nn) {}

string DangKy::getMaDK() const { return maDK; }

string DangKy::getMaSV() const { return maSV; }

string DangKy::getMaVB() const { return maVB; }

string DangKy::getTrangThai() const { return trangThai; }

string DangKy::getLyDo() const { return lyDoTuChoi; }

string DangKy::getNgayDK() const { return ngayDangKy; }

string DangKy::getNgayIn() const { return ngayIn; }

string DangKy::getNgayNhan() const { return ngayNhan; }

void DangKy::setTrangThai(const string& v) { trangThai = v; }

void DangKy::setLyDo(const string& v) { lyDoTuChoi = v; }

void DangKy::setNgayIn(const string& v) { ngayIn = v; }

void DangKy::setNgayNhan(const string& v) { ngayNhan = v; }

string DangKy::getKey() const { return maDK; }

string DangKy::toLine() const
{
    return maDK + "|" + maSV + "|" + maVB + "|" + trangThai + "|" + lyDoTuChoi + "|" +
           ngayDangKy + "|" + ngayIn + "|" + ngayNhan;
}

DangKy DangKy::fromLine(const string& line)
{
    MyList<string> f = splitN(line, 8);
    return DangKy(f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7]);
}

PhieuHangDoi::PhieuHangDoi() : soThuTu(0) {}

PhieuHangDoi::PhieuHangDoi(const string& p, int stt, const string& sv, const string& q, const string& tt)
    : maPhieu(p), soThuTu(stt), maSV(sv), maQuay(q), trangThai(tt) {}

string PhieuHangDoi::getMaPhieu() const { return maPhieu; }

int PhieuHangDoi::getSoThuTu() const { return soThuTu; }

string PhieuHangDoi::getMaSV() const { return maSV; }

string PhieuHangDoi::getMaQuay() const { return maQuay; }

string PhieuHangDoi::getTrangThai() const { return trangThai; }

void PhieuHangDoi::setTrangThai(const string& v) { trangThai = v; }

string PhieuHangDoi::getDisplay() const { return "A" + pad(soThuTu, 3); }

string PhieuHangDoi::getKey() const { return maPhieu; }

string PhieuHangDoi::toLine() const
{
    return maPhieu + "|" + to_string(soThuTu) + "|" + maSV + "|" + maQuay + "|" + trangThai;
}

PhieuHangDoi PhieuHangDoi::fromLine(const string& line)
{
    MyList<string> f = splitN(line, 5);
    return PhieuHangDoi(f[0], toInt(f[1]), f[2], f[3], f[4]);
}
