#include "DocumentRepository.h"
#include "../utils/FileHelper.h"
#include "../utils/DateTimeHelper.h"
#include "../utils/PasswordHelper.h"
#include <limits>
#include <sstream>
#include <stdexcept>

using namespace std;

DocumentRepository::DocumentRepository(const string& directory) : path(directory + "/documents.txt") {}
MyList<DocumentType> DocumentRepository::Load() const {
    MyList<DocumentType> items;
    MyList<string> lines = FileHelper::ReadLines(path);
    for (int i = 0; i < lines.Size(); ++i) {
        try {
            MyList<string> f = FileHelper::Split(lines.GetByIndex(i), 4);
            FileHelper::ValidateText(f.GetByIndex(0), true);
            FileHelper::ValidateText(f.GetByIndex(1), true);
            FileHelper::ValidateText(f.GetByIndex(2), true);
            FileHelper::ValidateText(f.GetByIndex(3), true);
            DocumentType item(f.GetByIndex(0), f.GetByIndex(1), ParseDepartment(f.GetByIndex(2)),
        FileHelper::Number(f.GetByIndex(3), 0, (numeric_limits<long long>::max)()));
    if (item.GetDepartment() == Department::NONE) throw runtime_error("Giay to phai thuoc mot phong.");
            items.Add(item);
        } catch (const exception& error) {
            throw runtime_error(path + ": dong " + to_string(i + 1) + ": " + error.what());
        }
    }
    return items;
}
void DocumentRepository::Save(const MyList<DocumentType>& items) const {
    MyList<string> lines;
    for (int i = 0; i < items.Size(); ++i) {
        const DocumentType& item = items.GetByIndex(i);
        ostringstream line;
        line << item.GetDocumentId() << "|" << item.GetDocumentName() << "|" << ToString(item.GetDepartment()) << "|" << item.GetFee();
        lines.Add(line.str());
    }
    FileHelper::WriteLines(path, lines);
}
