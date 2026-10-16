#include "InputHelper.h"
#include "FileHelper.h"
#include <iostream>
#include <stdexcept>

using namespace std;
string InputHelper::Text(const string& prompt, bool required) {
    while (true) {
        cout << prompt;
        string value;
        if (!getline(cin, value)) throw runtime_error("Da dong luong nhap.");
        try { FileHelper::ValidateText(value, required); return value; }
        catch (const exception& error) { cout << "Loi: " << error.what() << '\n'; }
    }
}
string InputHelper::Password(const string& prompt) {
    // Nhap bang getline thong thuong de code don gian va de kiem tra.
    return Text(prompt);
}
int InputHelper::Integer(const string& prompt, int minimum, int maximum) {
    while (true) {
        string value = Text(prompt);
        try { return static_cast<int>(FileHelper::Number(value, minimum, maximum)); }
        catch (const exception&) { cout << "Nhap so nguyen tu " << minimum << " den " << maximum << ".\n"; }
    }
}
bool InputHelper::Confirm(const string& prompt) {
    return Integer(prompt + " (1: Co, 0: Khong): ", 0, 1) == 1;
}
