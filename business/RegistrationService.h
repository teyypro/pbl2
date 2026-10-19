#ifndef REGISTRATIONSERVICE_H
#define REGISTRATIONSERVICE_H

#include "../data/Repositories.h"

// Nghiệp vụ đăng ký, duyệt, in, trả giấy tờ
class RegistrationService
{
private:
    DangKyRepo& dkRepo;
    VanBanRepo& vbRepo;
    SinhVienRepo& svRepo;

    string nextMaDK();

    bool thuocPhong(const string& maVB, const string& maPHG);

    // mode: 0 = mọi hồ sơ, 1 = chưa có ngày in, 2 = đã có ngày in
    MyList<DangKy> collect(const string& maPHG, const string& status, int mode);

    bool loadPending(const string& maDK, DangKy& dk, string& msg);

public:
    RegistrationService(DangKyRepo& d, VanBanRepo& v, SinhVienRepo& s);

    bool getMaSV(const string& username, string& maSV);

    string tenVanBan(const string& maVB);

    // UC01: Đăng ký giấy tờ -> tạo hồ sơ PENDING
    bool registerDocument(const string& maSV, const string& maVB, string& msg);

    // UC02: Hồ sơ của một sinh viên
    MyList<DangKy> getBySinhVien(const string& maSV);

    // UC03: Hồ sơ chờ duyệt của phòng
    MyList<DangKy> getPending(const string& maPHG);

    bool approve(const string& maDK, string& msg);

    bool reject(const string& maDK, const string& reason, string& msg);

    // Hồ sơ APPROVED chưa có ngày in
    MyList<DangKy> getForPrint(const string& maPHG);

    // Hồ sơ APPROVED đã in (để cập nhật ngày nhận)
    MyList<DangKy> getForReceive(const string& maPHG);

    // UC04: Xuất danh sách in -> file. Trả về số hồ sơ, -1 nếu lỗi ghi file
    int exportPrintList(const string& maPHG, const string& path);

    // UC05: Cập nhật ngày in
    bool setNgayIn(const string& maDK, string date, string& msg);

    // UC06: Cập nhật ngày nhận (>= ngày in)
    bool setNgayNhan(const string& maDK, string date, string& msg);

    // UC12: Xác nhận trả giấy. Điều kiện: ngayNhan <= hôm nay <= ngayNhan + 30 ngày
    bool confirmReceive(const string& maDK, const string& maSVDangPhucVu, string& msg);
};

#endif
