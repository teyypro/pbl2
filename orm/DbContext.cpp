#include "DbContext.h"

DbContext::DbContext(const string& folder)
    : database_(folder),
      taiKhoan(&database_, this),
      sinhVien(&database_, this),
      phong(&database_, this),
      quay(&database_, this),
      vanBan(&database_, this),
      dangKy(&database_, this),
      phieu(&database_, this)
{
}

void DbContext::loadAll()
{
    // Bước 1: file -> RAM (tạo đối tượng, chưa có quan hệ)
    taiKhoan.load();
    sinhVien.load();
    phong.load();
    quay.load();
    vanBan.load();
    dangKy.load();
    phieu.load();

    // Bước 2: dựng quan hệ hai chiều giữa các đối tượng bằng khóa ngoại
    sinhVien.linkAll();      // SinhVien  <-> TaiKhoan
    phong.linkAll();         // Phong     <-> TaiKhoan
    quay.linkAll();          // Quay      <-> TaiKhoan, Phong
    vanBan.linkAll();        // VanBan    <-> Phong
    dangKy.linkAll();        // DangKy    <-> SinhVien, VanBan
    phieu.linkAll();         // PhieuHangDoi <-> Quay, SinhVien, VanBan
}
