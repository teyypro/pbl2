#ifndef REGISTRATIONSERVICE_H
#define REGISTRATIONSERVICE_H

#include "../orm/DbContext.h"
#include "../models/Constants.h"

// Nghiệp vụ đăng ký, duyệt, in, trả giấy tờ (xử lý trên RAM, mỗi thay đổi ghi về TXT)
// Mọi mốc thời gian lưu theo định dạng dd/mm/yyyy HH:MM:SS
class RegistrationService
{
private:
    DbContext& db;

    // mode: 0 = tất cả, 1 = chưa có ngày in, 2 = đã có ngày in,
    //       3 = đã có ngày in nhưng chưa có ngày nhận
    MyList<DangKy*> collect(const string& maPHG, const string& status, int mode);
    DangKy* findPending(const string& maDK, string& msg);

public:
    RegistrationService(DbContext& db);

    // UC01, UC02
    bool registerDocument(const string& maSV, const string& maVB, string& msg);
    MyList<DangKy*> getBySinhVien(const string& maSV);

    // UC03
    MyList<DangKy*> getPending(const string& maPHG);
    bool approve(const string& maDK, string& msg);
    bool reject(const string& maDK, const string& reason, string& msg);

    // UC04, UC05, UC06  (thoiGian = thời điểm bấm cập nhật, do tầng trên truyền vào)
    MyList<DangKy*> getForPrint(const string& maPHG);
    MyList<DangKy*> getForReceive(const string& maPHG);
    int exportPrintList(const string& maPHG);
    bool setNgayIn(const string& maDK, const string& thoiGian, string& msg);
    bool setNgayNhan(const string& maDK, const string& thoiGian, string& msg);

    // UC12: danh sách giấy của sinh viên cần xử lý trả tại một phòng
    MyList<DangKy*> getForReturn(const string& maSV, const string& maPHG);
    bool confirmReceive(const string& maDK, const string& maSVDangPhucVu, string& msg);
};

#endif
