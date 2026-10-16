# Kế hoạch PBL2 — Quản lý đăng ký và nhận giấy tờ sinh viên

Tài liệu này tổng hợp các quyết định đã thống nhất và thiết kế đề xuất để triển khai. Chưa phải mã nguồn. Tài liệu yêu cầu ban đầu được giữ nguyên.

## 1. Phạm vi đã thống nhất

- Một người thực hiện, C++ console trên Windows. Cập nhật theo lựa chọn mới: VS Code, g++ và F5 mở console ngoài/CMD; không cần solution/project Visual Studio.
- Một máy mở nhiều terminal. Mỗi terminal chạy một phiên riêng; các phiên dùng chung dữ liệu TXT. Không làm mạng hay client–server.
- Hai phòng: Công tác sinh viên (CTSV), Đào tạo (DAOTAO); mỗi phòng có một quầy và hàng đợi riêng.
- Sinh viên tự đăng ký bằng MSSV có sẵn trong danh sách, dùng MSSV làm tên đăng nhập. Mỗi MSSV chỉ có một tài khoản.
- Tài khoản nhân viên, phòng làm việc, danh sách sinh viên, danh mục giấy tờ và phí được chuẩn bị trong TXT. Chưa có giao diện quản trị.
- Nhân viên thuộc một phòng cố định và chỉ thao tác nghiệp vụ của phòng đó.
- Một đơn đăng ký một loại giấy, có số lượng bản và ghi chú. Một vé nhận một đơn.
- Chỉ lấy vé khi giấy sẵn sàng và thời điểm hiện tại bằng hoặc sau giờ hẹn.
- Không đặt hạn mức số vé phát trong ngày. Mỗi đơn chỉ có một vé WAITING hoặc SERVING còn hiệu lực.
- Số thứ tự reset mỗi ngày, độc lập theo phòng. Vé chỉ có hiệu lực trong ngày cấp. Qua ngày mới phải lấy vé mới; không tạo lại đơn.
- Sinh viên vắng mặt: nhân viên bỏ lượt; muốn nhận giấy thì lấy vé mới.
- Thu tiền mặt tại quầy trước khi giao giấy. Đơn miễn phí bỏ qua bước thu tiền. Không tích hợp thanh toán trực tuyến.
- Dashboard đọc lại trạng thái khoảng 5 giây một lần, chỉ hiện số đang phục vụ và các số chờ.
- Tự cài và sử dụng MyList, MyQueue, MyStack bằng template và danh sách liên kết.

## 2. Quy tắc nghiệp vụ

### Trạng thái đơn

```text
PENDING -> APPROVED -> PRINTED -> READY_FOR_COLLECTION -> COLLECTED
    +----> REJECTED
```

- Tạo đơn: kiểm tra sinh viên, loại giấy, số lượng nguyên dương; lưu đơn giá tại thời điểm đăng ký.
- Duyệt/từ chối: chỉ áp dụng với PENDING; từ chối phải có lý do.
- Đánh dấu in: chỉ áp dụng với APPROVED. In, ký và đóng dấu là thao tác ngoài phần mềm.
- Sẵn sàng nhận: chỉ áp dụng với PRINTED; phải có lịch hẹn hợp lệ.
- `readyAt` là lúc nhân viên xác nhận giấy sẵn sàng; `appointmentAt` là thời điểm sinh viên được phép đến lấy số. Đề xuất không đặt lịch hẹn trước lúc xác nhận sẵn sàng.
- Không cung cấp thao tác đổi trạng thái tùy ý trên giao diện.
- Sinh viên chỉ xem đơn của mình; nhân viên chỉ xử lý đơn thuộc phòng mình.

### Trạng thái vé

```text
WAITING -> SERVING -> COMPLETED
              +----> SKIPPED
WAITING / SERVING -> EXPIRED khi sang ngày mới
```

