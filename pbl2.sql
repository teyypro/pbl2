CREATE TABLE `users` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `username` varchar(50) UNIQUE NOT NULL,
  `password_hash` varchar(255) NOT NULL,
  `full_name` varchar(100) NOT NULL,
  `email` varchar(100) UNIQUE,
  `phone` varchar(20),
  `role` enum(admin,receptionist,doctor,patient) NOT NULL,
  `status` enum(active,locked,pending) DEFAULT 'pending',
  `last_login` timestamp,
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `doctors` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `user_id` integer UNIQUE NOT NULL,
  `department_id` integer NOT NULL,
  `specialty` varchar(100),
  `dob` date,
  `salary` decimal(15,2),
  `years_experience` integer,
  `qualification` varchar(200),
  `accept_appointments` boolean DEFAULT true,
  `accept_walkin` boolean DEFAULT true,
  `max_appointments_per_day` integer DEFAULT 10,
  `max_walkin_per_day` integer DEFAULT 15,
  `can_respond_feedback` boolean DEFAULT false,
  `status` enum(active,inactive,on_leave) DEFAULT 'active',
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `patients` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `user_id` integer UNIQUE NOT NULL,
  `patient_code` varchar(20) UNIQUE NOT NULL,
  `full_name` varchar(100) NOT NULL,
  `dob` date NOT NULL,
  `gender` enum(male,female,other),
  `phone` varchar(20),
  `email` varchar(100),
  `address` text,
  `bhyt_code` varchar(20),
  `bhyt_expiry` date,
  `medical_history` text,
  `allergies` text,
  `blood_type` varchar(5),
  `emergency_contact` varchar(100),
  `emergency_phone` varchar(20),
  `status` enum(active,inactive) DEFAULT 'active',
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `departments` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `code` varchar(20) UNIQUE NOT NULL,
  `name` varchar(100) NOT NULL,
  `description` text,
  `floor_location` varchar(50),
  `head_doctor_id` integer,
  `status` enum(active,inactive) DEFAULT 'active',
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `services` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `code` varchar(20) UNIQUE NOT NULL,
  `name` varchar(200) NOT NULL,
  `description` text,
  `price` decimal(15,2) NOT NULL,
  `unit` varchar(20),
  `category` varchar(50),
  `department_id` integer NOT NULL,
  `status` enum(active,inactive) DEFAULT 'active',
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `medicines` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `code` varchar(20) UNIQUE NOT NULL,
  `name` varchar(200) NOT NULL,
  `active_ingredient` varchar(200),
  `unit` varchar(20) NOT NULL,
  `price` decimal(15,2) NOT NULL,
  `stock_quantity` integer DEFAULT 0,
  `min_stock` integer DEFAULT 10,
  `expiry_date` date,
  `batch_number` varchar(50),
  `manufacturer` varchar(100),
  `requires_prescription` boolean DEFAULT true,
  `status` enum(available,out_of_stock,discontinued) DEFAULT 'available',
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `doctor_services` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `doctor_id` integer NOT NULL,
  `service_id` integer NOT NULL,
  `is_active` boolean DEFAULT true,
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `doctor_appointment_schedules` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `doctor_id` integer NOT NULL,
  `weekday` integer NOT NULL,
  `start_time` time NOT NULL,
  `end_time` time NOT NULL,
  `slot_duration_minutes` integer DEFAULT 20,
  `max_patients` integer,
  `is_active` boolean DEFAULT true,
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `appointment_slots` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `doctor_id` integer NOT NULL,
  `slot_date` date NOT NULL,
  `slot_time` time NOT NULL,
  `slot_duration_minutes` integer DEFAULT 20,
  `appointment_id` integer,
  `patient_id` integer,
  `status` enum(available,booked,completed,missed,cancelled) DEFAULT 'available',
  `note` text,
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `appointments` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `appointment_code` varchar(30) UNIQUE NOT NULL,
  `patient_id` integer NOT NULL,
  `doctor_id` integer NOT NULL,
  `department_id` integer NOT NULL,
  `service_id` integer NOT NULL,
  `slot_id` integer UNIQUE NOT NULL,
  `scheduled_at` timestamp NOT NULL,
  `status` enum(pending,confirmed,booked,completed,cancelled,missed) DEFAULT 'pending',
  `cancel_reason` text,
  `cancelled_by` integer,
  `created_by` integer NOT NULL,
  `note` text,
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `walkin_queues` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `ticket_code` varchar(30) UNIQUE NOT NULL,
  `patient_id` integer NOT NULL,
  `department_id` integer NOT NULL,
  `service_id` integer NOT NULL,
  `doctor_id` integer,
  `priority` enum(normal,priority,emergency) DEFAULT 'normal',
  `status` enum(waiting,assigned,examining,completed,missed,cancelled) DEFAULT 'waiting',
  `queue_number` integer,
  `check_in_time` timestamp DEFAULT (now()),
  `assigned_at` timestamp,
  `assigned_doctor_id` integer,
  `called_at` timestamp,
  `examination_started_at` timestamp,
  `examination_completed_at` timestamp,
  `estimated_wait_minutes` integer,
  `created_by` integer,
  `note` text,
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `walkin_doctor_daily_limits` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `doctor_id` integer NOT NULL,
  `work_date` date NOT NULL,
  `max_walkin` integer NOT NULL,
  `current_count` integer DEFAULT 0,
  `is_available` boolean DEFAULT true,
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `medical_records` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `record_code` varchar(30) UNIQUE NOT NULL,
  `patient_id` integer NOT NULL,
  `doctor_id` integer NOT NULL,
  `source_type` enum(appointment,walkin) NOT NULL,
  `source_id` integer NOT NULL,
  `visit_date` timestamp DEFAULT (now()),
  `symptoms` text NOT NULL,
  `clinical_findings` text,
  `diagnosis` text NOT NULL,
  `diagnosis_code` varchar(20),
  `advice` text,
  `follow_up_date` date,
  `height` decimal(5,2),
  `weight` decimal(5,2),
  `blood_pressure` varchar(20),
  `temperature` decimal(4,1),
  `status` enum(draft,finalized,completed) DEFAULT 'draft',
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `prescriptions` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `prescription_code` varchar(30) UNIQUE NOT NULL,
  `medical_record_id` integer NOT NULL,
  `patient_id` integer NOT NULL,
  `doctor_id` integer NOT NULL,
  `prescribed_at` timestamp DEFAULT (now()),
  `note` text,
  `status` enum(pending,dispensed,completed,cancelled) DEFAULT 'pending',
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `prescription_details` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `prescription_id` integer NOT NULL,
  `medicine_id` integer NOT NULL,
  `dosage` varchar(50) NOT NULL,
  `quantity` integer NOT NULL,
  `days` integer NOT NULL,
  `frequency` varchar(50) NOT NULL,
  `route` enum(oral,injection,topical,inhaled,other) DEFAULT 'oral',
  `morning` boolean DEFAULT false,
  `noon` boolean DEFAULT false,
  `evening` boolean DEFAULT false,
  `night` boolean DEFAULT false,
  `note` text,
  `status` enum(active,dispensed,cancelled) DEFAULT 'active',
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `examination_services` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `medical_record_id` integer NOT NULL,
  `service_id` integer NOT NULL,
  `doctor_id` integer NOT NULL,
  `quantity` integer DEFAULT 1,
  `unit_price` decimal(15,2) NOT NULL,
  `discount` decimal(15,2) DEFAULT 0,
  `discount_type` enum(percent,fixed) DEFAULT 'percent',
  `status` enum(pending,performed,cancelled) DEFAULT 'pending',
  `performed_by` integer,
  `performed_at` timestamp,
  `note` text,
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `invoices` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `invoice_code` varchar(30) UNIQUE NOT NULL,
  `patient_id` integer NOT NULL,
  `medical_record_id` integer,
  `prescription_id` integer,
  `total_amount` decimal(15,2) NOT NULL,
  `bhyt_discount` decimal(15,2) DEFAULT 0,
  `other_discount` decimal(15,2) DEFAULT 0,
  `patient_pay` decimal(15,2) NOT NULL,
  `paid_amount` decimal(15,2) DEFAULT 0,
  `payment_method` enum(cash,card,bank_transfer,insurance,other),
  `payment_status` enum(pending,paid,partial,refunded,cancelled) DEFAULT 'pending',
  `invoice_date` timestamp DEFAULT (now()),
  `paid_at` timestamp,
  `created_by` integer NOT NULL,
  `note` text,
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `invoice_items` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `invoice_id` integer NOT NULL,
  `item_type` enum(service,medicine) NOT NULL,
  `item_id` integer NOT NULL,
  `item_name` varchar(200) NOT NULL,
  `quantity` integer NOT NULL,
  `unit_price` decimal(15,2) NOT NULL,
  `discount` decimal(15,2) DEFAULT 0,
  `amount` decimal(15,2) NOT NULL,
  `note` text,
  `created_at` timestamp DEFAULT (now())
);

