#include "FileHelper.h"
#include <cstdio>
#include <cerrno>
#include <io.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <limits>
#include <stdexcept>

using namespace std;
MyList<string> FileHelper::ReadLines(const string& path) {
    // Neu lan ghi truoc bi ngat giua hai lenh rename, khoi phuc ban goc.
    if (_access(path.c_str(), 0) != 0 && _access((path + ".bak").c_str(), 0) == 0) {
        if (rename((path + ".bak").c_str(), path.c_str()) != 0)
            throw runtime_error("Khong phuc hoi duoc file: " + path);
    }
    ifstream input(path, ios::binary);
    if (!input) throw runtime_error("Khong mo duoc file: " + path);
    MyList<string> lines;
    string line;
    while (getline(input, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (lines.Empty() && line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
        if (line.empty()) throw runtime_error("Dong rong trong file: " + path);
        lines.Add(line);
    }
    if (input.bad()) throw runtime_error("Loi doc file: " + path);
    return lines;
}
void FileHelper::WriteLines(const string& path, const MyList<string>& lines) {
    string temporary = path + ".tmp";
    string backup = path + ".bak";
    ofstream output(temporary, ios::binary | ios::trunc);
    if (!output) throw runtime_error("Khong tao duoc file tam: " + path);
    for (int i = 0; i < lines.Size(); ++i) output << lines.GetByIndex(i) << '\n';
    output.flush();
    if (!output) throw runtime_error("Ghi file that bai: " + path);
    output.close();
    if (!output) throw runtime_error("Dong file that bai: " + path);
    if (remove(backup.c_str()) != 0 && errno != ENOENT)
        throw runtime_error("Khong don duoc ban sao cu: " + backup);
    if (rename(path.c_str(), backup.c_str()) != 0)
        throw runtime_error("Khong tao duoc ban sao: " + path);
    if (rename(temporary.c_str(), path.c_str()) != 0) {
        rename(backup.c_str(), path.c_str());
        throw runtime_error("Khong thay the duoc file; kiem tra ban .bak: " + path);
    }
    remove(backup.c_str());
}
MyList<string> FileHelper::Split(const string& line, int expected) {
    MyList<string> fields;
    size_t start = 0;
    while (true) {
        size_t end = line.find('|', start);
        fields.Add(line.substr(start, end == string::npos ? end : end - start));
        if (end == string::npos) break;
        start = end + 1;
    }
    if (fields.Size() != expected) throw runtime_error("Sai so truong du lieu.");
    return fields;
}
void FileHelper::ValidateText(const string& value, bool required) {
    if (value.size() > 2000) throw runtime_error("Noi dung qua dai (toi da 2000 byte).");
    bool hasText = false;
    for (unsigned char c : value) {
        if (c == '|' || c < 32 || c == 127) throw runtime_error("Khong chap nhan ky tu | hoac ky tu dieu khien.");
        if (c != ' ') hasText = true;
    }
    if (required && !hasText) throw runtime_error("Noi dung khong duoc rong.");
}
long long FileHelper::Number(const string& value, long long minimum, long long maximum) {
    if (value.empty()) throw runtime_error("Thieu so.");
    for (char c : value) if (c < '0' || c > '9') throw runtime_error("So phai gom cac chu so 0-9.");
    size_t used = 0;
    long long result = stoll(value, &used);
    if (used != value.size() || result < minimum || result > maximum) throw runtime_error("So ngoai pham vi.");
    return result;
}
string FileHelper::NextId(const MyList<string>& ids, const string& prefix) {
    long long largest = 0;
    for (int i = 0; i < ids.Size(); ++i) {
        const string& id = ids.GetByIndex(i);
        if (id.compare(0, prefix.size(), prefix) != 0) throw runtime_error("Ma du lieu sai tien to.");
        long long number = Number(id.substr(prefix.size()), 1, (numeric_limits<long long>::max)() - 1);
        if (number > largest) largest = number;
    }
    ostringstream output;
    output << prefix << setw(3) << setfill('0') << largest + 1;
    return output.str();
}
string FileHelper::DefaultDataDirectory() {
    const char* candidates[] = {"data", "StudentDocumentSystem/data", "../data"};
    for (const char* directory : candidates) {
        ifstream input(string(directory) + "/students.txt");
        if (input) return directory;
    }
    throw runtime_error("Khong tim thay data. Hay truyen --data duong_dan.");
}
