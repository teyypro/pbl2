# PBL2 — C++ console, VS Code và CMD

Ứng dụng viết bằng `.h`, `.cpp`, nhập/xuất với cin/cout. Không dùng Windows.h, <filesystem> hay project Visual Studio.

## Chạy bằng F5

1. Trong VS Code chọn File → Open Folder, mở **Prj2** hoặc **StudentDocumentSystem**. Cả hai có cấu hình riêng phù hợp với thư mục đang mở; giữ nguyên cấu trúc thư mục hiện tại.
2. Mở `StudentDocumentSystem/main.cpp`.
3. Bấm **F5**, chọn **PBL2 - F5 console CMD** nếu được hỏi.
4. Phím F5 gọi make/g++ để chỉ biên dịch phần thay đổi rồi mở console ngoài để nhập/xuất. Terminal mặc định được đặt là Command Prompt. Không cần bấm nút Run hoặc chạy script.

Các file `.vscode/launch.json`, `tasks.json`, `settings.json` cấu hình F5, không phải mã nghiệp vụ. Không chọn “build active file” vì dự án gồm nhiều .cpp. Cấu hình trỏ tới g++ và gdb trong `C:/msys64/ucrt64/bin`, đã có trên máy cùng extension Microsoft C/C++.

`.vscode/build.mk` là cấu hình biên dịch nhanh. Lần đầu tạo các file `.o` và `.d` trong `.build/`, tối đa 4 lệnh compiler chạy song song. Lần sau không sửa mã thì bỏ qua biên dịch/liên kết; sửa `.cpp` thì chỉ biên dịch file đó, sửa `.h` thì biên dịch các file phụ thuộc. Không xóa `.build/` nếu muốn lần F5 sau chạy nhanh. Thư mục này là bộ nhớ đệm compiler, không phải dữ liệu kiểm tra. Đổi cấu hình build.mk sẽ biên dịch lại các file để áp dụng cờ mới.

Nếu phím F5 cá nhân được gán cho Code Runner, executor C++ của workspace cũng đã được cấu hình để gọi cùng makefile rồi mở `start "" cmd /k ...`. Không dùng lệnh `g++ *.cpp`: lệnh đó chỉ lấy main.cpp tại thư mục chính, bỏ sót các file trong thư mục con và thiếu liên kết OpenSSL. Khi vừa cập nhật cấu hình, đóng terminal cũ, mở lại terminal CMD và thử F5. Nếu vẫn xuất hiện lệnh cũ, chạy lệnh **Developer: Reload Window** trong Command Palette của VS Code.

## Chạy bằng CMD

Từ Prj2:

```bat
cd StudentDocumentSystem
StudentDocumentSystem.exe
```

Phím F5 đã biên dịch tự động. Nếu muốn tự biên dịch bằng CMD, lệnh tương đương là:

```bat
set "PATH=C:\msys64\ucrt64\bin;%PATH%"
g++ -std=c++17 -g -O0 -Wall -Wextra main.cpp models\*.cpp enums\*.cpp utils\*.cpp repositories\*.cpp services\*.cpp views\*.cpp -lcrypto -o StudentDocumentSystem.exe
StudentDocumentSystem.exe
```

Mở nhiều CMD, cùng chuyển vào StudentDocumentSystem và chạy exe. Mỗi cửa sổ có phiên đăng nhập riêng. Mở dashboard bằng:

```bat
StudentDocumentSystem.exe --dashboard
```

Q/Esc để quay lại; khoảng 5 giây in snapshot mới. Không xóa màn hình bằng Windows API. Khi stdin bị chuyển hướng, dashboard in một lần để hỗ trợ kiểm tra tự động.

## Dữ liệu thật