CREATE TABLE `feedbacks` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `patient_id` integer NOT NULL,
  `doctor_id` integer NOT NULL,
  `medical_record_id` integer NOT NULL,
  `rating` integer NOT NULL,
  `comment` text,
  `response` text,
  `response_by` integer,
  `responded_at` timestamp,
  `status` enum(pending,responded,hidden) DEFAULT 'pending',
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `system_configs` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `config_key` varchar(50) UNIQUE NOT NULL,
  `config_value` text NOT NULL,
  `data_type` enum(string,number,boolean,json) DEFAULT 'string',
  `description` text,
  `is_editable` boolean DEFAULT true,
  `created_at` timestamp DEFAULT (now()),
  `updated_at` timestamp DEFAULT (now())
);

CREATE TABLE `activity_logs` (
  `id` integer PRIMARY KEY AUTO_INCREMENT,
  `user_id` integer,
  `action` varchar(100) NOT NULL,
  `table_name` varchar(50),
  `record_id` integer,
  `old_data` json,
  `new_data` json,
  `ip` varchar(45),
  `user_agent` text,
  `created_at` timestamp DEFAULT (now())
);

ALTER TABLE `users` COMMENT = '- Khi status = locked → tự động set doctor/patient tương ứng thành inactive (nếu có).
- role = patient bắt buộc đi kèm với bản ghi patients.
';

