#include "RegistrationService.h"

string RegistrationService::nextMaDK()
{
    int mx = 0;
    const MyList<DangKy>& all = dkRepo.getAll();
    for (int i = 0; i < all.getSize(); i++)
    {
        string k = all[i].getMaDK();
        if (k.size() > 2)
        {
            int n = toInt(k.substr(2), 0);
            if (n > mx) mx = n;
        }
    }
    return "DK" + pad(mx + 1, 3);
}

bool RegistrationService::thuocPhong(const string& maVB, const string& maPHG)
{
    VanBan vb;
    return vbRepo.findByKey(maVB, vb) && vb.getMaPHG() == maPHG;
}

MyList<DangKy> RegistrationService::collect(const string& maPHG, const string& status, int mode)
{
    dkRepo.load();
    vbRepo.load();
    MyList<DangKy> r;
    const MyList<DangKy>& all = dkRepo.getAll();
    for (int i = 0; i < all.getSize(); i++)
    {
        const DangKy& d = all[i];
        if (d.getTrangThai() != status) continue;
        if (!thuocPhong(d.getMaVB(), maPHG)) continue;
        if (mode == 1 && !d.getNgayIn().empty()) continue;
        if (mode == 2 && d.getNgayIn().empty()) continue;
        r.add(d);
    }
    return r;
}

bool RegistrationService::loadPending(const string& maDK, DangKy& dk, string& msg)
{
    dkRepo.load();
    if (!dkRepo.findByKey(maDK, dk)) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
    if (dk.getTrangThai() != ST_PENDING)
    {
        msg = "Đơn " + maDK + " không ở trạng thái PENDING.";
        return false;
    }
    return true;
}

RegistrationService::RegistrationService(DangKyRepo& d, VanBanRepo& v, SinhVienRepo& s) : dkRepo(d), vbRepo(v), svRepo(s) {}

bool RegistrationService::getMaSV(const string& username, string& maSV)
{
    svRepo.load();
    const MyList<SinhVien>& all = svRepo.getAll();
    for (int i = 0; i < all.getSize(); i++)
    {
        if (all[i].getUsername() == username) { maSV = all[i].getMaSV(); return true; }
    }
    return false;
}

string RegistrationService::tenVanBan(const string& maVB)
{
    vbRepo.load();
    VanBan vb;
    if (vbRepo.findByKey(maVB, vb)) return vb.getTenVB();
    return maVB;
}

bool RegistrationService::registerDocument(const string& maSV, const string& maVB, string& msg)
{
    dkRepo.load();
    vbRepo.load();
    VanBan vb;
    if (!vbRepo.findByKey(maVB, vb)) { msg = "Văn bản không tồn tại."; return false; }

    const MyList<DangKy>& all = dkRepo.getAll();
    for (int i = 0; i < all.getSize(); i++)
    {
        if (all[i].getMaSV() == maSV && all[i].getMaVB() == maVB &&
            (all[i].getTrangThai() == ST_PENDING || all[i].getTrangThai() == ST_APPROVED))
        {
            msg = "Bạn đã có hồ sơ " + all[i].getMaDK() + " cho văn bản này đang được xử lý.";
            return false;
        }
    }

    DangKy dk(nextMaDK(), maSV, maVB, ST_PENDING, "", today(), "", "");
    dkRepo.add(dk);
    msg = "Đăng ký thành công. Mã đơn: " + dk.getMaDK();
    return true;
}

MyList<DangKy> RegistrationService::getBySinhVien(const string& maSV)
{
    dkRepo.load();
    MyList<DangKy> r;
    const MyList<DangKy>& all = dkRepo.getAll();
    for (int i = 0; i < all.getSize(); i++)
        if (all[i].getMaSV() == maSV) r.add(all[i]);
    return r;
}

MyList<DangKy> RegistrationService::getPending(const string& maPHG) { return collect(maPHG, ST_PENDING, 0); }

bool RegistrationService::approve(const string& maDK, string& msg)
{
    DangKy dk;
    if (!loadPending(maDK, dk, msg)) return false;
    dk.setTrangThai(ST_APPROVED);
    dkRepo.update(dk);
    msg = "Đã duyệt đơn " + maDK + ".";
    return true;
}

