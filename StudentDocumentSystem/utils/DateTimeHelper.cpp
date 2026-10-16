#include "DateTimeHelper.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

using namespace std;
string DateTimeHelper::Now() const {
    if (!fixedNow.empty()) return fixedNow;
    time_t current = time(nullptr);
    tm local{};
    localtime_s(&local, &current);
    ostringstream output;
    output << put_time(&local, "%Y-%m-%d %H:%M:%S");
    return output.str();
}
bool DateTimeHelper::IsValid(const string& value) {
    if (value.size() != 19 || value[4] != '-' || value[7] != '-' || value[10] != ' '
        || value[13] != ':' || value[16] != ':') return false;
    for (int i = 0; i < 19; ++i) {
        if (i == 4 || i == 7 || i == 10 || i == 13 || i == 16) continue;
        if (value[i] < '0' || value[i] > '9') return false;
    }
    int year = stoi(value.substr(0, 4)), month = stoi(value.substr(5, 2));
    int day = stoi(value.substr(8, 2)), hour = stoi(value.substr(11, 2));
    int minute = stoi(value.substr(14, 2)), second = stoi(value.substr(17, 2));
    if (year < 1900 || month < 1 || month > 12 || hour > 23 || minute > 59 || second > 59) return false;
    int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    if (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)) days[1] = 29;
    return day >= 1 && day <= days[month - 1];
}
string DateTimeHelper::Date(const string& value) {
    if (!IsValid(value)) throw runtime_error("Ngay gio khong hop le: " + value);
    return value.substr(0, 10);
}
void DateTimeHelper::SetForTesting(const string& value) {
    if (!value.empty() && !IsValid(value)) throw runtime_error("Gio kiem tra khong hop le.");
    fixedNow = value;
}