ALTER TABLE `doctors` COMMENT = '- accept_appointments / accept_walkin dùng để lọc khi patient/receptionist đặt lịch hoặc lấy số.
- max_* chỉ là giá trị mặc định; giới hạn thực tế theo ngày nằm ở walkin_doctor_daily_limits và appointment_slots.
- can_respond_feedback: cờ do admin bật/tắt, quyết định doctor có được tự trả lời đánh giá của bệnh nhân hay không (nếu false, chỉ admin được phản hồi).
';

ALTER TABLE `patients` COMMENT = '- user_id bắt buộc (không còn guest).
- Khi tạo patient phải đồng thời tạo user với role = patient.
';

ALTER TABLE `departments` COMMENT = '- head_doctor_id phải là bác sĩ có doctors.department_id = departments.id của chính khoa này.
  Ràng buộc này không thể enforce bằng FK/CHECK thông thường (tham chiếu chéo 2 bảng) → validate ở application layer khi gán/đổi trưởng khoa.
';

ALTER TABLE `doctor_services` COMMENT = '1 bác sĩ - 1 dịch vụ chỉ được có 1 bản ghi (unique index).';

ALTER TABLE `appointment_slots` COMMENT = '- 1 bác sĩ không được có 2 slot trùng (doctor_id + slot_date + slot_time).
- Khi book: status = booked, set appointment_id + patient_id.
- Khi cancel/missed: status tương ứng + clear appointment_id + patient_id.
- Khi completed: status = completed.
';

ALTER TABLE `appointments` COMMENT = '- slot_id unique → 1 slot chỉ thuộc 1 appointment.
- Khi đổi status phải đồng bộ ngược lại appointment_slots.
';

ALTER TABLE `walkin_queues` COMMENT = '- doctor_id: bác sĩ ưu tiên lúc tạo.
- assigned_doctor_id: bác sĩ thực sự khám (được gán bởi receptionist hoặc hệ thống).
- 1 bệnh nhân không được có 2 vé đang ở trạng thái waiting/assigned/examining cùng 1 khoa.
';