- Khi lấy số: tra cứu theo MSSV, chọn phòng, hiển thị các đơn đủ điều kiện và chọn mã đơn.
- Mỗi phòng tối đa một vé SERVING. Phải hoàn tất hoặc bỏ lượt trước khi gọi vé tiếp theo.
- Gọi theo FIFO. Không giữ khóa dữ liệu trong lúc nhân viên chờ hoặc kiểm tra thẻ.
- Xác minh là nhân viên kiểm tra thẻ sinh viên/CCCD bên ngoài và xác nhận qua giao diện; không lưu số CCCD.
- Đơn có phí chỉ được hoàn tất khi đã ghi nhận thanh toán.
- Khi hoàn tất phải đọc lại đơn, vé, phòng và ngày hiệu lực; không dùng dữ liệu cũ đang hiển thị làm căn cứ duy nhất.
- Vé hết hạn hoặc bị bỏ lượt không thay đổi trạng thái READY_FOR_COLLECTION của đơn.
- Kiểm tra ngày trước các thao tác hàng đợi, kể cả khi chương trình chạy xuyên ngày. Không cần tiến trình chạy đúng 0 giờ.
- Giữ lịch sử vé. Qua ngày không xóa file hàng đợi.

### Thu phí

- Tổng tiền = đơn giá đã lưu trong đơn × số lượng. Tiền dùng số nguyên `long long`.
- PaymentStatus: NOT_REQUIRED, UNPAID, PAID.
- Thu tiền và hoàn tất giao giấy là hai thao tác lưu riêng. PAID được lưu ngay khi nhân viên xác nhận đã thu.
- Nếu đã thanh toán nhưng chưa giao hoặc vé hết hạn, trạng thái PAID vẫn giữ trên đơn; vé mới không làm thu tiền lần nữa.
- Không tự động hoàn tiền hay hoàn tác giao giấy trong bản đầu.

## 3. Thiết kế lớp đề xuất

### Models

| Lớp | Trách nhiệm và dữ liệu chính |
|---|---|
| User | userId, username, thông tin kiểm tra mật khẩu, fullName, role; lớp cha có destructor virtual |
| Student : User | studentId, className, faculty; dữ liệu sinh viên được ghép từ students.txt |
| ProcessingStaff : User | department |
| CounterStaff : User | department |
| DocumentType | documentId, documentName, department, fee |
| Request | mã đơn, MSSV, mã giấy, số lượng, đơn giá, ghi chú, trạng thái, thời gian, lý do từ chối, thanh toán |
| Ticket | ticketId, ngày cấp, phòng, số thứ tự, requestId, studentId, trạng thái và thời gian |

`StudentRecord` có thể là struct đơn giản để đọc danh sách sinh viên chưa có tài khoản. Không cần tạo User giả cho các sinh viên chưa đăng ký.

### Services

| Lớp | Hàm nghiệp vụ dự kiến |
|---|---|
| AuthService | RegisterStudent, Login, Logout |
| RequestService | GetDocuments, CreateRequest, GetMyRequests |
| ProcessingService | GetDepartmentRequests, ApproveRequest, RejectRequest, MarkPrinted, SetReadyForCollection |
| QueueService | GetEligibleRequests, GenerateTicket, ExpireOldTickets, GetQueueSnapshot |
| CounterService | CallNextTicket, SkipCurrentTicket, RecordPayment, CompleteCollection |

Service nhận thông tin phiên đăng nhập để kiểm tra quyền, không chỉ dựa vào việc menu đã ẩn chức năng.

### Repositories và hỗ trợ

- UserRepository: đọc/tìm/thêm tài khoản.
- StudentRepository: tra cứu danh sách sinh viên có sẵn.
- DocumentRepository: đọc danh mục giấy tờ.
- RequestRepository: đọc và lưu đơn.
- QueueRepository: đọc và lưu lịch sử vé theo từng phòng.
- DataLock: dùng CRT mở độc quyền file system.lock trong data, phối hợp giữa các tiến trình; đóng descriptor khi ra khỏi phạm vi. Không dùng Windows.h.
- DateTimeHelper: lấy ngày giờ, kiểm tra ngày thực tế, so sánh thời gian.
- InputHelper: đọc số, chuỗi, kiểm tra dữ liệu đầu vào.
- PasswordHelper: dùng OpenSSL để tạo salt và băm PBKDF2-HMAC-SHA256; không tự viết thuật toán mật mã, không dùng Windows.h.

