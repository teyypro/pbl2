#pragma once
#include <string>

using namespace std;
class PasswordHelper {
public:
    static string GenerateSalt();
    static string Hash(const string& password, const string& salt);
    static bool Verify(const string& password, const string& salt, const string& hash);
    static bool IsHex(const string& value, int length);
};