ALTER TABLE `walkin_doctor_daily_limits` COMMENT = '- 1 bác sĩ chỉ có 1 bản ghi limit cho 1 ngày.
- Khi tạo vé thành công → tăng current_count.
- Khi cancel/missed trước examining → giảm current_count.
- current_count >= max_walkin → is_available = false.
- Rủi ro race condition: 2 request tạo vé cùng lúc có thể cùng đọc current_count < max_walkin trước khi commit
  → cần đọc + tăng current_count trong 1 transaction có khóa dòng (SELECT ... FOR UPDATE) hoặc UPDATE ... WHERE current_count < max_walkin
  để đảm bảo không vượt hạn mức khi có nhiều receptionist/thao tác đồng thời.
';

ALTER TABLE `medical_records` COMMENT = '- Unique (source_type, source_id) → 1 nguồn chỉ có 1 phiếu khám.
- Chỉ được tạo khi source đang ở examining hoặc completed.
- Sau finalized chỉ admin được sửa.
- source_id là khóa đa hình (trỏ tới appointments.id nếu source_type=''appointment'', hoặc walkin_queues.id nếu source_type=''walkin'').
  Không thể khai báo FK thật cho cột này (bảng đích thay đổi theo source_type) → phải validate tính hợp lệ ở application layer,
  hoặc cân nhắc tách 2 cột nullable (appointment_id, walkin_id) + CHECK đúng 1 cột khác NULL nếu muốn ràng buộc ở tầng DB.
';

ALTER TABLE `invoices` COMMENT = '- Nên tạo từ examination + prescription đã completed.
- patient_pay = total_amount - bhyt_discount - other_discount.
- partial: paid_amount < patient_pay.
';

ALTER TABLE `invoice_items` COMMENT = '- item_id là khóa đa hình (trỏ tới services.id nếu item_type=''service'', hoặc medicines.id nếu item_type=''medicine'').
  Không enforce được bằng FK thật → validate item_id tồn tại đúng bảng tương ứng ở application layer khi tạo hóa đơn.
- item_name/unit_price được sao chép (snapshot) tại thời điểm lập hóa đơn, không tự đồng bộ nếu services/medicines đổi giá sau đó — đây là chủ đích để giữ lịch sử hóa đơn chính xác.
';

ALTER TABLE `feedbacks` COMMENT = '1 examination chỉ được 1 feedback.';

ALTER TABLE `doctors` ADD FOREIGN KEY (`user_id`) REFERENCES `users` (`id`);

ALTER TABLE `doctors` ADD FOREIGN KEY (`department_id`) REFERENCES `departments` (`id`);

ALTER TABLE `departments` ADD FOREIGN KEY (`head_doctor_id`) REFERENCES `doctors` (`id`);

ALTER TABLE `services` ADD FOREIGN KEY (`department_id`) REFERENCES `departments` (`id`);

ALTER TABLE `doctor_services` ADD FOREIGN KEY (`doctor_id`) REFERENCES `doctors` (`id`);

ALTER TABLE `doctor_services` ADD FOREIGN KEY (`service_id`) REFERENCES `services` (`id`);

ALTER TABLE `doctor_appointment_schedules` ADD FOREIGN KEY (`doctor_id`) REFERENCES `doctors` (`id`);

ALTER TABLE `appointment_slots` ADD FOREIGN KEY (`doctor_id`) REFERENCES `doctors` (`id`);

ALTER TABLE `appointment_slots` ADD FOREIGN KEY (`patient_id`) REFERENCES `patients` (`id`);

ALTER TABLE `appointment_slots` ADD FOREIGN KEY (`appointment_id`) REFERENCES `appointments` (`id`);

ALTER TABLE `appointments` ADD FOREIGN KEY (`patient_id`) REFERENCES `patients` (`id`);

ALTER TABLE `appointments` ADD FOREIGN KEY (`doctor_id`) REFERENCES `doctors` (`id`);

ALTER TABLE `appointments` ADD FOREIGN KEY (`department_id`) REFERENCES `departments` (`id`);

