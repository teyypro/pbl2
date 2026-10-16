#include "AuthService.h"
#include "../models/Student.h"
#include "../models/ProcessingStaff.h"
#include "../models/CounterStaff.h"
#include "../utils/DataLock.h"
#include "../utils/FileHelper.h"
#include "../utils/PasswordHelper.h"
#include <stdexcept>

using namespace std;
AuthService::AuthService(ServiceContext& context) : context(context) {}
void AuthService::RegisterStudent(const string& studentId, const string& password, const string& confirmation) {
    FileHelper::ValidateText(studentId); FileHelper::ValidateText(password);
    if (password != confirmation) throw runtime_error("Xac nhan mat khau khong khop.");
    if (password.size() < 6 || password.size() > 128) throw runtime_error("Mat khau can 6-128 byte.");
    // Bam mat khau truoc khi giu khoa de cac terminal khac khong phai cho.
    string salt = PasswordHelper::GenerateSalt();
    string hash = PasswordHelper::Hash(password, salt);
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    const StudentRecord& student = data.students.GetByIndex(ServiceContext::FindStudent(data, studentId));
    MyList<string> ids;
    for (int i = 0; i < data.accounts.Size(); ++i) {
        const AccountRecord& account = data.accounts.GetByIndex(i);
        if (account.username == studentId) throw runtime_error("MSSV da co tai khoan.");
        ids.Add(account.userId);
    }
    data.accounts.Add({FileHelper::NextId(ids, "U"), studentId, salt, hash, student.fullName,
        UserRole::STUDENT, Department::NONE});
    context.users.Save(data.accounts);
}
unique_ptr<User> AuthService::Login(const string& username, const string& password) {
    FileHelper::ValidateText(username); FileHelper::ValidateText(password);
    if (password.size() > 128) throw runtime_error("Sai tai khoan hoac mat khau.");
    AccountRecord account;
    StudentRecord student;
    bool found = false;
    {
        DataLock lock(context.GetDirectory());
        SystemData data = context.Load();
        for (int i = 0; i < data.accounts.Size(); ++i) {
            if (data.accounts.GetByIndex(i).username != username) continue;
            account = data.accounts.GetByIndex(i); found = true;
            if (account.role == UserRole::STUDENT)
                student = data.students.GetByIndex(ServiceContext::FindStudent(data, username));
            break;
        }
    }
    if (!found || !PasswordHelper::Verify(password, account.passwordSalt, account.passwordHash))
        throw runtime_error("Sai tai khoan hoac mat khau.");
    // unique_ptr so huu User, huy dung lop con nho destructor virtual.
    if (account.role == UserRole::STUDENT)
        return make_unique<Student>(account.userId, student.studentId, student.fullName, student.className, student.faculty);
    if (account.role == UserRole::PROCESSING_STAFF)
        return make_unique<ProcessingStaff>(account.userId, account.username, account.fullName, account.department);
    return make_unique<CounterStaff>(account.userId, account.username, account.fullName, account.department);
}
void AuthService::Logout(unique_ptr<User>& session) { session.reset(); }
