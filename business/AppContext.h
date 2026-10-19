#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include "AuthService.h"
#include "DocumentService.h"
#include "RegistrationService.h"
#include "DashboardService.h"
#include "QueueService.h"

// Gom toàn bộ repository + service (thứ tự khai báo = thứ tự khởi tạo)
struct AppContext
{
    TaiKhoanRepo taiKhoanRepo;
    SinhVienRepo sinhVienRepo;
    PhongRepo phongRepo;
    QuayRepo quayRepo;
    VanBanRepo vanBanRepo;
    DangKyRepo dangKyRepo;
    PhieuRepo phieuRepo;

    AuthService auth;
    DocumentService doc;
    RegistrationService reg;
    DashboardService dash;
    QueueService queue;

    AppContext();
};

#endif