ALTER TABLE `appointments` ADD FOREIGN KEY (`service_id`) REFERENCES `services` (`id`);

ALTER TABLE `appointments` ADD FOREIGN KEY (`slot_id`) REFERENCES `appointment_slots` (`id`);

ALTER TABLE `appointments` ADD FOREIGN KEY (`created_by`) REFERENCES `users` (`id`);

ALTER TABLE `appointments` ADD FOREIGN KEY (`cancelled_by`) REFERENCES `users` (`id`);

ALTER TABLE `walkin_queues` ADD FOREIGN KEY (`patient_id`) REFERENCES `patients` (`id`);

ALTER TABLE `walkin_queues` ADD FOREIGN KEY (`department_id`) REFERENCES `departments` (`id`);

ALTER TABLE `walkin_queues` ADD FOREIGN KEY (`service_id`) REFERENCES `services` (`id`);

ALTER TABLE `walkin_queues` ADD FOREIGN KEY (`doctor_id`) REFERENCES `doctors` (`id`);

ALTER TABLE `walkin_queues` ADD FOREIGN KEY (`assigned_doctor_id`) REFERENCES `doctors` (`id`);

ALTER TABLE `walkin_queues` ADD FOREIGN KEY (`created_by`) REFERENCES `users` (`id`);

ALTER TABLE `walkin_doctor_daily_limits` ADD FOREIGN KEY (`doctor_id`) REFERENCES `doctors` (`id`);

ALTER TABLE `medical_records` ADD FOREIGN KEY (`patient_id`) REFERENCES `patients` (`id`);

ALTER TABLE `medical_records` ADD FOREIGN KEY (`doctor_id`) REFERENCES `doctors` (`id`);

ALTER TABLE `prescriptions` ADD FOREIGN KEY (`medical_record_id`) REFERENCES `medical_records` (`id`);

ALTER TABLE `prescriptions` ADD FOREIGN KEY (`patient_id`) REFERENCES `patients` (`id`);

ALTER TABLE `prescriptions` ADD FOREIGN KEY (`doctor_id`) REFERENCES `doctors` (`id`);

ALTER TABLE `prescription_details` ADD FOREIGN KEY (`prescription_id`) REFERENCES `prescriptions` (`id`);

ALTER TABLE `prescription_details` ADD FOREIGN KEY (`medicine_id`) REFERENCES `medicines` (`id`);

ALTER TABLE `examination_services` ADD FOREIGN KEY (`medical_record_id`) REFERENCES `medical_records` (`id`);

ALTER TABLE `examination_services` ADD FOREIGN KEY (`service_id`) REFERENCES `services` (`id`);

ALTER TABLE `examination_services` ADD FOREIGN KEY (`doctor_id`) REFERENCES `doctors` (`id`);

ALTER TABLE `examination_services` ADD FOREIGN KEY (`performed_by`) REFERENCES `doctors` (`id`);

ALTER TABLE `invoices` ADD FOREIGN KEY (`patient_id`) REFERENCES `patients` (`id`);

ALTER TABLE `invoices` ADD FOREIGN KEY (`medical_record_id`) REFERENCES `medical_records` (`id`);

ALTER TABLE `invoices` ADD FOREIGN KEY (`prescription_id`) REFERENCES `prescriptions` (`id`);

ALTER TABLE `invoices` ADD FOREIGN KEY (`created_by`) REFERENCES `users` (`id`);

ALTER TABLE `invoice_items` ADD FOREIGN KEY (`invoice_id`) REFERENCES `invoices` (`id`);

ALTER TABLE `feedbacks` ADD FOREIGN KEY (`patient_id`) REFERENCES `patients` (`id`);

ALTER TABLE `feedbacks` ADD FOREIGN KEY (`doctor_id`) REFERENCES `doctors` (`id`);

ALTER TABLE `feedbacks` ADD FOREIGN KEY (`medical_record_id`) REFERENCES `medical_records` (`id`);

ALTER TABLE `feedbacks` ADD FOREIGN KEY (`response_by`) REFERENCES `users` (`id`);

ALTER TABLE `activity_logs` ADD FOREIGN KEY (`user_id`) REFERENCES `users` (`id`);
