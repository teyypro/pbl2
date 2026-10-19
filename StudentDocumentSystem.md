# Hệ thống Quản lý Đăng ký và Nhận Giấy tờ Sinh viên

**C++ | OOP | 3-Layer Architecture | Generic Template tự xây | File TXT Database**

## 1. Cách build và chạy

Yêu cầu: g++ hỗ trợ C++11 trở lên (khuyến nghị C++17). Không dùng thư viện ngoài, chỉ dùng thư viện chuẩn (`string`, `fstream`, `ctime`...). Các cấu trúc dữ liệu `MyList`, `MyQueue`, `Repository` đều tự cài đặt.

```bash
mkdir data            # chứa các file TXT (xem mục 3)
g++ -std=c++17 -o app main.cpp
./app                 # Windows: app.exe
```

Chạy từ thư mục gốc của project (nơi có thư mục `data/`).

**Mô phỏng nhiều quầy + dashboard:** mở nhiều cửa sổ terminal, mỗi cửa sổ chạy `./app` riêng (ví dụ: 1 cửa sổ cho dashboard, 1 cho Q01, 1 cho Q02). Mọi thao tác đều `load()` lại từ file TXT trước khi xử lý nên các tiến trình đồng bộ qua file.

## 2. Cấu trúc thư mục

```
Project/
├── main.cpp
├── utils/
│   └── Utils.h                  (split, ngày tháng, nhập liệu, waitKey)
├── templates/
│   ├── MyList.h
│   ├── MyQueue.h
│   └── Repository.h
├── models/
│   └── Models.h                 (Nguoi, TaiKhoan, SinhVien, Phong, Quay, VanBan, DangKy, PhieuHangDoi)
├── data/                        (Data Layer)
│   ├── FileManager.h
│   ├── TxtRepository.h
│   ├── Repositories.h
│   └── *.txt                    (file dữ liệu)
├── business/                    (Business Layer)
│   ├── AuthService.h
│   ├── DocumentService.h
│   ├── RegistrationService.h
│   ├── DashboardService.h
│   ├── QueueService.h
│   └── AppContext.h
└── presentation/                (Presentation Layer)
    ├── User.h                   (lớp trừu tượng User + hàm in hồ sơ)
    ├── StudentUI.h
    ├── DepartmentUI.h
    ├── CounterUI.h
    ├── DashboardUI.h
    └── MainUI.h
```

> Lưu ý: `models/Models.h` gom 8 lớp model vào một file cho gọn; nếu giảng viên yêu cầu tách, chỉ cần cắt từng lớp ra file riêng (mỗi lớp độc lập).

## 3. Dữ liệu mẫu (thư mục `data/`)

Định dạng: mỗi dòng một bản ghi, các trường cách nhau bằng `|`.

**data/TaiKhoan.txt** (`username|password|role`)
```
sv001|123|STUDENT
sv002|123|STUDENT
pdt001|123|PHONG
ctsv001|123|PHONG
q01|123|QUAY
q02|123|QUAY
q03|123|QUAY
```

**data/SinhVien.txt** (`maSV|username`)
```
2212345|sv001
2212346|sv002
```

**data/Phong.txt** (`maPHG|tenPhong|username`)
```
PH001|Phòng Đào Tạo|pdt001
PH002|Phòng CTSV|ctsv001
```

**data/Quay.txt** (`maQuay|maPHG|username|active`)
```
Q01|PH001|q01|1
Q02|PH001|q02|1
Q03|PH002|q03|1
```

**data/VanBan.txt** (`maVB|tenVB|maPHG`)
```
VB001|Giấy xác nhận sinh viên|PH002
VB002|Giấy vay vốn|PH002
VB003|Giấy tạm hoãn NVQS|PH002
VB004|Bảng điểm toàn khóa|PH001
VB005|Giấy xác nhận hoàn thành chương trình|PH001
```

Các file sau **tự được tạo** khi có dữ liệu, không cần tạo trước: `DangKy.txt`, `PhieuHangDoi.txt`, `CurrentServing.txt`, `PrintList.txt`.

- **DangKy.txt**: `maDK|maSV|maVB|trangThai|lyDoTuChoi|ngayDangKy|ngayIn|ngayNhan`
- **PhieuHangDoi.txt**: `maPhieu|soThuTu|maSV|maQuay|trangThai`
- **CurrentServing.txt**: `maQuay|soDangPhucVu|active`

---

## 4. Templates (tự cài đặt)

### templates/MyList.h

```cpp
#ifndef MYLIST_H
#define MYLIST_H

#include <stdexcept>

// Danh sách động tự cài đặt (mảng tự giãn nở)
template<class T>
class MyList
{
private:
    T* data;
    int size;
    int capacity;

    void grow()
    {
        int newCap = capacity * 2;
        T* nd = new T[newCap];
        for (int i = 0; i < size; i++) nd[i] = data[i];
        delete[] data;
        data = nd;
        capacity = newCap;
    }

public:
    MyList() : data(new T[4]), size(0), capacity(4) {}

    MyList(const MyList& o) : data(new T[o.capacity]), size(o.size), capacity(o.capacity)
    {
        for (int i = 0; i < size; i++) data[i] = o.data[i];
    }

    MyList& operator=(const MyList& o)
    {
        if (this != &o)
        {
            T* nd = new T[o.capacity];
            for (int i = 0; i < o.size; i++) nd[i] = o.data[i];
            delete[] data;
            data = nd;
            size = o.size;
            capacity = o.capacity;
        }
        return *this;
    }

    ~MyList() { delete[] data; }

    void add(const T& value)
    {
        if (size == capacity) grow();
        data[size++] = value;
    }

    void remove(int index)
    {
        if (index < 0 || index >= size) throw std::out_of_range("MyList::remove");
        for (int i = index; i < size - 1; i++) data[i] = data[i + 1];
        size--;
    }

    T& get(int index)
    {
        if (index < 0 || index >= size) throw std::out_of_range("MyList::get");
        return data[index];
    }

    const T& get(int index) const
    {
        if (index < 0 || index >= size) throw std::out_of_range("MyList::get");
        return data[index];
    }

    T& operator[](int index) { return get(index); }
    const T& operator[](int index) const { return get(index); }

    int getSize() const { return size; }
    bool isEmpty() const { return size == 0; }
    void clear() { size = 0; }
};

#endif
```

### templates/MyQueue.h

```cpp
#ifndef MYQUEUE_H
#define MYQUEUE_H

#include <stdexcept>

// Hàng đợi vòng (circular queue) tự cài đặt, tự giãn nở
template<class T>
class MyQueue
{
private:
    T* data;
    int front;
    int rear;
    int count;
    int capacity;

    void grow()
    {
        T* nd = new T[capacity * 2];
        for (int i = 0; i < count; i++) nd[i] = data[(front + i) % capacity];
        delete[] data;
        data = nd;
        front = 0;
        rear = count;
        capacity *= 2;
    }

public:
    MyQueue() : data(new T[8]), front(0), rear(0), count(0), capacity(8) {}

    MyQueue(const MyQueue& o) : data(new T[o.capacity]), front(0), rear(o.count), count(o.count), capacity(o.capacity)
    {
        for (int i = 0; i < count; i++) data[i] = o.data[(o.front + i) % o.capacity];
        if (rear == capacity) rear = 0;
    }

    MyQueue& operator=(const MyQueue& o)
    {
        if (this != &o)
        {
            T* nd = new T[o.capacity];
            for (int i = 0; i < o.count; i++) nd[i] = o.data[(o.front + i) % o.capacity];
            delete[] data;
            data = nd;
            front = 0;
            count = o.count;
            capacity = o.capacity;
            rear = (count == capacity) ? 0 : count;
        }
        return *this;
    }

    ~MyQueue() { delete[] data; }

    void enqueue(const T& item)
    {
        if (count == capacity) grow();
        data[rear] = item;
        rear = (rear + 1) % capacity;
        count++;
    }

    void dequeue()
    {
        if (count == 0) throw std::out_of_range("MyQueue::dequeue");
        front = (front + 1) % capacity;
        count--;
    }

    T& peek()
    {
        if (count == 0) throw std::out_of_range("MyQueue::peek");
        return data[front];
    }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
};

#endif
```

