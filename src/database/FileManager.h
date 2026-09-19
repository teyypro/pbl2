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
    FileManager(const string& dir = "data") : dataDir(dir) {}

    // Lay duong dan day du cua file
    string getFilePath(const string& filename) {
        return dataDir + "/" + filename;
    }

    // Doc tat ca dong tu file (bo dong header)
    vector<string> readAllLines(const string& filename) {
        vector<string> lines;
        ifstream file(getFilePath(filename));
        if (!file.is_open()) {
            cerr << "Khong the mo file: " << getFilePath(filename) << endl;
            return lines;
        }

        string line;
        bool isHeader = true;
        while (getline(file, line)) {
            while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
                line.pop_back();
            }
            if (isHeader) { isHeader = false; continue; } // Bo dong header
            if (!line.empty()) {
                lines.push_back(line);
            }
        }
        file.close();
        return lines;
    }

    // Doc dong header
    string readHeader(const string& filename) {
        ifstream file(getFilePath(filename));
        string header;
        if (file.is_open()) {
            getline(file, header);
            while (!header.empty() && (header.back() == '\r' || header.back() == '\n')) {
                header.pop_back();
            }
            file.close();
        }
        return header;
    }

    // Ghi tat ca dong vao file (bao gom header)
    bool writeAllLines(const string& filename, const string& header, const vector<string>& lines) {
        ofstream file(getFilePath(filename));
        if (!file.is_open()) {
            cerr << "Khong the ghi file: " << getFilePath(filename) << endl;
            return false;
        }
        file << header << "\n";
        for (const auto& line : lines) {
            file << line << "\n";
        }
        file.close();
        return true;
    }

    // Them 1 dong vao cuoi file
    bool appendLine(const string& filename, const string& line) {
        ofstream file(getFilePath(filename), ios::app);
        if (!file.is_open()) {
            cerr << "Khong the ghi file: " << getFilePath(filename) << endl;
            return false;
        }
        file << line << "\n";
        file.close();
        return true;
    }

    // Tim ID lon nhat trong file -> tra ve ID + 1
    int getNextId(const string& filename) {
        vector<string> lines = readAllLines(filename);
        int maxId = 0;
        for (const auto& line : lines) {
            // ID la truong dau tien (truoc dau | dau tien)
            size_t pos = line.find('|');
            if (pos != string::npos) {
                try {
                    int id = stoi(line.substr(0, pos));
                    if (id > maxId) maxId = id;
                } catch (...) {}
            }
        }
        return maxId + 1;
    }

    // Tach 1 dong thanh cac truong theo dau |
    static vector<string> split(const string& line, char delimiter = '|') {
        vector<string> fields;
        stringstream ss(line);
        string field;
        while (getline(ss, field, delimiter)) {
            while (!field.empty() && (field.back() == '\r' || field.back() == '\n')) {
                field.pop_back();
            }
            fields.push_back(field);
        }
        return fields;
    }

    // Noi cac truong thanh 1 dong voi dau |
    static string join(const vector<string>& fields, char delimiter = '|') {
        string result;
        for (size_t i = 0; i < fields.size(); i++) {
            if (i > 0) result += delimiter;
            result += fields[i];
        }
        return result;
    }
};

#endif
