# Ghi chú triển khai (Implementation Notes)

Đây là các ràng buộc/logic **không thể enforce ở tầng database** (dù đã ghi chú trong DBML) —
bắt buộc phải code đúng ở application layer, nếu không sẽ gây bug hoặc dữ liệu không nhất quán
dù schema đúng.

## 1. Validate khóa đa hình (polymorphic reference)

Áp dụng cho:
- `medical_records.source_id` (tùy `source_type` = `appointment` hoặc `walkin`)
- `invoice_items.item_id` (tùy `item_type` = `service` hoặc `medicine`)

DB không có FK thật cho các cột này nên **sẽ không tự chặn** nếu insert sai. Trước khi insert/update,
code phải tự kiểm tra `source_id`/`item_id` tồn tại thật trong bảng tương ứng với giá trị
`source_type`/`item_type` đã chọn.

## 2. Tránh race condition khi tạo vé walk-in

`walkin_doctor_daily_limits.current_count` có thể bị vượt `max_walkin` nếu 2 request tạo vé cùng lúc
đều đọc được `current_count < max_walkin` trước khi commit.

**Không làm:** SELECT current_count → so sánh ở code → UPDATE riêng.

**Nên làm:** update có điều kiện ngay trong 1 câu lệnh, trong transaction:

```sql
UPDATE walkin_doctor_daily_limits
SET current_count = current_count + 1
WHERE doctor_id = ? AND work_date = ? AND current_count < max_walkin;
```

Sau đó kiểm tra `rowCount` (số dòng bị ảnh hưởng):
- `rowCount = 1` → tăng thành công, tiếp tục tạo vé.
- `rowCount = 0` → đã đầy hạn mức, từ chối tạo vé.

## 3. Kiểm tra `doctors.can_respond_feedback` ở tầng API

Khi doctor gọi API trả lời feedback, phải check field `can_respond_feedback = true` **ở service/API
layer**, không chỉ ẩn nút bấm ở UI. Ẩn UI không ngăn được người dùng gọi thẳng endpoint.

## 4. Validate `departments.head_doctor_id` khớp khoa

Khi admin gán/đổi trưởng khoa, code phải:
1. Query `doctors.department_id` của bác sĩ được chọn.
2. So sánh với `departments.id` của khoa đang gán.
3. Chỉ cho lưu nếu khớp nhau.

Ràng buộc chéo bảng này không thể khai báo bằng FK/CHECK thông thường.

## 5. Thứ tự tạo dữ liệu (transaction flow)

- **Đặt lịch hẹn:** slot phải được sinh sẵn trước (từ `doctor_appointment_schedules`, trạng thái
  `available`), rồi mới cho đặt lịch bằng cách chọn 1 slot có sẵn. Không tạo `appointments` trước rồi
  mới tạo slot sau — thứ tự ngược sẽ vướng ràng buộc `appointments.slot_id [unique, not null]`.
- **Đăng ký bệnh nhân:** tạo bản ghi `users` (role = `patient`) và `patients` phải nằm **trong cùng
  một transaction**, để tránh trường hợp có `user` với role patient nhưng không có bản ghi `patients`
  tương ứng (hoặc ngược lại) nếu một trong hai bước thất bại giữa chừng.
