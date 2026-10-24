#ifndef UTILS_H
#define UTILS_H

#include <string>
#include "../templates/MyList.h"
using namespace std;

// ---------- Chuỗi ----------
MyList<string> split(const string& s, char delim);   // giữ các trường rỗng
string trim(const string& s);
string toUpper(string s);
string pad(int n, int width);                        // pad(5,3) -> "005"
int toInt(const string& s, int def = 0);            // sai định dạng -> def

// ---------- Ngày tháng (dd/mm/yyyy) / thời gian (dd/mm/yyyy HH:MM:SS) ----------
bool normalizeDate(string& s);                       // kiểm tra + chuẩn hóa
long dateToDays(const string& s);                    // số ngày từ 1/1/1970, -1 nếu sai
string today();                                      // dd/mm/yyyy
string nowDateTime();                                // dd/mm/yyyy HH:MM:SS (thời điểm hiện tại)
long long dateTimeToSeconds(const string& s);        // "dd/mm/yyyy[ HH:MM:SS]" -> giây từ 1/1/1970, -1 nếu sai

// ---------- Nhập xuất console ----------
string readLine(const string& prompt);
int readInt(const string& prompt);
void pauseScreen();
void clearScreen();
MyList<int> parseSelection(const string& line, int maxIdx);  // "1 2 4" hoặc "ALL" -> chỉ số 0-based
bool waitKey(int ms);                                // chờ ms; true nếu người dùng bấm Enter

#endif
