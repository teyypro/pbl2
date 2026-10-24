#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "Entity.h"
using namespace std;

// Tầng thấp nhất của ORM: DUY NHẤT nơi mở file TXT.
// Mỗi "bảng" là một file  <folder>/<tên bảng>.txt
class Database
{
private:
    string folder;

public:
    Database(const string& folder);

    // ----- Làm việc theo bảng -----
    bool tableExists(const string& table) const;
    MyList<Row> readTable(const string& table) const;
    bool writeTable(const string& table, const MyList<Row>& rows) const;

    // ----- File thô (PrintList.txt, CurrentServing.txt...) -----
    MyList<string> readLines(const string& fileName) const;
    bool writeLines(const string& fileName, const MyList<string>& lines) const;
};

#endif