bool RegistrationService::reject(const string& maDK, const string& reason, string& msg)
{
    DangKy dk;
    if (!loadPending(maDK, dk, msg)) return false;
    dk.setTrangThai(ST_REJECTED);
    dk.setLyDo(cleanField(reason));
    dkRepo.update(dk);
    msg = "Đã từ chối đơn " + maDK + ".";
    return true;
}

MyList<DangKy> RegistrationService::getForPrint(const string& maPHG) { return collect(maPHG, ST_APPROVED, 1); }

MyList<DangKy> RegistrationService::getForReceive(const string& maPHG) { return collect(maPHG, ST_APPROVED, 2); }

int RegistrationService::exportPrintList(const string& maPHG, const string& path)
{
    MyList<DangKy> ds = getForPrint(maPHG);
    if (ds.isEmpty()) return 0;

    MyList<string> lines;
    lines.add("DANH SACH CAN IN - NGAY " + today());
    lines.add("STT | MaDon | MSSV | TenVanBan | NgayDangKy");
    for (int i = 0; i < ds.getSize(); i++)
    {
        lines.add(to_string(i + 1) + " | " + ds[i].getMaDK() + " | " + ds[i].getMaSV() + " | " +
                  tenVanBan(ds[i].getMaVB()) + " | " + ds[i].getNgayDK());
    }
    if (!FileManager::writeLines(path, lines)) return -1;
    return ds.getSize();
}

bool RegistrationService::setNgayIn(const string& maDK, string date, string& msg)
{
    dkRepo.load();
    DangKy dk;
    if (!dkRepo.findByKey(maDK, dk)) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
    if (dk.getTrangThai() != ST_APPROVED) { msg = "Đơn " + maDK + " chưa được duyệt (APPROVED)."; return false; }
    if (!normalizeDate(date)) { msg = "Ngày không hợp lệ (định dạng dd/mm/yyyy)."; return false; }
    dk.setNgayIn(date);
    dkRepo.update(dk);
    msg = "Đã cập nhật ngày in cho đơn " + maDK + ": " + date;
    return true;
}

bool RegistrationService::setNgayNhan(const string& maDK, string date, string& msg)
{
    dkRepo.load();
    DangKy dk;
    if (!dkRepo.findByKey(maDK, dk)) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
    if (dk.getTrangThai() != ST_APPROVED) { msg = "Đơn " + maDK + " chưa được duyệt (APPROVED)."; return false; }
    if (dk.getNgayIn().empty()) { msg = "Đơn " + maDK + " chưa có ngày in."; return false; }
    if (!normalizeDate(date)) { msg = "Ngày không hợp lệ (định dạng dd/mm/yyyy)."; return false; }
    if (dateToDays(date) < dateToDays(dk.getNgayIn()))
    {
        msg = "Ngày nhận không được trước ngày in (" + dk.getNgayIn() + ").";
        return false;
    }
    dk.setNgayNhan(date);
    dkRepo.update(dk);
    msg = "Đã cập nhật ngày nhận cho đơn " + maDK + ": " + date;
    return true;
}

bool RegistrationService::confirmReceive(const string& maDK, const string& maSVDangPhucVu, string& msg)
{
    dkRepo.load();
    DangKy dk;
    if (!dkRepo.findByKey(maDK, dk)) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
    if (dk.getMaSV() != maSVDangPhucVu) { msg = "Đơn này không thuộc sinh viên đang được phục vụ."; return false; }
    if (dk.getTrangThai() != ST_APPROVED) { msg = "Đơn chưa ở trạng thái APPROVED (hiện tại: " + dk.getTrangThai() + ")."; return false; }
    if (dk.getNgayNhan().empty()) { msg = "Đơn chưa có ngày nhận."; return false; }

    long t = dateToDays(today());
    long n = dateToDays(dk.getNgayNhan());
    if (t < n) { msg = "Chưa đến ngày nhận (" + dk.getNgayNhan() + ")."; return false; }
    if (t > n + 30) { msg = "Đã quá hạn nhận giấy (hạn: ngày nhận + 30 ngày)."; return false; }

    dk.setTrangThai(ST_DONE);
    dkRepo.update(dk);
    msg = "Đã nhận thành công. Đơn " + maDK + " chuyển sang DONE.";
    return true;
}
