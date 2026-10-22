#include "Database.h"
#include <fstream>

Database::Database(const string& f) : folder(f) {}

bool Database::tableExists(const string& table) const
{
    ifstream f((folder + table + ".txt").c_str());
    return f.good();
}

MyList<string> Database::readLines(const string& fileName) const
{
    MyList<string> r;
    ifstream f((folder + fileName).c_str());
    if (!f) return r;                       // file chưa tồn tại -> danh sách rỗng

    string line;
    bool first = true;
    while (getline(f, line))
    {
        // bỏ BOM UTF-8 nếu file được lưu từ Notepad
        if (first && line.size() >= 3 &&
            (unsigned char)line[0] == 0xEF && (unsigned char)line[1] == 0xBB && (unsigned char)line[2] == 0xBF)
            line = line.substr(3);
        first = false;

        line = trim(line);
        if (!line.empty()) r.add(line);
    }
    return r;
}

bool Database::writeLines(const string& fileName, const MyList<string>& lines) const
{
    ofstream f((folder + fileName).c_str(), ios::trunc);
    if (!f) return false;
    for (int i = 0; i < lines.getSize(); i++) f << lines[i] << "\n";
    return true;
}

MyList<Row> Database::readTable(const string& table) const
{
    MyList<Row> rows;
    MyList<string> lines = readLines(table + ".txt");
    for (int i = 0; i < lines.getSize(); i++) rows.add(split(lines[i], '|'));
    return rows;
}

bool Database::writeTable(const string& table, const MyList<Row>& rows) const
{
    MyList<string> lines;
    for (int i = 0; i < rows.getSize(); i++)
    {
        string line;
        for (int j = 0; j < rows[i].getSize(); j++)
        {
            string v = rows[i][j];
            for (size_t k = 0; k < v.size(); k++)    // '|' là ký tự phân cách -> thay bằng '/'
                if (v[k] == '|') v[k] = '/';
            if (j > 0) line += "|";
            line += v;
        }
        lines.add(line);
    }
    return writeLines(table + ".txt", lines);
}