### templates/Repository.h

```cpp
#ifndef REPOSITORY_H
#define REPOSITORY_H

// Giao diện Repository tổng quát (Repository Pattern)
template<class T>
class Repository
{
public:
    virtual ~Repository() {}
    virtual void load() = 0;
    virtual void save() = 0;
    virtual void add(const T& obj) = 0;
    virtual void update(const T& obj) = 0;
};

#endif
```

---

## 5. Utils

### utils/Utils.h

```cpp
#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include "../templates/MyList.h"

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif

using namespace std;

// ---------- Chuỗi ----------
// Tách chuỗi theo ký tự phân cách, GIỮ các trường rỗng
inline MyList<string> split(const string& s, char delim)
{
    MyList<string> r;
    string cur;
    for (size_t i = 0; i < s.size(); i++)
    {
        if (s[i] == delim) { r.add(cur); cur.clear(); }
        else cur += s[i];
    }
    r.add(cur);
    return r;
}

inline string trim(const string& s)
{
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

inline string toUpper(string s)
{
    for (size_t i = 0; i < s.size(); i++)
        if (s[i] >= 'a' && s[i] <= 'z') s[i] -= 32;
    return s;
}

// Đệm số 0 phía trước: pad(5,3) -> "005"
inline string pad(int n, int width)
{
    string s = to_string(n);
    while ((int)s.size() < width) s = "0" + s;
    return s;
}

// Chuyển chuỗi toàn chữ số thành int, sai định dạng trả về def
inline int toInt(const string& s, int def = 0)
{
    if (s.empty()) return def;
    for (size_t i = 0; i < s.size(); i++)
        if (s[i] < '0' || s[i] > '9') return def;
    return atoi(s.c_str());
}

// ---------- Ngày tháng (dd/mm/yyyy) ----------
inline long daysFromCivil(int y, int m, int d)
{
    y -= (m <= 2);
    long era = (y >= 0 ? y : y - 399) / 400;
    unsigned yoe = (unsigned)(y - era * 400);
    unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + (long)doe - 719468;
}

inline bool isValidDate(int d, int m, int y)
{
    if (m < 1 || m > 12 || d < 1 || y < 2000 || y > 2100) return false;
    int dm[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    int maxDay = dm[m - 1] + ((m == 2 && leap) ? 1 : 0);
    return d <= maxDay;
}

// Kiểm tra và chuẩn hóa về dd/mm/yyyy
inline bool normalizeDate(string& s)
{
    int d, m, y;
    s = trim(s);
    if (sscanf(s.c_str(), "%d/%d/%d", &d, &m, &y) != 3) return false;
    if (!isValidDate(d, m, y)) return false;
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d", d, m, y);
    s = buf;
    return true;
}

// Số ngày kể từ 1/1/1970; trả về -1 nếu chuỗi không hợp lệ
inline long dateToDays(const string& s)
{
    int d, m, y;
    if (sscanf(s.c_str(), "%d/%d/%d", &d, &m, &y) != 3) return -1;
    return daysFromCivil(y, m, d);
}

inline string today()
{
    time_t t = time(0);
    tm* l = localtime(&t);
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d", l->tm_mday, l->tm_mon + 1, l->tm_year + 1900);
    return string(buf);
}

// ---------- Nhập xuất ----------
inline string readLine(const string& prompt)
{
    cout << prompt;
    string s;
    if (!getline(cin, s)) { cout << endl; exit(0); }
    return trim(s);
}

inline int readInt(const string& prompt)
{
    while (true)
    {
        string s = readLine(prompt);
        if (!s.empty() && toInt(s, -1) != -1) return atoi(s.c_str());
        cout << "Vui lòng nhập một số nguyên không âm.\n";
    }
}

inline void pauseScreen() { readLine("\nNhấn Enter để tiếp tục..."); }

inline void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Phân tích danh sách chọn "1 2 4" hoặc "ALL" -> danh sách chỉ số (0-based)
inline MyList<int> parseSelection(const string& line, int maxIdx)
{
    MyList<int> r;
    if (toUpper(trim(line)) == "ALL")
    {
        for (int i = 0; i < maxIdx; i++) r.add(i);
        return r;
    }
    stringstream ss(line);
    string tok;
    while (ss >> tok)
    {
        int v = toInt(tok, -1);
        if (v >= 1 && v <= maxIdx) r.add(v - 1);
    }
    return r;
}

// Chờ tối đa ms mili-giây; trả về true nếu người dùng bấm phím (Enter) để thoát
#ifdef _WIN32
inline bool waitKey(int ms)
{
    int t = 0;
    while (t < ms)
    {
        if (_kbhit()) { _getch(); return true; }
        Sleep(100);
        t += 100;
    }
    return false;
}
#else
inline bool waitKey(int ms)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    int r = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
    if (r > 0)
    {
        string tmp;
        getline(cin, tmp);
        return true;
    }
    return false;
}
#endif

#endif
```

---

## 6. Models

### models/Models.h

