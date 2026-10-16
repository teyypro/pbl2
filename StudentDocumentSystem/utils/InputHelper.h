#pragma once
#include <string>

using namespace std;
class InputHelper {
public:
    static string Text(const string& prompt, bool required = true);
    static string Password(const string& prompt);
    static int Integer(const string& prompt, int minimum, int maximum);
    static bool Confirm(const string& prompt);
};