Không tạo interface cho mọi lớp. Đề xuất dùng `IRequestRepository` với các hàm đọc/lưu thuần ảo và `RequestRepository` triển khai để minh họa trừu tượng; chỉ mở rộng nếu tiêu chí giảng viên yêu cầu.

### Views và OOP

- BaseView có `virtual void showMenu() = 0` và destructor virtual.
- LoginView, StudentView, ProcessingView, CounterView, GuestView, DashboardView triển khai giao diện tương ứng.
- View nhận dữ liệu và trình bày kết quả; không mở file TXT trực tiếp.
- Service xử lý nghiệp vụ; Repository chỉ chuyển đổi dữ liệu và đọc/ghi.
- main tạo các đối tượng và điều hướng, không chứa toàn bộ nghiệp vụ.
- Đề xuất không đặt showMenu trong User như bản yêu cầu cũ, để giao diện nằm ở views. Cần đối chiếu rubric nếu giảng viên yêu cầu đa hình trên chính User.

## 4. Ba template tự cài

| Template | Các thao tác | Sử dụng |
|---|---|---|
| MyList<T> | Add, Remove, Find, GetByIndex, Size, Clear | Danh sách model và kết quả đọc file |
| MyQueue<T> | Enqueue, Dequeue, Front, Empty, Size, Clear | FIFO các ticketId đang WAITING của từng phòng |
| MyStack<T> | Push, Pop, Top, Empty, Size, Clear | Lịch sử ScreenId để quay lại màn hình trước |

- Header chứa cả khai báo và định nghĩa template.
- Không dùng vector/list/queue/stack có sẵn thay thế ba cấu trúc trên. Vẫn dùng string, fstream, stringstream và thư viện hỗ trợ cơ bản.
- Quản lý node bằng constructor/destructor; kiểm tra cấu trúc rỗng và chỉ số không hợp lệ.
- Nếu cho phép sao chép, triển khai copy constructor và copy assignment sao chép sâu. Tránh sao chép không cần thiết bằng tham chiếu.
- Không dùng con trỏ tới node hoặc dữ liệu đã bị giải phóng. Quy định rõ chủ sở hữu các User được tạo đa hình.
- Queue được dựng lại từ dữ liệu vé mới nhất khi xử lý; không coi queue trong bộ nhớ một terminal là dữ liệu chung luôn đúng.
- Stack phục vụ điều hướng thật; không thêm undo giao giấy chỉ để dùng stack.

## 5. Định dạng TXT đề xuất

Mỗi dòng một bản ghi, phân tách bằng `|`, không có dòng tiêu đề. Dòng dưới đây mô tả thứ tự trường, không phải dữ liệu mẫu để nạp trực tiếp.

```text
users.txt
userId|username|passwordSalt|passwordHash|fullName|role|department

students.txt
studentId|fullName|className|faculty

documents.txt
documentId|documentName|department|fee

requests.txt
requestId|studentId|documentId|quantity|unitFee|note|status|requestedAt|approvedAt|printedAt|readyAt|appointmentAt|collectedAt|rejectReason|paymentStatus|paidAt

queue_ctsv.txt và queue_daotao.txt
ticketId|ticketDate|department|sequenceNumber|studentId|requestId|status|issuedAt|calledAt|finishedAt
```

- Với STUDENT: username chính là studentId; department dùng NONE. Staff dùng CTSV hoặc DAOTAO.
- Ngày: YYYY-MM-DD; thời gian: YYYY-MM-DD HH:MM:SS. Trường thời gian chưa có được để rỗng, parser phải giữ đúng cả trường rỗng cuối dòng.
- MSSV lưu string. Không nhận `|`, CR hoặc LF trong trường người dùng nhập.
- Nội dung TXT dùng UTF-8. Dùng fstream và đường dẫn tương đối từ thư mục dự án; không dùng <filesystem>. Nhập/xuất console bằng cin/cout.
- Kiểm tra số trường, số hợp lệ, enum hợp lệ và khóa liên kết. Dữ liệu lỗi phải được báo; không âm thầm bỏ dòng rồi ghi đè làm mất bản ghi.
- Mã đơn và mã tài khoản: lấy giá trị lớn nhất đang lưu cộng 1 dưới khóa; bản đầu không xóa lịch sử và không dùng số lượng dòng cộng 1.
- Mã vé ví dụ `20261021-CTSV-003`; hiển thị `CTSV-003`. Sau 999 tiếp tục 1000, không cắt số.
- Số vé mới lấy max sequenceNumber theo ngày và phòng cộng 1, bao gồm cả vé đã hoàn tất/bỏ lượt.
- Hai file queue lưu cả lịch sử, không chỉ vé đang chờ. SERVING được lưu xuống file nên mở lại vẫn khôi phục được.

