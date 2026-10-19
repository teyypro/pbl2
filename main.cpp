#include <iostream>
#include "business/AppContext.h"
#include "presentation/MainUI.h"

int main()
{
#ifdef _WIN32
    SetConsoleOutputCP(65001); // hiển thị tiếng Việt UTF-8
    SetConsoleCP(65001);
#endif

    if (!FileManager::exists(DATA_DIR + "TaiKhoan.txt"))
    {
        cout << "Không tìm thấy " << DATA_DIR << "TaiKhoan.txt\n";
        cout << "Hãy tạo thư mục data/ và các file dữ liệu mẫu (xem mục 3 của tài liệu).\n";
        return 1;
    }

    AppContext ctx;
    MainUI ui(ctx);
    ui.run();
    return 0;
}