```cpp
#ifndef MODELS_H
#define MODELS_H

#include "../utils/Utils.h"

// ---------- Hằng số ----------
const string ROLE_STUDENT = "STUDENT";
const string ROLE_PHONG   = "PHONG";
const string ROLE_QUAY    = "QUAY";

const string ST_PENDING  = "PENDING";
const string ST_APPROVED = "APPROVED";
const string ST_REJECTED = "REJECTED";
const string ST_DONE     = "DONE";

const string Q_WAITING  = "WAITING";
const string Q_SERVING  = "SERVING";
const string Q_FINISHED = "FINISHED";

// Tách dòng và đảm bảo đủ n trường (thiếu thì bù chuỗi rỗng)
inline MyList<string> splitN(const string& line, int n)
{
    MyList<string> f = split(line, '|');
    while (f.getSize() < n) f.add("");
    return f;
}

// Không cho phép ký tự '|' trong dữ liệu người dùng nhập
inline string cleanField(string s)
{
    for (size_t i = 0; i < s.size(); i++)
        if (s[i] == '|') s[i] = '/';
    return s;
}

// =====================================================
// Lớp cha
// =====================================================
class Nguoi
{
protected:
    string username;

public:
    Nguoi() {}
    Nguoi(const string& u) : username(u) {}
    virtual ~Nguoi() {}
    string getUsername() const { return username; }
};

// =====================================================
// TaiKhoan
// =====================================================
class TaiKhoan
{
private:
    string username;
    string password;
    string role;

public:
    TaiKhoan() {}
    TaiKhoan(const string& u, const string& p, const string& r) : username(u), password(p), role(r) {}

    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getRole() const { return role; }

    string getKey() const { return username; }
    string toLine() const { return username + "|" + password + "|" + role; }
    static TaiKhoan fromLine(const string& line)
    {
        MyList<string> f = splitN(line, 3);
        return TaiKhoan(f[0], f[1], f[2]);
    }
};

// =====================================================
// SinhVien (kế thừa Nguoi)
// =====================================================
class SinhVien : public Nguoi
{
private:
    string maSV;

public:
    SinhVien() {}
    SinhVien(const string& ma, const string& u) : Nguoi(u), maSV(ma) {}

    string getMaSV() const { return maSV; }

    string getKey() const { return maSV; }
    string toLine() const { return maSV + "|" + username; }
    static SinhVien fromLine(const string& line)
    {
        MyList<string> f = splitN(line, 2);
        return SinhVien(f[0], f[1]);
    }
};

// =====================================================
// Phong = NhanVienPhong (kế thừa Nguoi)
// =====================================================
class Phong : public Nguoi
{
private:
    string maPHG;
    string tenPhong;

public:
    Phong() {}
    Phong(const string& ma, const string& ten, const string& u) : Nguoi(u), maPHG(ma), tenPhong(ten) {}

    string getMaPHG() const { return maPHG; }
    string getTenPhong() const { return tenPhong; }

    string getKey() const { return maPHG; }
    string toLine() const { return maPHG + "|" + tenPhong + "|" + username; }
    static Phong fromLine(const string& line)
    {
        MyList<string> f = splitN(line, 3);
        return Phong(f[0], f[1], f[2]);
    }
};
typedef Phong NhanVienPhong;

// =====================================================
// Quay = NhanVienQuay (kế thừa Nguoi)
// =====================================================
class Quay : public Nguoi
{
private:
    string maQuay;
    string maPHG;
    bool active;

public:
    Quay() : active(true) {}
    Quay(const string& mq, const string& mp, const string& u, bool a)
        : Nguoi(u), maQuay(mq), maPHG(mp), active(a) {}

    string getMaQuay() const { return maQuay; }
    string getMaPHG() const { return maPHG; }
    bool isActive() const { return active; }

    string getKey() const { return maQuay; }
    string toLine() const { return maQuay + "|" + maPHG + "|" + username + "|" + (active ? "1" : "0"); }
    static Quay fromLine(const string& line)
    {
        MyList<string> f = splitN(line, 4);
        return Quay(f[0], f[1], f[2], f[3] != "0");
    }
};
typedef Quay NhanVienQuay;

// =====================================================
// VanBan
// =====================================================
class VanBan
{
private:
    string maVB;
    string tenVB;
    string maPHG;

public:
    VanBan() {}
    VanBan(const string& ma, const string& ten, const string& mp) : maVB(ma), tenVB(ten), maPHG(mp) {}

    string getMaVB() const { return maVB; }
    string getTenVB() const { return tenVB; }
    string getMaPHG() const { return maPHG; }

    string getKey() const { return maVB; }
    string toLine() const { return maVB + "|" + tenVB + "|" + maPHG; }
    static VanBan fromLine(const string& line)
    {
        MyList<string> f = splitN(line, 3);
        return VanBan(f[0], f[1], f[2]);
    }
};

// =====================================================
// DangKy
// =====================================================
class DangKy
{
private:
    string maDK;
    string maSV;
    string maVB;
    string trangThai;
    string lyDoTuChoi;
    string ngayDangKy;
    string ngayIn;
    string ngayNhan;

public:
    DangKy() {}
    DangKy(const string& dk, const string& sv, const string& vb, const string& tt,
           const string& lydo, const string& ndk, const string& ni, const string& nn)
        : maDK(dk), maSV(sv), maVB(vb), trangThai(tt), lyDoTuChoi(lydo),
          ngayDangKy(ndk), ngayIn(ni), ngayNhan(nn) {}

    string getMaDK() const { return maDK; }
    string getMaSV() const { return maSV; }
    string getMaVB() const { return maVB; }
    string getTrangThai() const { return trangThai; }
    string getLyDo() const { return lyDoTuChoi; }
    string getNgayDK() const { return ngayDangKy; }
    string getNgayIn() const { return ngayIn; }
    string getNgayNhan() const { return ngayNhan; }

    void setTrangThai(const string& v) { trangThai = v; }
    void setLyDo(const string& v) { lyDoTuChoi = v; }
    void setNgayIn(const string& v) { ngayIn = v; }
    void setNgayNhan(const string& v) { ngayNhan = v; }

    string getKey() const { return maDK; }
    string toLine() const
    {
        return maDK + "|" + maSV + "|" + maVB + "|" + trangThai + "|" + lyDoTuChoi + "|" +
               ngayDangKy + "|" + ngayIn + "|" + ngayNhan;
    }
    static DangKy fromLine(const string& line)
    {
        MyList<string> f = splitN(line, 8);
        return DangKy(f[0], f[1], f[2], f[3], f[4], f[5], f[6], f[7]);
    }
};

// =====================================================
// PhieuHangDoi
// =====================================================
class PhieuHangDoi
{
private:
    string maPhieu;
    int soThuTu;
    string maSV;
    string maQuay;
    string trangThai;

public:
    PhieuHangDoi() : soThuTu(0) {}
    PhieuHangDoi(const string& p, int stt, const string& sv, const string& q, const string& tt)
        : maPhieu(p), soThuTu(stt), maSV(sv), maQuay(q), trangThai(tt) {}

    string getMaPhieu() const { return maPhieu; }
    int getSoThuTu() const { return soThuTu; }
    string getMaSV() const { return maSV; }
    string getMaQuay() const { return maQuay; }
    string getTrangThai() const { return trangThai; }
    void setTrangThai(const string& v) { trangThai = v; }

    // Số hiển thị trên vé, ví dụ A015
    string getDisplay() const { return "A" + pad(soThuTu, 3); }

    string getKey() const { return maPhieu; }
    string toLine() const
    {
        return maPhieu + "|" + to_string(soThuTu) + "|" + maSV + "|" + maQuay + "|" + trangThai;
    }
    static PhieuHangDoi fromLine(const string& line)
    {
        MyList<string> f = splitN(line, 5);
        return PhieuHangDoi(f[0], toInt(f[1]), f[2], f[3], f[4]);
    }
};

#endif
```

---

## 7. Data Layer

### data/FileManager.h

```cpp
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <fstream>
#include "../utils/Utils.h"

const string DATA_DIR = "data/";

// Đọc / ghi file TXT theo dòng
class FileManager
{
public:
    static bool exists(const string& path)
    {
        ifstream f(path.c_str());
        return f.good();
    }

    // Đọc tất cả dòng không rỗng (bỏ BOM UTF-8 nếu có). File không tồn tại -> danh sách rỗng
    static MyList<string> readLines(const string& path)
    {
        MyList<string> r;
        ifstream f(path.c_str());
        if (!f) return r;
        string line;
        bool first = true;
        while (getline(f, line))
        {
            if (first && line.size() >= 3 &&
                (unsigned char)line[0] == 0xEF && (unsigned char)line[1] == 0xBB && (unsigned char)line[2] == 0xBF)
                line = line.substr(3);
            first = false;
            line = trim(line);
            if (!line.empty()) r.add(line);
        }
        return r;
    }

    static bool writeLines(const string& path, const MyList<string>& lines)
    {
        ofstream f(path.c_str(), ios::trunc);
        if (!f) return false;
        for (int i = 0; i < lines.getSize(); i++) f << lines[i] << "\n";
        return true;
    }
};

#endif
```

### data/TxtRepository.h

