#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <fstream>
#include "../utils/Utils.h"

const string DATA_DIR = "data/";

// Đọc / ghi file TXT theo dòng
class FileManager
{
public:
    static bool exists(const string& path);

    // Đọc tất cả dòng không rỗng (bỏ BOM UTF-8 nếu có). File không tồn tại -> danh sách rỗng
    static MyList<string> readLines(const string& path);

    static bool writeLines(const string& path, const MyList<string>& lines);
};

#endif
