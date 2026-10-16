#include "UserRepository.h"
#include "../utils/FileHelper.h"
#include "../utils/DateTimeHelper.h"
#include "../utils/PasswordHelper.h"
#include <limits>
#include <sstream>
#include <stdexcept>

using namespace std;

UserRepository::UserRepository(const string& directory) : path(directory + "/users.txt") {}
MyList<AccountRecord> UserRepository::Load() const {
    MyList<AccountRecord> items;
    MyList<string> lines = FileHelper::ReadLines(path);
    for (int i = 0; i < lines.Size(); ++i) {
        try {
            MyList<string> f = FileHelper::Split(lines.GetByIndex(i), 7);
            FileHelper::ValidateText(f.GetByIndex(0), true);
            FileHelper::ValidateText(f.GetByIndex(1), true);
            FileHelper::ValidateText(f.GetByIndex(2), true);
            FileHelper::ValidateText(f.GetByIndex(3), true);
            FileHelper::ValidateText(f.GetByIndex(4), true);
            FileHelper::ValidateText(f.GetByIndex(5), true);
            FileHelper::ValidateText(f.GetByIndex(6), true);
            AccountRecord item{f.GetByIndex(0), f.GetByIndex(1), f.GetByIndex(2), f.GetByIndex(3),
        f.GetByIndex(4), ParseUserRole(f.GetByIndex(5)), ParseDepartment(f.GetByIndex(6))};
    if (!PasswordHelper::IsHex(item.passwordSalt, 32) || !PasswordHelper::IsHex(item.passwordHash, 64))
        throw runtime_error("Salt/hash khong hop le.");
    if ((item.role == UserRole::STUDENT) != (item.department == Department::NONE))
        throw runtime_error("Phong va vai tro khong phu hop.");
            items.Add(item);
        } catch (const exception& error) {
            throw runtime_error(path + ": dong " + to_string(i + 1) + ": " + error.what());
        }
    }
    return items;
}
void UserRepository::Save(const MyList<AccountRecord>& items) const {
    MyList<string> lines;
    for (int i = 0; i < items.Size(); ++i) {
        const AccountRecord& item = items.GetByIndex(i);
        ostringstream line;
        line << item.userId << "|" << item.username << "|" << item.passwordSalt << "|" << item.passwordHash << "|" << item.fullName << "|" << ToString(item.role) << "|" << ToString(item.department);
        lines.Add(line.str());
    }
    FileHelper::WriteLines(path, lines);
}
