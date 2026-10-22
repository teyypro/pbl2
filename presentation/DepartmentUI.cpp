#include "DepartmentUI.h"
#include <iostream>

Department::Department(const TaiKhoan& a, AppContext& c) : User(a), ctx(c), ok(false)
{
    Phong* p = account.phong.get();                 // Navigation 1-1: TaiKhoan -> Phong
    if (p != NULL)
    {
        phong = *p;
        ok = true;
    }
}

void Department::listWithIndex(MyList<DangKy>& ds)
{
    for (int i = 0; i < ds.getSize(); i++)
    {
        cout << i + 1 << ". ";
        printDangKy(ds[i]);
    }
}

void Department::xemChoDuyet()
{
    MyList<DangKy> ds = ctx.reg.getPending(phong.getMaPHG());
    cout << "\n=== HỒ SƠ CHỜ DUYỆT ===\n";
    if (ds.isEmpty()) cout << "Không có hồ sơ nào chờ duyệt.\n";
    listWithIndex(ds);
    pauseScreen();
}

// UC03
void Department::duyetHoSo()
{
    MyList<DangKy> ds = ctx.reg.getPending(phong.getMaPHG());
    cout << "\n=== DUYỆT HỒ SƠ ===\n";
    if (ds.isEmpty()) { cout << "Không có hồ sơ nào chờ duyệt.\n"; pauseScreen(); return; }
    listWithIndex(ds);

    string line = readLine("Chọn các hồ sơ (VD: 1 2 4, hoặc ALL, Enter để hủy): ");
    MyList<int> sel = parseSelection(line, ds.getSize());
    if (sel.isEmpty()) { cout << "Không có hồ sơ nào được chọn.\n"; pauseScreen(); return; }

    for (int k = 0; k < sel.getSize(); k++)
    {
        const DangKy& d = ds[sel[k]];
        cout << "\n--- Đơn " << d.getMaDK() << " (" << d.tenVanBan()
             << ", MSSV " << d.getMaSV() << ") ---\n";
        cout << "1. Đồng ý\n2. Từ chối\n";
        int c = readInt("Chọn: ");

        string msg;
        if (c == 1)
        {
            ctx.reg.approve(d.getMaDK(), msg);
        }
        else if (c == 2)
        {
            string lyDo = readLine("Nhập lý do từ chối: ");
            while (lyDo.empty()) lyDo = readLine("Lý do không được để trống. Nhập lại: ");
            ctx.reg.reject(d.getMaDK(), lyDo, msg);
        }
        else
        {
            msg = "Bỏ qua đơn " + d.getMaDK() + ".";
        }
        cout << msg << endl;
    }
    pauseScreen();
}

// UC04
void Department::xuatDanhSachIn()
{
    int n = ctx.reg.exportPrintList(phong.getMaPHG());
    if (n < 0) cout << "Không ghi được file PrintList.txt\n";
    else if (n == 0) cout << "Không có hồ sơ nào cần in.\n";
    else cout << "Đã xuất " << n << " hồ sơ ra file PrintList.txt (thư mục data).\n";
    pauseScreen();
}

// UC05 + UC06: ngày in / ngày nhận = thời điểm bấm cập nhật (không cần nhập)
void Department::capNhatNgay(bool ngayIn)
{
    MyList<DangKy> ds = ngayIn ? ctx.reg.getForPrint(phong.getMaPHG())
                               : ctx.reg.getForReceive(phong.getMaPHG());
    cout << (ngayIn ? "\n=== CẬP NHẬT NGÀY IN ===\n" : "\n=== CẬP NHẬT NGÀY NHẬN ===\n");
    if (ds.isEmpty())
    {
        cout << (ngayIn ? "Không có hồ sơ nào đang chờ in.\n"
                        : "Không có hồ sơ nào đã in để cập nhật ngày nhận.\n");
        pauseScreen();
        return;
    }
    listWithIndex(ds);

    string line = readLine("Chọn các hồ sơ (VD: 1 3, hoặc ALL, Enter để hủy): ");
    MyList<int> sel = parseSelection(line, ds.getSize());
    if (sel.isEmpty()) { cout << "Không có hồ sơ nào được chọn.\n"; pauseScreen(); return; }

    // Thời điểm chọn cập nhật: lấy một lần cho cả lượt chọn
    string thoiGian = nowDateTime();
    cout << "\nThời điểm cập nhật: " << thoiGian << endl;
    for (int k = 0; k < sel.getSize(); k++)
    {
        string msg;
        if (ngayIn) ctx.reg.setNgayIn(ds[sel[k]].getMaDK(), thoiGian, msg);
        else ctx.reg.setNgayNhan(ds[sel[k]].getMaDK(), thoiGian, msg);
        cout << msg << endl;
    }
    pauseScreen();
}

void Department::showMenu()
{
    if (!ok)
    {
        cout << "Tài khoản chưa được gắn với phòng ban nào.\n";
        pauseScreen();
        return;
    }
    while (true)
    {
        clearScreen();
        cout << "====== " << phong.getTenPhong() << " ======\n";
        cout << "1. Xem hồ sơ chờ duyệt\n";
        cout << "2. Duyệt hồ sơ\n";
        cout << "3. Xuất danh sách in\n";
        cout << "4. Cập nhật ngày in\n";
        cout << "5. Cập nhật ngày nhận\n";
        cout << "6. Đăng xuất\n";
        int c = readInt("Chọn: ");
        if (c == 1) xemChoDuyet();
        else if (c == 2) duyetHoSo();
        else if (c == 3) xuatDanhSachIn();
        else if (c == 4) capNhatNgay(true);
        else if (c == 5) capNhatNgay(false);
        else if (c == 6) return;
        else { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); }
    }
}