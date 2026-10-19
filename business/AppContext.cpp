#include "AppContext.h"

AppContext::AppContext()
    : taiKhoanRepo(DATA_DIR + "TaiKhoan.txt"),
      sinhVienRepo(DATA_DIR + "SinhVien.txt"),
      phongRepo(DATA_DIR + "Phong.txt"),
      quayRepo(DATA_DIR + "Quay.txt"),
      vanBanRepo(DATA_DIR + "VanBan.txt"),
      dangKyRepo(DATA_DIR + "DangKy.txt"),
      phieuRepo(DATA_DIR + "PhieuHangDoi.txt"),
      auth(taiKhoanRepo),
      doc(phongRepo, vanBanRepo),
      reg(dangKyRepo, vanBanRepo, sinhVienRepo),
      dash(phieuRepo, quayRepo, DATA_DIR + "CurrentServing.txt"),
      queue(phieuRepo, quayRepo, sinhVienRepo, dash)
{
}
