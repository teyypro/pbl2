#include "FileManager.h"

bool FileManager::exists(const string& path)
{
    ifstream f(path.c_str());
    return f.good();
}

MyList<string> FileManager::readLines(const string& path)
{
    MyList<string> r;
    ifstream f(path.c_str());
    if (!f) return r;
    string line;
    bool first = true;
    while (getline(f, line))
    {
        if (first && line.size() >= 3 &&
            (unsigned char)line[0] == 0xEF && (unsigned char)line[1] == 0xBB && (unsigned char)line[2] == 0xBF)
            line = line.substr(3);
        first = false;
        line = trim(line);
        if (!line.empty()) r.add(line);
    }
    return r;
}

bool FileManager::writeLines(const string& path, const MyList<string>& lines)
{
    ofstream f(path.c_str(), ios::trunc);
    if (!f) return false;
    for (int i = 0; i < lines.getSize(); i++) f << lines[i] << "\n";
    return true;
}