```cpp
#ifndef TXTREPOSITORY_H
#define TXTREPOSITORY_H

#include "../templates/Repository.h"
#include "../templates/MyList.h"
#include "FileManager.h"

// Repository tổng quát lưu vào file TXT.
// Yêu cầu kiểu T có: T::fromLine(string), toLine() const, getKey() const
template<class T>
class TxtRepository : public Repository<T>
{
private:
    string filePath;
    MyList<T> items;

public:
    TxtRepository(const string& path) : filePath(path) {}

    void load()
    {
        items.clear();
        MyList<string> lines = FileManager::readLines(filePath);
        for (int i = 0; i < lines.getSize(); i++) items.add(T::fromLine(lines[i]));
    }

    void save()
    {
        MyList<string> lines;
        for (int i = 0; i < items.getSize(); i++) lines.add(items[i].toLine());
        FileManager::writeLines(filePath, lines);
    }

    void add(const T& obj)
    {
        items.add(obj);
        save();
    }

    // Cập nhật bản ghi có cùng getKey()
    void update(const T& obj)
    {
        for (int i = 0; i < items.getSize(); i++)
        {
            if (items[i].getKey() == obj.getKey())
            {
                items[i] = obj;
                save();
                return;
            }
        }
    }

    const MyList<T>& getAll() const { return items; }

    bool findByKey(const string& key, T& out) const
    {
        for (int i = 0; i < items.getSize(); i++)
        {
            if (items[i].getKey() == key) { out = items[i]; return true; }
        }
        return false;
    }
};

#endif
```

### data/Repositories.h

```cpp
#ifndef REPOSITORIES_H
#define REPOSITORIES_H

#include "../models/Models.h"
#include "TxtRepository.h"

typedef TxtRepository<TaiKhoan>     TaiKhoanRepo;
typedef TxtRepository<SinhVien>     SinhVienRepo;
typedef TxtRepository<Phong>        PhongRepo;
typedef TxtRepository<Quay>         QuayRepo;
typedef TxtRepository<VanBan>       VanBanRepo;
typedef TxtRepository<DangKy>       DangKyRepo;
typedef TxtRepository<PhieuHangDoi> PhieuRepo;

#endif
```

---

## 8. Business Layer

### business/AuthService.h

```cpp
#ifndef AUTHSERVICE_H
#define AUTHSERVICE_H

#include "../data/Repositories.h"

class AuthService
{
private:
    TaiKhoanRepo& repo;

public:
    AuthService(TaiKhoanRepo& r) : repo(r) {}

    // Đăng nhập: tra cứu TaiKhoan.txt
    bool login(const string& username, const string& password, TaiKhoan& out)
    {
        repo.load();
        const MyList<TaiKhoan>& all = repo.getAll();
        for (int i = 0; i < all.getSize(); i++)
        {
            if (all[i].getUsername() == username && all[i].getPassword() == password)
            {
                out = all[i];
                return true;
            }
        }
        return false;
    }
};

#endif
```

### business/DocumentService.h

```cpp
#ifndef DOCUMENTSERVICE_H
#define DOCUMENTSERVICE_H

#include "../data/Repositories.h"

class DocumentService
{
private:
    PhongRepo& phongRepo;
    VanBanRepo& vbRepo;

public:
    DocumentService(PhongRepo& p, VanBanRepo& v) : phongRepo(p), vbRepo(v) {}

    MyList<Phong> getAllPhong()
    {
        phongRepo.load();
        return phongRepo.getAll();
    }

    MyList<VanBan> getVanBanByPhong(const string& maPHG)
    {
        vbRepo.load();
        MyList<VanBan> r;
        const MyList<VanBan>& all = vbRepo.getAll();
        for (int i = 0; i < all.getSize(); i++)
            if (all[i].getMaPHG() == maPHG) r.add(all[i]);
        return r;
    }

    bool findPhongByUsername(const string& username, Phong& out)
    {
        phongRepo.load();
        const MyList<Phong>& all = phongRepo.getAll();
        for (int i = 0; i < all.getSize(); i++)
        {
            if (all[i].getUsername() == username) { out = all[i]; return true; }
        }
        return false;
    }
};

#endif
```

### business/RegistrationService.h

