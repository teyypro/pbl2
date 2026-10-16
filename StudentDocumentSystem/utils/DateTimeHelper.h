#pragma once
#include <string>

using namespace std;
class DateTimeHelper {
private:
    string fixedNow;
public:
    string Now() const;
    static bool IsValid(const string& value);
    static string Date(const string& value);
    // Chi dung trong chuong trinh kiem tra, khong co menu sua gio that.
    void SetForTesting(const string& value);
};
