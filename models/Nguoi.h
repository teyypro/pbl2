#ifndef NGUOI_H
#define NGUOI_H

#include "../orm/Entity.h"

// Lớp cha của SinhVien, Phong (NhanVienPhong), Quay (NhanVienQuay)
class Nguoi : public Entity
{
protected:
    string username;

public:
    Nguoi() {}
    Nguoi(const string& u) : username(u) {}
    string getUsername() const { return username; }
};

#endif