```cpp
#ifndef REGISTRATIONSERVICE_H
#define REGISTRATIONSERVICE_H

#include "../data/Repositories.h"

// Nghiệp vụ đăng ký, duyệt, in, trả giấy tờ
class RegistrationService
{
private:
    DangKyRepo& dkRepo;
    VanBanRepo& vbRepo;
    SinhVienRepo& svRepo;

    string nextMaDK()
    {
        int mx = 0;
        const MyList<DangKy>& all = dkRepo.getAll();
        for (int i = 0; i < all.getSize(); i++)
        {
            string k = all[i].getMaDK();
            if (k.size() > 2)
            {
                int n = toInt(k.substr(2), 0);
                if (n > mx) mx = n;
            }
        }
        return "DK" + pad(mx + 1, 3);
    }

    bool thuocPhong(const string& maVB, const string& maPHG)
    {
        VanBan vb;
        return vbRepo.findByKey(maVB, vb) && vb.getMaPHG() == maPHG;
    }

    // mode: 0 = mọi hồ sơ, 1 = chưa có ngày in, 2 = đã có ngày in
    MyList<DangKy> collect(const string& maPHG, const string& status, int mode)
    {
        dkRepo.load();
        vbRepo.load();
        MyList<DangKy> r;
        const MyList<DangKy>& all = dkRepo.getAll();
        for (int i = 0; i < all.getSize(); i++)
        {
            const DangKy& d = all[i];
            if (d.getTrangThai() != status) continue;
            if (!thuocPhong(d.getMaVB(), maPHG)) continue;
            if (mode == 1 && !d.getNgayIn().empty()) continue;
            if (mode == 2 && d.getNgayIn().empty()) continue;
            r.add(d);
        }
        return r;
    }

    bool loadPending(const string& maDK, DangKy& dk, string& msg)
    {
        dkRepo.load();
        if (!dkRepo.findByKey(maDK, dk)) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
        if (dk.getTrangThai() != ST_PENDING)
        {
            msg = "Đơn " + maDK + " không ở trạng thái PENDING.";
            return false;
        }
        return true;
    }

public:
    RegistrationService(DangKyRepo& d, VanBanRepo& v, SinhVienRepo& s) : dkRepo(d), vbRepo(v), svRepo(s) {}

    bool getMaSV(const string& username, string& maSV)
    {
        svRepo.load();
        const MyList<SinhVien>& all = svRepo.getAll();
        for (int i = 0; i < all.getSize(); i++)
        {
            if (all[i].getUsername() == username) { maSV = all[i].getMaSV(); return true; }
        }
        return false;
    }

    string tenVanBan(const string& maVB)
    {
        vbRepo.load();
        VanBan vb;
        if (vbRepo.findByKey(maVB, vb)) return vb.getTenVB();
        return maVB;
    }

    // UC01: Đăng ký giấy tờ -> tạo hồ sơ PENDING
    bool registerDocument(const string& maSV, const string& maVB, string& msg)
    {
        dkRepo.load();
        vbRepo.load();
        VanBan vb;
        if (!vbRepo.findByKey(maVB, vb)) { msg = "Văn bản không tồn tại."; return false; }

        const MyList<DangKy>& all = dkRepo.getAll();
        for (int i = 0; i < all.getSize(); i++)
        {
            if (all[i].getMaSV() == maSV && all[i].getMaVB() == maVB &&
                (all[i].getTrangThai() == ST_PENDING || all[i].getTrangThai() == ST_APPROVED))
            {
                msg = "Bạn đã có hồ sơ " + all[i].getMaDK() + " cho văn bản này đang được xử lý.";
                return false;
            }
        }

        DangKy dk(nextMaDK(), maSV, maVB, ST_PENDING, "", today(), "", "");
        dkRepo.add(dk);
        msg = "Đăng ký thành công. Mã đơn: " + dk.getMaDK();
        return true;
    }

    // UC02: Hồ sơ của một sinh viên
    MyList<DangKy> getBySinhVien(const string& maSV)
    {
        dkRepo.load();
        MyList<DangKy> r;
        const MyList<DangKy>& all = dkRepo.getAll();
        for (int i = 0; i < all.getSize(); i++)
            if (all[i].getMaSV() == maSV) r.add(all[i]);
        return r;
    }

    // UC03: Hồ sơ chờ duyệt của phòng
    MyList<DangKy> getPending(const string& maPHG) { return collect(maPHG, ST_PENDING, 0); }

    bool approve(const string& maDK, string& msg)
    {
        DangKy dk;
        if (!loadPending(maDK, dk, msg)) return false;
        dk.setTrangThai(ST_APPROVED);
        dkRepo.update(dk);
        msg = "Đã duyệt đơn " + maDK + ".";
        return true;
    }

    bool reject(const string& maDK, const string& reason, string& msg)
    {
        DangKy dk;
        if (!loadPending(maDK, dk, msg)) return false;
        dk.setTrangThai(ST_REJECTED);
        dk.setLyDo(cleanField(reason));
        dkRepo.update(dk);
        msg = "Đã từ chối đơn " + maDK + ".";
        return true;
    }

    // Hồ sơ APPROVED chưa có ngày in
    MyList<DangKy> getForPrint(const string& maPHG) { return collect(maPHG, ST_APPROVED, 1); }

    // Hồ sơ APPROVED đã in (để cập nhật ngày nhận)
    MyList<DangKy> getForReceive(const string& maPHG) { return collect(maPHG, ST_APPROVED, 2); }

    // UC04: Xuất danh sách in -> file. Trả về số hồ sơ, -1 nếu lỗi ghi file
    int exportPrintList(const string& maPHG, const string& path)
    {
        MyList<DangKy> ds = getForPrint(maPHG);
        if (ds.isEmpty()) return 0;

        MyList<string> lines;
        lines.add("DANH SACH CAN IN - NGAY " + today());
        lines.add("STT | MaDon | MSSV | TenVanBan | NgayDangKy");
        for (int i = 0; i < ds.getSize(); i++)
        {
            lines.add(to_string(i + 1) + " | " + ds[i].getMaDK() + " | " + ds[i].getMaSV() + " | " +
                      tenVanBan(ds[i].getMaVB()) + " | " + ds[i].getNgayDK());
        }
        if (!FileManager::writeLines(path, lines)) return -1;
        return ds.getSize();
    }

    // UC05: Cập nhật ngày in
    bool setNgayIn(const string& maDK, string date, string& msg)
    {
        dkRepo.load();
        DangKy dk;
        if (!dkRepo.findByKey(maDK, dk)) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
        if (dk.getTrangThai() != ST_APPROVED) { msg = "Đơn " + maDK + " chưa được duyệt (APPROVED)."; return false; }
        if (!normalizeDate(date)) { msg = "Ngày không hợp lệ (định dạng dd/mm/yyyy)."; return false; }
        dk.setNgayIn(date);
        dkRepo.update(dk);
        msg = "Đã cập nhật ngày in cho đơn " + maDK + ": " + date;
        return true;
    }

    // UC06: Cập nhật ngày nhận (>= ngày in)
    bool setNgayNhan(const string& maDK, string date, string& msg)
    {
        dkRepo.load();
        DangKy dk;
        if (!dkRepo.findByKey(maDK, dk)) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
        if (dk.getTrangThai() != ST_APPROVED) { msg = "Đơn " + maDK + " chưa được duyệt (APPROVED)."; return false; }
        if (dk.getNgayIn().empty()) { msg = "Đơn " + maDK + " chưa có ngày in."; return false; }
        if (!normalizeDate(date)) { msg = "Ngày không hợp lệ (định dạng dd/mm/yyyy)."; return false; }
        if (dateToDays(date) < dateToDays(dk.getNgayIn()))
        {
            msg = "Ngày nhận không được trước ngày in (" + dk.getNgayIn() + ").";
            return false;
        }
        dk.setNgayNhan(date);
        dkRepo.update(dk);
        msg = "Đã cập nhật ngày nhận cho đơn " + maDK + ": " + date;
        return true;
    }

    // UC12: Xác nhận trả giấy. Điều kiện: ngayNhan <= hôm nay <= ngayNhan + 30 ngày
    bool confirmReceive(const string& maDK, const string& maSVDangPhucVu, string& msg)
    {
        dkRepo.load();
        DangKy dk;
        if (!dkRepo.findByKey(maDK, dk)) { msg = "Không tìm thấy đơn " + maDK + "."; return false; }
        if (dk.getMaSV() != maSVDangPhucVu) { msg = "Đơn này không thuộc sinh viên đang được phục vụ."; return false; }
        if (dk.getTrangThai() != ST_APPROVED) { msg = "Đơn chưa ở trạng thái APPROVED (hiện tại: " + dk.getTrangThai() + ")."; return false; }
        if (dk.getNgayNhan().empty()) { msg = "Đơn chưa có ngày nhận."; return false; }

        long t = dateToDays(today());
        long n = dateToDays(dk.getNgayNhan());
        if (t < n) { msg = "Chưa đến ngày nhận (" + dk.getNgayNhan() + ")."; return false; }
        if (t > n + 30) { msg = "Đã quá hạn nhận giấy (hạn: ngày nhận + 30 ngày)."; return false; }

        dk.setTrangThai(ST_DONE);
        dkRepo.update(dk);
        msg = "Đã nhận thành công. Đơn " + maDK + " chuyển sang DONE.";
        return true;
    }
};

#endif
```

### business/DashboardService.h

```cpp
#ifndef DASHBOARDSERVICE_H
#define DASHBOARDSERVICE_H

#include "../data/Repositories.h"

struct CounterDisplay
{
    string maQuay;
    string soHienThi;
    bool active;
    CounterDisplay() : active(true) {}
};

// Dashboard công khai: ghi / đọc CurrentServing.txt
class DashboardService
{
private:
    PhieuRepo& phieuRepo;
    QuayRepo& quayRepo;
    string path;

public:
    DashboardService(PhieuRepo& p, QuayRepo& q, const string& file) : phieuRepo(p), quayRepo(q), path(file) {}

    // Dựng lại CurrentServing.txt từ danh sách phiếu SERVING hiện tại
    void refresh()
    {
        phieuRepo.load();
        quayRepo.load();
        MyList<string> lines;
        const MyList<Quay>& qs = quayRepo.getAll();
        const MyList<PhieuHangDoi>& tk = phieuRepo.getAll();
        for (int i = 0; i < qs.getSize(); i++)
        {
            string so = "---";
            for (int j = 0; j < tk.getSize(); j++)
            {
                if (tk[j].getMaQuay() == qs[i].getMaQuay() && tk[j].getTrangThai() == Q_SERVING)
                    so = tk[j].getDisplay();
            }
            lines.add(qs[i].getMaQuay() + "|" + so + "|" + (qs[i].isActive() ? "1" : "0"));
        }
        FileManager::writeLines(path, lines);
    }

    MyList<CounterDisplay> readCurrent()
    {
        MyList<CounterDisplay> r;
        MyList<string> lines = FileManager::readLines(path);
        for (int i = 0; i < lines.getSize(); i++)
        {
            MyList<string> f = splitN(lines[i], 3);
            CounterDisplay c;
            c.maQuay = f[0];
            c.soHienThi = f[1];
            c.active = (f[2] != "0");
            r.add(c);
        }
        return r;
    }
};

#endif
```

