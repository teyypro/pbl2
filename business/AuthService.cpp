#include "AuthService.h"

AuthService::AuthService(DbContext& d) : db(d) {}

// TaiKhoan trả về đã được nạp sẵn navigation (sinhVien / phong / quay)
bool AuthService::login(const string& username, const string& password, TaiKhoan& out)
{
    TaiKhoan t;
    if (!db.taiKhoan.findByKey(username, t)) return false;   // username là khóa chính
    if (t.getPassword() != password) return false;
    out = t;
    return true;
}