Danh mục ít thay đổi, dữ liệu nhỏ nên đọc/ghi toàn bộ file là chấp nhận được cho đồ án. Chưa cần tối ưu chỉ mục hay lưu trữ phức tạp.

## 6. Nhiều terminal và tính nhất quán

Mọi terminal phải trỏ tới cùng một thư mục data được xác định rõ. Không để mỗi thư mục chạy Debug/Release vô tình có bộ dữ liệu riêng.

```text
Nhập thông tin ngoài khóa
-> Lấy khóa dùng chung
-> Đọc dữ liệu mới nhất
-> Kiểm tra quyền, ngày và trạng thái
-> Cập nhật, lưu và kiểm tra kết quả
-> Nhả khóa
-> Hiển thị kết quả
```

- Dùng một file khóa CRT chung theo thư mục data. std::mutex thông thường không đồng bộ được các tiến trình độc lập.
- Không giữ khóa khi chờ nhập liệu, chờ sinh viên hoặc đợi 5 giây refresh.
- Dashboard lấy snapshot dưới khóa, nhả khóa rồi mới hiển thị và đợi lần kế tiếp.
- Không sửa TXT thủ công khi chương trình đang chạy.
- Ghi đủ .tmp; đổi file gốc sang .bak, đặt file mới rồi dọn .bak dưới khóa. Nếu bị ngắt khi thiếu file chính, lần đọc sau khôi phục .bak. Không xóa file gốc trước khi biết bản mới đã ghi đủ.
- Cập nhật hai file không phải giao dịch nguyên tử. Khi hoàn tất giao giấy, lưu đơn COLLECTED trước rồi lưu vé COMPLETED. Nếu bước sau lỗi, báo trạng thái cần đồng bộ; lần đọc sau đối chiếu đơn COLLECTED để đóng vé còn hoạt động.
- Bước đối chiếu thực hiện khi khởi động và trước thao tác hàng đợi; descriptor file khóa được hệ điều hành đóng khi tiến trình kết thúc.
- Chưa cam kết chịu mọi tình huống mất điện hoặc hỏng đĩa. Giải pháp này tập trung vào demo nhiều terminal và phục hồi sai lệch đơn/vé có thể nhận biết.

## 7. Cấu trúc thư mục dự kiến

```text
StudentDocumentSystem/
  main.cpp
  models/
  enums/
  templates/
  services/
  repositories/
  views/
  utils/
  data/
```

Các lớp thường tách .h/.cpp; template đặt trong .h. Các file kiểm tra và dữ liệu chạy thử đã được dọn theo yêu cầu sau khi xác minh.

Cấu hình cho phím F5 trên bàn phím nằm trong `.vscode/` tại thư mục Prj2: tasks.json gọi make với build.mk để chỉ biên dịch phần thay đổi bằng g++, launch.json chạy gdb với externalConsole, settings.json chọn CMD. Bộ nhớ đệm compiler nằm trong .build/. Không dùng script build, .sln hoặc .vcxproj. Phần mật khẩu dùng OpenSSL có sẵn trong MSYS2 UCRT64; không tự cài thuật toán băm.

## 8. Thứ tự triển khai và tiêu chí hoàn thành

