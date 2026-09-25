#include "database/FileManager.h"

FileManager::FileManager(const string& dir) : dataDir(dir) {
    // Tu dong tim thu muc data (ho tro chay tu src/ hoac project root)
    ifstream test(dataDir + "/users.txt");
    if (!test.is_open()) {
        dataDir = "../" + dir;
    }
}

string FileManager::getFilePath(const string& filename) {
    return dataDir + "/" + filename;
}

vector<string> FileManager::readAllLines(const string& filename) {
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
        if (isHeader) { isHeader = false; continue; }
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    file.close();
    return lines;
}

string FileManager::readHeader(const string& filename) {
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

bool FileManager::writeAllLines(const string& filename, const string& header, const vector<string>& lines) {
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

bool FileManager::appendLine(const string& filename, const string& line) {
    ofstream file(getFilePath(filename), ios::app);
    if (!file.is_open()) {
        cerr << "Khong the ghi file: " << getFilePath(filename) << endl;
        return false;
    }
    file << line << "\n";
    file.close();
    return true;
}

int FileManager::getNextId(const string& filename) {
    vector<string> lines = readAllLines(filename);
    int maxId = 0;
    for (const auto& line : lines) {
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

vector<string> FileManager::split(const string& line, char delimiter) {
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

string FileManager::join(const vector<string>& fields, char delimiter) {
    string result;
    for (size_t i = 0; i < fields.size(); i++) {
        if (i > 0) result += delimiter;
        result += fields[i];
    }
    return result;
}
