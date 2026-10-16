#pragma once
#include <string>
#include "../templates/MyList.h"

using namespace std;
class FileHelper {
public:
    static MyList<string> ReadLines(const string& path);
    static void WriteLines(const string& path, const MyList<string>& lines);
    static MyList<string> Split(const string& line, int expected);
    static void ValidateText(const string& value, bool required = true);
    static long long Number(const string& value, long long minimum, long long maximum);
    static string NextId(const MyList<string>& ids, const string& prefix);
    static string DefaultDataDirectory();
};
