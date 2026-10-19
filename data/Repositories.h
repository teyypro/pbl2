#ifndef REPOSITORIES_H
#define REPOSITORIES_H

#include "../models/Models.h"
#include "TxtRepository.h"

typedef TxtRepository<TaiKhoan>     TaiKhoanRepo;
typedef TxtRepository<SinhVien>     SinhVienRepo;
typedef TxtRepository<Phong>        PhongRepo;
typedef TxtRepository<Quay>         QuayRepo;
typedef TxtRepository<VanBan>       VanBanRepo;
typedef TxtRepository<DangKy>       DangKyRepo;
typedef TxtRepository<PhieuHangDoi> PhieuRepo;

#endif