### business/QueueService.h

```cpp
#ifndef QUEUESERVICE_H
#define QUEUESERVICE_H

#include "../data/Repositories.h"
#include "../templates/MyQueue.h"
#include "DashboardService.h"

// Quản lý hàng đợi nhiều quầy + chia tải tự động
class QueueService
{
private:
    PhieuRepo& phieuRepo;
    QuayRepo& quayRepo;
    SinhVienRepo& svRepo;
    DashboardService& dash;

    // Đếm số phiếu WAITING của một quầy (không load lại file)
    int countWaitingNoLoad(const string& maQuay)
    {
        int c = 0;
        const MyList<PhieuHangDoi>& tk = phieuRepo.getAll();
        for (int i = 0; i < tk.getSize(); i++)
            if (tk[i].getMaQuay() == maQuay && tk[i].getTrangThai() == Q_WAITING) c++;
        return c;
    }

public:
    QueueService(PhieuRepo& p, QuayRepo& q, SinhVienRepo& s, DashboardService& d)
        : phieuRepo(p), quayRepo(q), svRepo(s), dash(d) {}

    // UC07 + UC08: Guest nhập MSSV -> tạo phiếu, gán vào quầy ĐANG HOẠT ĐỘNG có ít người chờ nhất
    bool takeTicket(const string& maSV, PhieuHangDoi& out, string& msg)
    {
        svRepo.load();
        quayRepo.load();
        phieuRepo.load();

        SinhVien sv;
        if (!svRepo.findByKey(maSV, sv)) { msg = "MSSV không tồn tại."; return false; }

        const MyList<PhieuHangDoi>& tk = phieuRepo.getAll();
        int mx = 0;
        for (int i = 0; i < tk.getSize(); i++)
        {
            if (tk[i].getSoThuTu() > mx) mx = tk[i].getSoThuTu();
            if (tk[i].getMaSV() == maSV && tk[i].getTrangThai() == Q_WAITING)
            {
                out = tk[i];
                msg = "Bạn đang có số " + tk[i].getDisplay() + " chờ tại quầy " + tk[i].getMaQuay() + ".";
                return false;
            }
        }

        string best = "";
        int bestCnt = 0;
        const MyList<Quay>& qs = quayRepo.getAll();
        for (int i = 0; i < qs.getSize(); i++)
        {
            if (!qs[i].isActive()) continue;
            int c = countWaitingNoLoad(qs[i].getMaQuay());
            if (best.empty() || c < bestCnt) { best = qs[i].getMaQuay(); bestCnt = c; }
        }
        if (best.empty()) { msg = "Hiện không có quầy nào đang hoạt động."; return false; }

        PhieuHangDoi p("T" + pad(mx + 1, 4), mx + 1, maSV, best, Q_WAITING);
        phieuRepo.add(p);
        out = p;
        return true;
    }

    // UC10: Quầy gọi số tiếp theo (WAITING -> SERVING). Phiếu SERVING trước đó -> FINISHED
    bool callNext(const string& maQuay, PhieuHangDoi& out, string& msg)
    {
        phieuRepo.load();
        const MyList<PhieuHangDoi>& tk = phieuRepo.getAll();

        // Dựng MyQueue từ các phiếu WAITING của quầy (theo thứ tự số tăng dần)
        MyQueue<PhieuHangDoi> q;
        for (int i = 0; i < tk.getSize(); i++)
            if (tk[i].getMaQuay() == maQuay && tk[i].getTrangThai() == Q_WAITING) q.enqueue(tk[i]);

        if (q.isEmpty())
        {
            msg = "Hàng đợi của quầy " + maQuay + " đang trống.";
            return false;
        }

        for (int i = 0; i < tk.getSize(); i++)
        {
            if (tk[i].getMaQuay() == maQuay && tk[i].getTrangThai() == Q_SERVING)
            {
                PhieuHangDoi old = tk[i];
                old.setTrangThai(Q_FINISHED);
                phieuRepo.update(old);
            }
        }

        PhieuHangDoi next = q.peek();
        q.dequeue();
        next.setTrangThai(Q_SERVING);
        phieuRepo.update(next);
        dash.refresh();
        out = next;
        return true;
    }

    bool getServing(const string& maQuay, PhieuHangDoi& out)
    {
        phieuRepo.load();
        const MyList<PhieuHangDoi>& tk = phieuRepo.getAll();
        for (int i = 0; i < tk.getSize(); i++)
        {
            if (tk[i].getMaQuay() == maQuay && tk[i].getTrangThai() == Q_SERVING)
            {
                out = tk[i];
                return true;
            }
        }
        return false;
    }

    int countWaiting(const string& maQuay)
    {
        phieuRepo.load();
        return countWaitingNoLoad(maQuay);
    }

    bool findQuayByUsername(const string& username, Quay& out)
    {
        quayRepo.load();
        const MyList<Quay>& qs = quayRepo.getAll();
        for (int i = 0; i < qs.getSize(); i++)
        {
            if (qs[i].getUsername() == username) { out = qs[i]; return true; }
        }
        return false;
    }
};

#endif
```

### business/AppContext.h

```cpp
#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include "AuthService.h"
#include "DocumentService.h"
#include "RegistrationService.h"
#include "DashboardService.h"
#include "QueueService.h"

// Gom toàn bộ repository + service (thứ tự khai báo = thứ tự khởi tạo)
struct AppContext
{
    TaiKhoanRepo taiKhoanRepo;
    SinhVienRepo sinhVienRepo;
    PhongRepo phongRepo;
    QuayRepo quayRepo;
    VanBanRepo vanBanRepo;
    DangKyRepo dangKyRepo;
    PhieuRepo phieuRepo;

    AuthService auth;
    DocumentService doc;
    RegistrationService reg;
    DashboardService dash;
    QueueService queue;

    AppContext()
        : taiKhoanRepo(DATA_DIR + "TaiKhoan.txt"),
          sinhVienRepo(DATA_DIR + "SinhVien.txt"),
          phongRepo(DATA_DIR + "Phong.txt"),
          quayRepo(DATA_DIR + "Quay.txt"),
          vanBanRepo(DATA_DIR + "VanBan.txt"),
          dangKyRepo(DATA_DIR + "DangKy.txt"),
          phieuRepo(DATA_DIR + "PhieuHangDoi.txt"),
          auth(taiKhoanRepo),
          doc(phongRepo, vanBanRepo),
          reg(dangKyRepo, vanBanRepo, sinhVienRepo),
          dash(phieuRepo, quayRepo, DATA_DIR + "CurrentServing.txt"),
          queue(phieuRepo, quayRepo, sinhVienRepo, dash)
    {
    }
};

#endif
```

---

## 9. Presentation Layer

### presentation/User.h

