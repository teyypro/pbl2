#include "CounterUI.h"

void Counter::hienThiHoSoSV(const string& maSV)
{
    MyList<DangKy> ds = ctx.reg.getBySinhVien(maSV);
    cout << "\n--- Thông tin sinh viên MSSV " << maSV << " ---\n";
    if (ds.isEmpty()) cout << "Sinh viên chưa có hồ sơ nào.\n";
    for (int i = 0; i < ds.getSize(); i++)
    {
        printDangKy(ctx.reg, ds[i]);
        cout << endl;
    }
}

void Counter::goiSoTiepTheo()
{
    PhieuHangDoi p;
    string msg;
    if (!ctx.queue.callNext(quay.getMaQuay(), p, msg))
    {
        cout << msg << endl;
        pauseScreen();
        return;
    }
    cout << "\n>>> MỜI SỐ " << p.getDisplay() << " (MSSV " << p.getMaSV()
         << ") ĐẾN QUẦY " << quay.getMaQuay() << " <<<\n";
    hienThiHoSoSV(p.getMaSV());
    pauseScreen();
}

void Counter::xemHoSo()
{
    string maSV = readLine("Nhập MSSV (Enter = sinh viên đang phục vụ): ");
    if (maSV.empty())
    {
        PhieuHangDoi cur;
        if (!ctx.queue.getServing(quay.getMaQuay(), cur))
        {
            cout << "Quầy chưa phục vụ sinh viên nào. Hãy gọi số trước.\n";
            pauseScreen();
            return;
        }
        maSV = cur.getMaSV();
    }
    hienThiHoSoSV(maSV);
    pauseScreen();
}

void Counter::xacNhanTraGiay()
{
    PhieuHangDoi cur;
    if (!ctx.queue.getServing(quay.getMaQuay(), cur))
    {
        cout << "Quầy chưa phục vụ sinh viên nào. Hãy gọi số trước.\n";
        pauseScreen();
        return;
    }
    hienThiHoSoSV(cur.getMaSV());
    string maDK = toUpper(readLine("Nhập mã đơn cần trả giấy (VD: DK001): "));
    string msg;
    ctx.reg.confirmReceive(maDK, cur.getMaSV(), msg);
    cout << msg << endl;
    pauseScreen();
}

Counter::Counter(const TaiKhoan& a, AppContext& c)
    : User(a), ctx(c), ok(ctx.queue.findQuayByUsername(a.getUsername(), quay))
{
}

void Counter::showMenu()
{
    if (!ok)
    {
        cout << "Tài khoản chưa được gắn với quầy nào.\n";
        pauseScreen();
        return;
    }
    while (true)
    {
        clearScreen();
        PhieuHangDoi cur;
        bool has = ctx.queue.getServing(quay.getMaQuay(), cur);
        cout << "====== QUẦY " << quay.getMaQuay() << " ======\n";
        cout << "Đang phục vụ: " << (has ? cur.getDisplay() + " (MSSV " + cur.getMaSV() + ")" : string("---")) << endl;
        cout << "Đang chờ    : " << ctx.queue.countWaiting(quay.getMaQuay()) << " người\n\n";
        cout << "1. Gọi số tiếp theo\n";
        cout << "2. Xem hồ sơ sinh viên\n";
        cout << "3. Xác nhận trả giấy\n";
        cout << "4. Đăng xuất\n";
        int c = readInt("Chọn: ");
        if (c == 1) goiSoTiepTheo();
        else if (c == 2) xemHoSo();
        else if (c == 3) xacNhanTraGiay();
        else if (c == 4) return;
        else { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); }
    }
}
