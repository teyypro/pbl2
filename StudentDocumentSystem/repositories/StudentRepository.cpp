#include "StudentRepository.h"
#include "../utils/FileHelper.h"
#include "../utils/DateTimeHelper.h"
#include "../utils/PasswordHelper.h"
#include <limits>
#include <sstream>
#include <stdexcept>

using namespace std;

StudentRepository::StudentRepository(const string& directory) : path(directory + "/students.txt") {}
MyList<StudentRecord> StudentRepository::Load() const {
    MyList<StudentRecord> items;
    MyList<string> lines = FileHelper::ReadLines(path);
    for (int i = 0; i < lines.Size(); ++i) {
        try {
            MyList<string> f = FileHelper::Split(lines.GetByIndex(i), 4);
            FileHelper::ValidateText(f.GetByIndex(0), true);
            FileHelper::ValidateText(f.GetByIndex(1), true);
            FileHelper::ValidateText(f.GetByIndex(2), true);
            FileHelper::ValidateText(f.GetByIndex(3), true);
            StudentRecord item{f.GetByIndex(0), f.GetByIndex(1), f.GetByIndex(2), f.GetByIndex(3)};
            items.Add(item);
        } catch (const exception& error) {
            throw runtime_error(path + ": dong " + to_string(i + 1) + ": " + error.what());
        }
    }
    return items;
}
void StudentRepository::Save(const MyList<StudentRecord>& items) const {
    MyList<string> lines;
    for (int i = 0; i < items.Size(); ++i) {
        const StudentRecord& item = items.GetByIndex(i);
        ostringstream line;
        line << item.studentId << "|" << item.fullName << "|" << item.className << "|" << item.faculty;
        lines.Add(line.str());
    }
    FileHelper::WriteLines(path, lines);
}