```cpp
#ifndef USER_H
#define USER_H

#include "../business/AppContext.h"

// Lớp trừu tượng: đa hình qua showMenu()
class User
{
protected:
    TaiKhoan account;

public:
    User(const TaiKhoan& a) : account(a) {}
    virtual ~User() {}
    virtual void showMenu() = 0;
};

// In một hồ sơ đăng ký
inline void printDangKy(RegistrationService& reg, const DangKy& d)
{
    cout << d.getMaDK() << " | " << reg.tenVanBan(d.getMaVB()) << " | MSSV: " << d.getMaSV() << endl;
    cout << "   Trạng thái: " << d.getTrangThai() << " | Ngày đăng ký: " << d.getNgayDK() << endl;
    if (!d.getNgayIn().empty()) cout << "   Ngày in: " << d.getNgayIn() << endl;
    if (!d.getNgayNhan().empty()) cout << "   Ngày nhận: " << d.getNgayNhan() << endl;
    if (!d.getLyDo().empty()) cout << "   Lý do từ chối: " << d.getLyDo() << endl;
}

#endif
```

### presentation/StudentUI.h

```cpp
#ifndef STUDENTUI_H
#define STUDENTUI_H

#include "User.h"

class Student : public User
{
private:
    AppContext& ctx;
    string maSV;

    void dangKyGiayTo()
    {
        MyList<Phong> ds = ctx.doc.getAllPhong();
        if (ds.isEmpty()) { cout << "Chưa có phòng ban nào.\n"; pauseScreen(); return; }

        cout << "\n--- Chọn phòng ---\n";
        for (int i = 0; i < ds.getSize(); i++) cout << i + 1 << ". " << ds[i].getTenPhong() << endl;
        int p = readInt("Chọn phòng (0 = quay lại): ");
        if (p < 1 || p > ds.getSize()) return;

        MyList<VanBan> vb = ctx.doc.getVanBanByPhong(ds[p - 1].getMaPHG());
        if (vb.isEmpty()) { cout << "Phòng này chưa có văn bản nào.\n"; pauseScreen(); return; }

        cout << "\n--- " << ds[p - 1].getTenPhong() << " ---\n";
        for (int i = 0; i < vb.getSize(); i++) cout << i + 1 << ". " << vb[i].getTenVB() << endl;
        int v = readInt("Chọn văn bản (0 = quay lại): ");
        if (v < 1 || v > vb.getSize()) return;

        string yn = toUpper(readLine("Xác nhận đăng ký \"" + vb[v - 1].getTenVB() + "\"? (Y/N): "));
        if (yn != "Y") { cout << "Đã hủy.\n"; pauseScreen(); return; }

        string msg;
        ctx.reg.registerDocument(maSV, vb[v - 1].getMaVB(), msg);
        cout << msg << endl;
        pauseScreen();
    }

    void xemHoSo()
    {
        MyList<DangKy> ds = ctx.reg.getBySinhVien(maSV);
        cout << "\n=== HỒ SƠ ĐÃ ĐĂNG KÝ ===\n";
        if (ds.isEmpty()) cout << "Bạn chưa đăng ký hồ sơ nào.\n";
        for (int i = 0; i < ds.getSize(); i++)
        {
            printDangKy(ctx.reg, ds[i]);
            cout << endl;
        }
        pauseScreen();
    }

public:
    Student(const TaiKhoan& a, AppContext& c) : User(a), ctx(c)
    {
        if (!ctx.reg.getMaSV(a.getUsername(), maSV)) maSV = "";
    }

    void showMenu()
    {
        if (maSV.empty())
        {
            cout << "Tài khoản chưa được gắn với sinh viên nào.\n";
            pauseScreen();
            return;
        }
        while (true)
        {
            clearScreen();
            cout << "====== SINH VIÊN: " << maSV << " ======\n";
            cout << "1. Đăng ký giấy tờ\n";
            cout << "2. Xem hồ sơ đã đăng ký\n";
            cout << "3. Đăng xuất\n";
            int c = readInt("Chọn: ");
            if (c == 1) dangKyGiayTo();
            else if (c == 2) xemHoSo();
            else if (c == 3) return;
            else { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); }
        }
    }
};

#endif
```

### presentation/DepartmentUI.h

```cpp
#ifndef DEPARTMENTUI_H
#define DEPARTMENTUI_H

#include "User.h"

class Department : public User
{
private:
    AppContext& ctx;
    Phong phong;
    bool ok;

    void listWithIndex(MyList<DangKy>& ds)
    {
        for (int i = 0; i < ds.getSize(); i++)
        {
            cout << i + 1 << ". ";
            printDangKy(ctx.reg, ds[i]);
        }
    }

    void xemChoDuyet()
    {
        MyList<DangKy> ds = ctx.reg.getPending(phong.getMaPHG());
        cout << "\n=== HỒ SƠ CHỜ DUYỆT ===\n";
        if (ds.isEmpty()) cout << "Không có hồ sơ nào chờ duyệt.\n";
        listWithIndex(ds);
        pauseScreen();
    }

    void duyetHoSo()
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
            cout << "\n--- Đơn " << d.getMaDK() << " (" << ctx.reg.tenVanBan(d.getMaVB())
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

    void xuatDanhSachIn()
    {
        string path = DATA_DIR + "PrintList.txt";
        int n = ctx.reg.exportPrintList(phong.getMaPHG(), path);
        if (n < 0) cout << "Không ghi được file " << path << endl;
        else if (n == 0) cout << "Không có hồ sơ nào cần in.\n";
        else cout << "Đã xuất " << n << " hồ sơ ra file " << path << endl;
        pauseScreen();
    }

    // ngayIn = true: cập nhật ngày in; false: cập nhật ngày nhận
    void capNhatNgay(bool ngayIn)
    {
        MyList<DangKy> ds = ngayIn ? ctx.reg.getForPrint(phong.getMaPHG())
                                   : ctx.reg.getForReceive(phong.getMaPHG());
        cout << (ngayIn ? "\n=== CẬP NHẬT NGÀY IN ===\n" : "\n=== CẬP NHẬT NGÀY NHẬN ===\n");
        if (ds.isEmpty())
        {
            cout << (ngayIn ? "Không có hồ sơ nào đang chờ in.\n" : "Không có hồ sơ nào đã in để cập nhật ngày nhận.\n");
            pauseScreen();
            return;
        }
        listWithIndex(ds);

        string line = readLine("Chọn các hồ sơ (VD: 1 3, hoặc ALL, Enter để hủy): ");
        MyList<int> sel = parseSelection(line, ds.getSize());
        if (sel.isEmpty()) { cout << "Không có hồ sơ nào được chọn.\n"; pauseScreen(); return; }

        string date = readLine("Nhập ngày (dd/mm/yyyy): ");
        for (int k = 0; k < sel.getSize(); k++)
        {
            string msg;
            if (ngayIn) ctx.reg.setNgayIn(ds[sel[k]].getMaDK(), date, msg);
            else ctx.reg.setNgayNhan(ds[sel[k]].getMaDK(), date, msg);
            cout << msg << endl;
        }
        pauseScreen();
    }

public:
    Department(const TaiKhoan& a, AppContext& c)
        : User(a), ctx(c), ok(ctx.doc.findPhongByUsername(a.getUsername(), phong))
    {
    }

    void showMenu()
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
};

#endif
```

### presentation/CounterUI.h

```cpp
#ifndef COUNTERUI_H
#define COUNTERUI_H

#include "User.h"

class Counter : public User
{
private:
    AppContext& ctx;
    Quay quay;
    bool ok;

    void hienThiHoSoSV(const string& maSV)
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

    // UC10 + UC11
    void goiSoTiepTheo()
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

    void xemHoSo()
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

    // UC12
    void xacNhanTraGiay()
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

public:
    Counter(const TaiKhoan& a, AppContext& c)
        : User(a), ctx(c), ok(ctx.queue.findQuayByUsername(a.getUsername(), quay))
    {
    }

    void showMenu()
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
};

#endif
```

### presentation/DashboardUI.h