Mở **StudentDocumentSystem/data/** trong Explorer của VS Code:

| File | Nội dung |
|---|---|
| students.txt | Danh sách sinh viên được phép đăng ký |
| users.txt | Tài khoản, salt và mật khẩu đã băm |
| documents.txt | Giấy tờ, phòng và đơn giá |
| requests.txt | Các đơn được tạo khi chạy |
| queue_ctsv.txt | Lịch sử vé CTSV |
| queue_daotao.txt | Lịch sử vé Đào tạo |

Ba file cuối ban đầu rỗng. Sau khi lập đơn/lấy số, mở lại file sẽ thấy bản ghi. system.lock là file khóa tự tạo, rỗng là bình thường. Không sửa TXT trong khi các chương trình đang chạy.

`<filesystem>` là thư viện C++ xử lý đường dẫn/thư mục, không phải nơi lưu dữ liệu. Phiên bản này dùng fstream đọc/ghi TXT và đường dẫn tương đối `data`. Có thể truyền `--data data`. Chạy các CMD từ cùng thư mục dự án; đường dẫn tuyệt đối có dấu phụ thuộc bảng mã CRT/Windows.

## Tài khoản và demo

Tất cả tài khoản dưới đây có mật khẩu **123456**:

| Tài khoản | Vai trò |
|---|---|
| 22110001, 22110002 | Sinh viên |
| processor_ctsv | Xử lý hồ sơ CTSV |
| processor_dt | Xử lý hồ sơ Đào tạo |
| counter_ctsv | Quầy CTSV |
| counter_dt | Quầy Đào tạo |

22110003 và 22110004 có trong danh sách nhưng chưa có tài khoản, dùng để thử đăng ký. Không cho tự chọn quyền nhân viên. Mật khẩu nhập bằng getline thông thường, **hiện khi gõ**.

Demo:

1. Sinh viên lập đơn DOC001, số lượng 2, phí 0.
2. processor_ctsv xem mã đơn → duyệt → đánh dấu đã in → hẹn nhận. Enter ở lịch hẹn để nhận ngay, hoặc nhập `YYYY-MM-DD HH:MM:SS` ở tương lai.
3. Guest nhập MSSV → chọn phòng → chọn đơn → lấy vé.
4. counter_ctsv gọi số, đối chiếu thẻ và xác nhận giao đủ giấy bằng mã vé đầy đủ.
5. Sinh viên xem COLLECTED; dashboard cập nhật trạng thái.

Với DOC003, dùng phòng Đào tạo, phí 10.000 VND/bản. Quầy phải xác nhận thu đủ tiền mặt trước khi hoàn tất. Thanh toán được lưu trên đơn, không thu lại khi vé hết hạn và cấp vé mới.

Một đơn chỉ có một vé đang chờ/phục vụ. Mỗi phòng tối đa một vé SERVING. Vắng mặt thì bỏ lượt và lấy vé mới. Qua ngày, vé cũ hết hạn; số mới từ 1 theo phòng, đơn chưa nhận vẫn giữ nguyên. Không giới hạn hạn mức phát vé: sau 999 tiếp tục 1000.

## Cấu trúc

```text
Prj2/
  .vscode/                   Cấu hình F5, build và CMD
  StudentDocumentSystem/
    main.cpp                 Khởi tạo Application
    enums/                   Vai trò, phòng, trạng thái
    models/                  Dữ liệu đối tượng và hàm nghiệp vụ
    templates/               MyList, MyQueue, MyStack tự cài
    repositories/            Đọc/ghi TXT
    services/                Quyền và nghiệp vụ
    views/                   Menu, nhập/xuất console
    utils/                   File, thời gian, khóa, mật khẩu
    data/                    Dữ liệu TXT thật
    StudentDocumentSystem.exe  File chạy được tạo khi bấm F5
```

View → Service → Repository → TXT. Model có thuộc tính private, getter và hàm nghiệp vụ. User có các lớp con Student/ProcessingStaff/CounterStaff. BaseView và IRequestRepository thể hiện trừu tượng; showMenu và phương thức User thể hiện đa hình. unique_ptr quản lý quyền sở hữu các đối tượng đa hình.

MyList tự cài node đơn và head/tail. MyQueue dùng MyList để thêm cuối/xóa đầu FIFO. MyStack dùng thêm đầu/xóa đầu để lưu lịch sử màn hình sinh viên. Có sao chép sâu, giải phóng node và kiểm tra truy cập sai. Không dùng container STL thay thế.

## Hai phần hỗ trợ

**Khóa:** DataLock gọi `_sopen_s` với `_SH_DENYRW` của CRT để mở độc quyền `data/system.lock`. Terminal khác chờ rồi thử lại. Descriptor được hệ điều hành đóng khi tiến trình kết thúc, kể cả thoát bất thường; không cần xóa file khóa. Không dùng Windows.h hoặc std::mutex để giả lập khóa giữa các tiến trình.

**Mật khẩu:** PasswordHelper gọi OpenSSL PBKDF2-HMAC-SHA256, 600.000 vòng; không tự viết thuật toán. OpenSSL UCRT64 đã có trên máy, link bằng `-lcrypto`. Giữ nguyên định dạng hash nên tài khoản cũ vẫn dùng được. Khi chuyển máy cần OpenSSL cùng bộ compiler; MSYS2 UCRT64 có gói `mingw-w64-ucrt-x86_64-openssl`. Các DLL runtime cần nằm trong PATH; cấu hình F5 đã thêm thư mục UCRT64.

## Lưu file và giới hạn

Mỗi dòng một bản ghi, ngăn bằng `|`, không có tiêu đề. Trường thời gian chưa có để rỗng. Tiền dùng long long; kiểm tra tràn trước khi nhân số lượng. Dữ liệu sai cấu trúc/khóa liên kết được báo và không âm thầm bỏ dòng rồi ghi đè.

Lưu: ghi đủ .tmp → đổi file gốc sang .bak → đặt file mới → dọn .bak, dưới khóa chung. Nếu bị ngắt khi thiếu file chính, lần đọc sau khôi phục .bak. Đây không phải thay file nguyên tử hay giao dịch giữa nhiều file. Khi giao giấy, lưu COLLECTED trước rồi lưu vé; lần tải hàng đợi sau sửa vé chưa đồng bộ theo đơn. Không cam kết chống mọi trường hợp mất điện/hỏng đĩa.

Thư mục kiểm tra, dữ liệu chạy thử và script build đã được dọn theo yêu cầu. Cấu hình trong `.vscode/` được giữ để phím F5 trên bàn phím biết cách biên dịch và mở console; `.build/` chỉ lưu kết quả biên dịch từng file để tăng tốc.
