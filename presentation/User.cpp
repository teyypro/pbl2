#include "User.h"

User::User(const TaiKhoan& a) : account(a) {}

User::~User() {}

void printDangKy(RegistrationService& reg, const DangKy& d)
{
    cout << d.getMaDK() << " | " << reg.tenVanBan(d.getMaVB()) << " | MSSV: " << d.getMaSV() << endl;
    cout << "   Trạng thái: " << d.getTrangThai() << " | Ngày đăng ký: " << d.getNgayDK() << endl;
    if (!d.getNgayIn().empty()) cout << "   Ngày in: " << d.getNgayIn() << endl;
    if (!d.getNgayNhan().empty()) cout << "   Ngày nhận: " << d.getNgayNhan() << endl;
    if (!d.getLyDo().empty()) cout << "   Lý do từ chối: " << d.getLyDo() << endl;
}