```cpp
#ifndef DASHBOARDUI_H
#define DASHBOARDUI_H

#include "../business/AppContext.h"

// UC09: Dashboard công khai, reload mỗi 3 giây từ CurrentServing.txt
class DashboardUI
{
public:
    static void run(AppContext& ctx)
    {
        while (true)
        {
            MyList<CounterDisplay> ds = ctx.dash.readCurrent();
            if (ds.isEmpty())
            {
                ctx.dash.refresh();
                ds = ctx.dash.readCurrent();
            }

            clearScreen();
            cout << "=====================================\n";
            cout << "        DASHBOARD HÀNG ĐỢI\n";
            cout << "=====================================\n";
            for (int i = 0; i < ds.getSize(); i++)
            {
                cout << " QUẦY " << ds[i].maQuay << " : " << (ds[i].active ? ds[i].soHienThi : string("ĐÓNG")) << endl;
            }
            cout << "=====================================\n";
            cout << "(Tự động cập nhật mỗi 3 giây - nhấn Enter để thoát)\n";

            if (waitKey(3000)) break;
        }
    }
};

#endif
```

### presentation/MainUI.h

```cpp
#ifndef MAINUI_H
#define MAINUI_H

#include "StudentUI.h"
#include "DepartmentUI.h"
#include "CounterUI.h"
#include "DashboardUI.h"

class MainUI
{
private:
    AppContext& ctx;

    void dangNhap()
    {
        cout << "\n--- ĐĂNG NHẬP ---\n";
        string u = readLine("Username: ");
        string p = readLine("Password: ");

        TaiKhoan acc;
        if (!ctx.auth.login(u, p, acc))
        {
            cout << "Sai username hoặc password.\n";
            pauseScreen();
            return;
        }

        // Đa hình: điều hướng theo Role
        User* user = NULL;
        if (acc.getRole() == ROLE_STUDENT) user = new Student(acc, ctx);
        else if (acc.getRole() == ROLE_PHONG) user = new Department(acc, ctx);
        else if (acc.getRole() == ROLE_QUAY) user = new Counter(acc, ctx);

        if (user != NULL)
        {
            user->showMenu();
            delete user;
        }
        else
        {
            cout << "Vai trò không hợp lệ: " << acc.getRole() << endl;
            pauseScreen();
        }
    }

    // UC07: Guest lấy số thứ tự
    void layThuTu()
    {
        cout << "\n--- LẤY SỐ THỨ TỰ ---\n";
        string mssv = readLine("Nhập MSSV: ");
        PhieuHangDoi p;
        string msg;
        if (ctx.queue.takeTicket(mssv, p, msg))
        {
            cout << "\n+---------------------------+\n";
            cout << "|        VÉ HÀNG ĐỢI        |\n";
            cout << "+---------------------------+\n";
            cout << "  MSSV      : " << p.getMaSV() << endl;
            cout << "  Số thứ tự : " << p.getDisplay() << endl;
            cout << "  Quầy      : " << p.getMaQuay() << endl;
            cout << "+---------------------------+\n";
        }
        else
        {
            cout << msg << endl;
        }
        pauseScreen();
    }

public:
    MainUI(AppContext& c) : ctx(c) {}

    void run()
    {
        while (true)
        {
            clearScreen();
            cout << "====================================\n";
            cout << " STUDENT DOCUMENT MANAGEMENT SYSTEM\n";
            cout << "====================================\n\n";
            cout << "1. Đăng nhập\n";
            cout << "2. Lấy số thứ tự\n";
            cout << "3. Dashboard công khai\n";
            cout << "0. Thoát\n";
            int c = readInt("Chọn: ");
            if (c == 1) dangNhap();
            else if (c == 2) layThuTu();
            else if (c == 3) DashboardUI::run(ctx);
            else if (c == 0) return;
            else { cout << "Lựa chọn không hợp lệ.\n"; pauseScreen(); }
        }
    }
};

#endif
```

---

## 10. main.cpp

```cpp
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
```

---

## 11. Kịch bản test nhanh

1. Chạy `./app` → **2. Lấy số thứ tự** → nhập `2212345` → nhận vé `A001` gán vào quầy ít người nhất (Q01). Lấy thêm bằng MSSV `2212346` → `A002` được gán sang Q02 (chia tải).
2. **1. Đăng nhập** `sv001 / 123` → Đăng ký giấy tờ → chọn *Phòng CTSV* → *Giấy xác nhận sinh viên* → `Y` → hồ sơ `DK001` trạng thái PENDING.
3. Đăng nhập `ctsv001 / 123` → Duyệt hồ sơ → chọn `1` → Đồng ý → *Xuất danh sách in* (ra `data/PrintList.txt`) → *Cập nhật ngày in* (ví dụ hôm nay) → *Cập nhật ngày nhận* (ngày ≥ ngày in).
4. Mở terminal khác, chạy `./app` → **3. Dashboard công khai** để xem trạng thái các quầy.
5. Đăng nhập `q01 / 123` → *Gọi số tiếp theo* (dashboard cập nhật sau tối đa 3 giây) → *Xác nhận trả giấy* → nhập `DK001` (chỉ hợp lệ nếu hôm nay nằm trong [ngày nhận, ngày nhận + 30]) → hồ sơ chuyển `DONE`.
6. Đăng nhập lại `sv001` → *Xem hồ sơ đã đăng ký* để thấy trạng thái, ngày in, ngày nhận.

## 12. Ánh xạ yêu cầu học thuật

| Yêu cầu | Thể hiện trong code |
|---|---|
| Encapsulation | Thuộc tính `private`/`protected`, truy cập qua getter/setter |
| Inheritance | `Nguoi` → `SinhVien`, `Phong` (NhanVienPhong), `Quay` (NhanVienQuay) |
| Polymorphism / Abstraction | `User::showMenu()` thuần ảo → `Student`, `Department`, `Counter`; `Repository<T>` trừu tượng |
| 3-Layer | `presentation/` → `business/` → `data/` (UI không đụng file; Service không dùng `cout`) |
| Template tự xây | `MyList<T>`, `MyQueue<T>`, `Repository<T>`, `TxtRepository<T>` |
| Queue Management | `QueueService` dùng `MyQueue<PhieuHangDoi>` để lấy vé đầu hàng đợi của từng quầy |
| Repository Pattern | 7 repository dùng chung `TxtRepository<T>` |
| File TXT Database | `FileManager`, dữ liệu trong `data/*.txt` |
| Role-based access | `AuthService` + điều hướng theo `role` trong `MainUI::dangNhap()` |
| Dashboard realtime | `DashboardUI` reload mỗi 3 giây từ `CurrentServing.txt` |
| Cân bằng tải | `QueueService::takeTicket()` chọn quầy `active` có ít phiếu WAITING nhất |

## 13. Giới hạn và hướng mở rộng

- Đồng bộ nhiều tiến trình dựa trên việc đọc/ghi lại toàn bộ file (đủ cho đồ án); nếu hai người ghi cùng một file cùng lúc thì bản ghi cuối thắng. Có thể bổ sung khóa file nếu cần.
- Số thứ tự tăng liên tục, chưa reset theo ngày; có thể thêm trường ngày vào `PhieuHangDoi` để reset mỗi ngày.
- Mật khẩu lưu dạng văn bản thô theo đặc tả; có thể băm (hash) khi mở rộng.
- Muốn test UC12 ngoài khoảng ngày nhận, chỉ cần đặt ngày nhận trong phạm vi [hôm nay − 30, hôm nay] ở bước cập nhật ngày nhận.
