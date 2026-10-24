#include "Utils.h"
#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <sys/select.h>
#include <unistd.h>
#endif

namespace
{
    long daysFromCivil(int y, int m, int d)
    {
        y -= (m <= 2);
        long era = (y >= 0 ? y : y - 399) / 400;
        unsigned yoe = (unsigned)(y - era * 400);
        unsigned doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
        unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
        return era * 146097 + (long)doe - 719468;
    }

    bool isValidDate(int d, int m, int y)
    {
        if (m < 1 || m > 12 || d < 1 || y < 2000 || y > 2100) return false;
        int dm[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        bool leap = (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
        return d <= dm[m - 1] + ((m == 2 && leap) ? 1 : 0);
    }
}

// ---------- Chuỗi ----------
MyList<string> split(const string& s, char delim)
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

string trim(const string& s)
{
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

string toUpper(string s)
{
    for (size_t i = 0; i < s.size(); i++)
        if (s[i] >= 'a' && s[i] <= 'z') s[i] -= 32;
    return s;
}

string pad(int n, int width)
{
    string s = to_string(n);
    while ((int)s.size() < width) s = "0" + s;
    return s;
}

int toInt(const string& s, int def)
{
    if (s.empty()) return def;
    for (size_t i = 0; i < s.size(); i++)
        if (s[i] < '0' || s[i] > '9') return def;
    return atoi(s.c_str());
}

// ---------- Ngày tháng ----------
bool normalizeDate(string& s)
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

long dateToDays(const string& s)
{
    int d, m, y;
    if (sscanf(s.c_str(), "%d/%d/%d", &d, &m, &y) != 3) return -1;
    return daysFromCivil(y, m, d);
}

string today()
{
    time_t t = time(0);
    tm* l = localtime(&t);
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d", l->tm_mday, l->tm_mon + 1, l->tm_year + 1900);
    return string(buf);
}

string nowDateTime()
{
    time_t t = time(0);
    tm* l = localtime(&t);
    char buf[32];
    snprintf(buf, sizeof(buf), "%02d/%02d/%04d %02d:%02d:%02d",
             l->tm_mday, l->tm_mon + 1, l->tm_year + 1900, l->tm_hour, l->tm_min, l->tm_sec);
    return string(buf);
}

// Chấp nhận cả "dd/mm/yyyy" (giờ = 00:00:00) lẫn "dd/mm/yyyy HH:MM:SS"
long long dateTimeToSeconds(const string& s)
{
    int d, m, y, h = 0, mi = 0, sec = 0;
    int n = sscanf(s.c_str(), "%d/%d/%d %d:%d:%d", &d, &m, &y, &h, &mi, &sec);
    if (n < 3) return -1;
    return (long long)daysFromCivil(y, m, d) * 86400LL + h * 3600 + mi * 60 + sec;
}

// ---------- Nhập xuất ----------
string readLine(const string& prompt)
{
    cout << prompt;
    string s;
    if (!getline(cin, s)) { cout << endl; exit(0); }
    return trim(s);
}

int readInt(const string& prompt)
{
    while (true)
    {
        string s = readLine(prompt);
        if (toInt(s, -1) != -1) return atoi(s.c_str());
        cout << "Vui lòng nhập một số nguyên không âm.\n";
    }
}

void pauseScreen() { readLine("\nNhấn Enter để tiếp tục..."); }

void clearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

MyList<int> parseSelection(const string& line, int maxIdx)
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

#ifdef _WIN32
bool waitKey(int ms)
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
bool waitKey(int ms)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0)
    {
        string tmp;
        getline(cin, tmp);
        return true;
    }
    return false;
}
#endif
