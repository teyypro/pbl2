#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

class FileManager {
private:
    string dataDir;

public:
    FileManager(const string& dir = "data");

    // Lay duong dan day du cua file
    string getFilePath(const string& filename);

    // Doc tat ca dong tu file (bo dong header)
    vector<string> readAllLines(const string& filename);

    // Doc dong header
    string readHeader(const string& filename);

    // Ghi tat ca dong vao file (bao gom header)
    bool writeAllLines(const string& filename, const string& header, const vector<string>& lines);

    // Them 1 dong vao cuoi file
    bool appendLine(const string& filename, const string& line);

    // Tim ID lon nhat trong file -> tra ve ID + 1
    int getNextId(const string& filename);

    // Tach 1 dong thanh cac truong theo dau |
    static vector<string> split(const string& line, char delimiter = '|');

    // Noi cac truong thanh 1 dong voi dau |
    static string join(const vector<string>& fields, char delimiter = '|');
};

#endif
