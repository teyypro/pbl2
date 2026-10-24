#ifndef NGUOI_H
#define NGUOI_H

#include "../orm/Entity.h"
#include "../orm/Navigation.h"

class TaiKhoan;

// INHERITANCE: lớp cha của SinhVien, Phong (NhanVienPhong), Quay (NhanVienQuay)
// ASSOCIATION 1-1 hai chiều: Nguoi <-> TaiKhoan (TaiKhoan cũng trỏ ngược lại)
class Nguoi : public Entity
{
protected:
    string username;

public:
    Reference<TaiKhoan> taiKhoan;          // tài khoản đăng nhập của người này

    Nguoi() {}
    Nguoi(const string& u) : username(u) {}
    string getUsername() const { return username; }
};

#endif
