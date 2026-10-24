#include "CounterUI.h"
#include <iostream>

Counter::Counter(TaiKhoan* a, AppContext& c) : User(a), ctx(c), quay(NULL), ok(false)
{
    quay = account->quay.get();                     // Navigation 1-1: TaiKhoan -> Quay (đủ quan hệ phong, phieus)
    ok = (quay != NULL);
}

// Hiển thị toàn bộ hồ sơ của một sinh viên (dùng cho option 2: tra cứu theo MSSV)
void Counter::hienThiHoSoSV(const string& maSV)
{
    MyList<DangKy*> ds = ctx.reg.getBySinhVien(maSV);
    cout << "\n--- Thông tin sinh viên MSSV " << maSV << " ---\n";
    if (ds.isEmpty()) cout << "Sinh viên chưa có hồ sơ nào.\n";
    for (int i = 0; i < ds.getSize(); i++)
    {
        printDangKy(ds[i]);
        cout << endl;
    }
}

// UC10 + UC11: gọi số, chỉ hiển thị yêu cầu của tờ số sinh viên đã lấy (giấy tờ / tư vấn)
void Counter::goiSoTiepTheo()
{
    PhieuHangDoi* p = NULL;
    string msg;
    if (!ctx.queue.callNext(quay, p, msg))
    {
        cout << msg << endl;
        pauseScreen();
        return;
    }
    cout << "\n>>> MỜI SỐ " << p->getDisplay() << " ĐẾN QUẦY " << quay->getMaQuay() << " <<<\n";
    cout << "    MSSV    : " << p->getMaSV() << endl;
    cout << "    Yêu cầu : " << p->tenYeuCau() << endl;
    pauseScreen();
}

void Counter::xemHoSo()
{
    string maSV = readLine("Nhập MSSV (Enter = sinh viên đang phục vụ): ");
    if (maSV.empty())
    {
        PhieuHangDoi* cur = quay->servingTicket();
        if (cur == NULL)
        {
            cout << "Quầy chưa phục vụ sinh viên nào. Hãy gọi số trước.\n";
            pauseScreen();
            return;
        }
        maSV = cur->getMaSV();
    }
    hienThiHoSoSV(maSV);
    pauseScreen();
}

// UC12: danh sách giấy cần xử lý trả hiển thị riêng ở đây
void Counter::xacNhanTraGiay()
{
    PhieuHangDoi* cur = quay->servingTicket();
    if (cur == NULL)
    {
        cout << "Quầy chưa phục vụ sinh viên nào. Hãy gọi số trước.\n";
        pauseScreen();
        return;
    }

    // Giấy của sinh viên đang phục vụ, thuộc phòng của quầy, đã có ngày nhận, chưa trả
    MyList<DangKy*> ds = ctx.reg.getForReturn(cur->getMaSV(), quay->getMaPHG());
    cout << "\n=== GIẤY CẦN TRẢ - MSSV " << cur->getMaSV() << " ===\n";
    if (ds.isEmpty())
    {
        cout << "Sinh viên không có giấy nào cần trả tại phòng này.\n";
        pauseScreen();
        return;
    }
    for (int i = 0; i < ds.getSize(); i++)
    {
        cout << i + 1 << ". ";
        printDangKy(ds[i]);
    }

    string line = readLine("Chọn các giấy cần trả (VD: 1 2, hoặc ALL, Enter để hủy): ");
    MyList<int> sel = parseSelection(line, ds.getSize());
    if (sel.isEmpty()) { cout << "Không có giấy nào được chọn.\n"; pauseScreen(); return; }

    for (int k = 0; k < sel.getSize(); k++)
    {
        string msg;
        ctx.reg.confirmReceive(ds[sel[k]]->getMaDK(), cur->getMaSV(), msg);
        cout << msg << endl;
    }
    pauseScreen();
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
        PhieuHangDoi* cur = quay->servingTicket();

        Phong* ph = quay->phong.get();              // Navigation N-1: Quay -> Phong
        cout << "====== QUẦY " << quay->getMaQuay();
        if (ph != NULL) cout << " (" << ph->getTenPhong() << ")";
        cout << " ======\n";
        if (cur != NULL)
        {
            cout << "Đang phục vụ: " << cur->getDisplay() << " (MSSV " << cur->getMaSV() << ")\n";
            cout << "Yêu cầu     : " << cur->tenYeuCau() << endl;
        }
        else
        {
            cout << "Đang phục vụ: ---\n";
        }
        cout << "Đang chờ    : " << quay->countWaiting() << " người\n\n";
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