| Mốc | Nội dung | Hoàn thành khi |
|---|---|---|
| 1 | Enum, model, định dạng dữ liệu và sơ đồ lớp | Theo được liên kết tài khoản -> sinh viên -> đơn -> vé |
| 2 | MyList, MyQueue, MyStack | Thêm/xóa, rỗng, FIFO/LIFO, sao chép và hủy đúng |
| 3 | Repository, mã định danh và DataLock | Hai terminal cập nhật thử không trùng mã, không mất bản ghi |
| 4 | Đăng ký, đăng nhập và quyền | Chặn đăng ký trùng MSSV; mỗi phiên độc lập; không vượt quyền |
| 5 | Lập, duyệt, in và hẹn nhận đơn | Đúng chuỗi trạng thái, tắt mở vẫn giữ dữ liệu |
| 6 | Cấp vé, gọi số, bỏ lượt, thu tiền, giao giấy | Hoàn thành một đơn từ đầu đến cuối |
| 7 | Dashboard, điều hướng stack, giao diện | Hai phòng hiển thị đúng và tự cập nhật |
| 8 | Kiểm thử tích hợp, UML, báo cáo và demo | Các tình huống chính đạt, giải thích được từng lớp và thuật toán |

Chưa gán lịch theo tuần vì chưa có hạn nộp. Không mở rộng chức năng trước khi hoàn thành mốc 6.

## 9. Kịch bản kiểm tra cần có

1. Sinh viên hợp lệ đăng ký, đăng nhập; MSSV không tồn tại hoặc đã có tài khoản bị từ chối.
2. Hai terminal cùng đăng ký một MSSV: chỉ tạo một tài khoản.
3. Sinh viên không xem đơn người khác; staff không thao tác đơn/vé phòng khác.
4. Số lượng âm, bằng 0, nhập chữ hoặc vượt giới hạn kiểu số đều được xử lý; tính phí không tràn số.
5. Duyệt/in/hẹn nhận sai trạng thái bị chặn; từ chối thiếu lý do bị chặn.
6. Lấy số trước giờ hẹn bị chặn; đúng giờ và sau giờ được phép.
7. Hai terminal cùng cấp vé cho một đơn: chỉ một vé hoạt động.
8. Hai terminal cùng gọi số của một phòng: chỉ một vé SERVING.
9. Bỏ lượt không thay đổi đơn; có thể lấy số mới.
10. Sang ngày mới trong lúc chương trình vẫn chạy: vé cũ hết hạn, số mới từ 1; đơn và thanh toán được giữ.
11. Đơn có phí chưa trả không được hoàn tất; miễn phí được hoàn tất sau xác minh.
12. Đã thu tiền nhưng chưa giao, đóng rồi mở lại: không thu lần hai.
13. Lỗi cập nhật vé sau khi lưu đơn COLLECTED: lần đọc sau đồng bộ vé, không giao lại.
14. Dashboard hiển thị đúng số đang phục vụ và thứ tự chờ của cả hai phòng.
15. File thiếu, sai định dạng hoặc không ghi được: báo lỗi rõ, không báo thành công giả và không ghi đè làm mất dữ liệu.

## 10. Nội dung báo cáo và demo

- Giới thiệu bài toán, phạm vi và vai trò.
- Use case; sơ đồ lớp; hoạt động đăng ký/nhận giấy; sequence cho lập đơn và gọi số.
- Giải thích 3 lớp, bốn tính chất OOP, ba template và quản lý bộ nhớ.
- Giải thích TXT, khóa giữa các tiến trình và giới hạn phục hồi.
- Demo một đơn miễn phí, một đơn có phí, một đơn bị từ chối, hai hàng đợi, một vé vắng mặt và một vé hết hạn.
- Chuẩn bị dữ liệu demo riêng; kiểm tra đổi ngày bằng đồng hồ giả lập trong bài kiểm tra, không cần sửa giờ hệ điều hành.

## 11. Ngoài phạm vi bản đầu

- Kết nối nhiều máy, web, GUI, cơ sở dữ liệu.
- Thanh toán online, hoàn tiền, hóa đơn tài chính.
- Quản trị tài khoản/danh mục, quên mật khẩu, gửi email/SMS.
- Nhận nhiều đơn bằng một vé, ưu tiên chen hàng, nhiều quầy trong cùng phòng.
- Hoàn tác giao giấy hoặc tự động sinh/in nội dung giấy tờ.

Các chi tiết kỹ thuật ở phần thiết kế là đề xuất triển khai, có thể đơn giản hóa nếu không ảnh hưởng yêu cầu đã chốt. Hai thông tin còn hữu ích để điều chỉnh kế hoạch là hạn nộp và rubric chấm OOP/template của giảng viên; không cản trở việc bắt đầu thiết kế nền tảng.
