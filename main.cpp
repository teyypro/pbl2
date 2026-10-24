#include <iostream>
#include "business/AppContext.h"
#include "presentation/MainUI.h"

#ifdef _WIN32
#include <windows.h>
#endif

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001);      // hiển thị tiếng Việt UTF-8
    SetConsoleCP(65001);
#endif

    AppContext ctx("data/");

    if (!ctx.db.database().tableExists("TaiKhoan"))
    {
        cout << "Không tìm thấy data/TaiKhoan.txt\n";
        cout << "Hãy tạo thư mục data/ và các file dữ liệu mẫu (xem mục 7 của tài liệu).\n";
        return 1;
    }

    ctx.db.loadAll();       // nạp TOÀN BỘ dữ liệu lên RAM + dựng quan hệ giữa các đối tượng
    ctx.dash.refresh();     // đồng bộ CurrentServing.txt với trạng thái hàng đợi vừa nạp

    MainUI ui(ctx);
    ui.run();
    return 0;
}
