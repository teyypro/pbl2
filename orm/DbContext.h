#ifndef DBCONTEXT_H
#define DBCONTEXT_H

#include "Database.h"
#include "DbSet.h"
#include "../models/TaiKhoan.h"
#include "../models/SinhVien.h"
#include "../models/Phong.h"
#include "../models/Quay.h"
#include "../models/VanBan.h"
#include "../models/DangKy.h"
#include "../models/PhieuHangDoi.h"

// Gom toàn bộ các bảng của hệ thống. Tầng Business chỉ làm việc với lớp này.
class DbContext
{
private:
    Database database_;

public:
    DbSet<TaiKhoan>     taiKhoan;
    DbSet<SinhVien>     sinhVien;
    DbSet<Phong>        phong;
    DbSet<Quay>         quay;
    DbSet<VanBan>       vanBan;
    DbSet<DangKy>       dangKy;
    DbSet<PhieuHangDoi> phieu;

    DbContext(const string& folder);

    // Không cho sao chép (các DbSet giữ con trỏ tới database_ và DbContext)
    DbContext(const DbContext&) = delete;
    DbContext& operator=(const DbContext&) = delete;

    Database& database() { return database_; }
};

#endif