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
MyList<string> split(const string& s, char delim);

string trim(const string& s);

string toUpper(string s);

// Đệm số 0 phía trước: pad(5,3) -> "005"
string pad(int n, int width);

// Chuyển chuỗi toàn chữ số thành int, sai định dạng trả về def
int toInt(const string& s, int def = 0);

// ---------- Ngày tháng (dd/mm/yyyy) ----------
long daysFromCivil(int y, int m, int d);

bool isValidDate(int d, int m, int y);

// Kiểm tra và chuẩn hóa về dd/mm/yyyy
bool normalizeDate(string& s);

// Số ngày kể từ 1/1/1970; trả về -1 nếu chuỗi không hợp lệ
long dateToDays(const string& s);

string today();

// ---------- Nhập xuất ----------
string readLine(const string& prompt);

int readInt(const string& prompt);

void pauseScreen();

void clearScreen();

// Phân tích danh sách chọn "1 2 4" hoặc "ALL" -> danh sách chỉ số (0-based)
MyList<int> parseSelection(const string& line, int maxIdx);

// Chờ tối đa ms mili-giây; trả về true nếu người dùng bấm phím (Enter) để thoát
#ifdef _WIN32
bool waitKey(int ms);
#else
bool waitKey(int ms);
#endif

#endif
