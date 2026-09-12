#include <iostream>
#include <mysql/mysql.h>

int main() {
    // Bước 1: Khởi tạo đối tượng kết nối MySQL
    // mysql_init() tạo ra 1 "ống kết nối" (connection handle)
    // Giống như bạn cầm điện thoại lên, chuẩn bị gọi — nhưng chưa bấm số
    MYSQL* conn = mysql_init(nullptr);

    if (conn == nullptr) {
        std::cerr << "mysql_init() failed" << std::endl;
        return 1;
    }

    // Bước 2: Kết nối thật sự đến MySQL Server
    // Giống như bạn bấm số điện thoại và chờ đầu dây bên kia nhấc máy
    // Tham số: (handle, host, user, password, database, port, socket, flags)
    if (mysql_real_connect(conn, "127.0.0.1", "root", "2712072007Thinh@",
                           nullptr,  // chưa chọn database nào
                           3306,     // port mặc định của MySQL
                           nullptr, 0) == nullptr) {
        std::cerr << "Connection failed: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return 1;
    }

    std::cout << "=== KET NOI MYSQL THANH CONG! ===" << std::endl;
    std::cout << "Server version: " << mysql_get_server_info(conn) << std::endl;

    // Bước 3: Đóng kết nối khi xong
    // Giống như cúp máy sau khi nói chuyện xong — luôn phải làm!
    mysql_close(conn);

    return 0;
}
