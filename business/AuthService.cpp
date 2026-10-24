#include "AuthService.h"

AuthService::AuthService(DbContext& d) : db(d) {}

// Trả về tài khoản trên RAM; từ đó đi tiếp qua navigation (sinhVien / phong / quay)
TaiKhoan* AuthService::login(const string& username, const string& password)
{
    TaiKhoan* t = db.taiKhoan.findByKey(username);       // username là khóa chính
    if (t == NULL || t->getPassword() != password) return NULL;
    return t;
}
