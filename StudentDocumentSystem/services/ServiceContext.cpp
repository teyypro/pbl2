#include "ServiceContext.h"
#include "../utils/FileHelper.h"
#include <stdexcept>

using namespace std;
namespace {
    void Unique(MyList<string>& keys, const string& key) {
        if (keys.Find(key) >= 0) throw runtime_error("Du lieu trung khoa: " + key);
        keys.Add(key);
    }
}
ServiceContext::ServiceContext(const string& directory, DateTimeHelper& clock)
    : directory(directory), requestRepository(directory), users(directory), students(directory), documents(directory),
      requests(requestRepository), queues(directory), clock(clock) {}
const string& ServiceContext::GetDirectory() const { return directory; }
int ServiceContext::FindStudent(const SystemData& data, const string& id) {
    for (int i = 0; i < data.students.Size(); ++i) if (data.students.GetByIndex(i).studentId == id) return i;
    throw runtime_error("MSSV khong ton tai: " + id);
}
int ServiceContext::FindDocument(const SystemData& data, const string& id) {
    for (int i = 0; i < data.documents.Size(); ++i) if (data.documents.GetByIndex(i).GetDocumentId() == id) return i;
    throw runtime_error("Ma giay to khong ton tai: " + id);
}
int ServiceContext::FindRequest(const SystemData& data, const string& id) {
    for (int i = 0; i < data.requests.Size(); ++i) if (data.requests.GetByIndex(i).GetRequestId() == id) return i;
    throw runtime_error("Ma don khong ton tai: " + id);
}
int ServiceContext::FindTicket(const SystemData& data, const string& id) {
    for (int i = 0; i < data.tickets.Size(); ++i) if (data.tickets.GetByIndex(i).GetTicketId() == id) return i;
    throw runtime_error("Ma ve khong ton tai: " + id);
}
Department ServiceContext::RequestDepartment(const SystemData& data, const Request& request) {
    return data.documents.GetByIndex(FindDocument(data, request.GetDocumentId())).GetDepartment();
}
RequestInfo ServiceContext::Describe(const SystemData& data, const Request& request) {
    return {request, data.documents.GetByIndex(FindDocument(data, request.GetDocumentId())).GetDocumentName(),
        data.students.GetByIndex(FindStudent(data, request.GetStudentId())).fullName};
}
void ServiceContext::CheckDepartment(Department department) {
    if (department != Department::CTSV && department != Department::DAOTAO)
        throw runtime_error("Phong khong hop le.");
}
SystemData ServiceContext::Load() const {
    SystemData data{users.Load(), students.Load(), documents.Load(), requests.Load(), queues.Load()};
    MyList<string> ids, usernames;
    for (int i = 0; i < data.students.Size(); ++i) Unique(ids, data.students.GetByIndex(i).studentId);
    ids.Clear();
    for (int i = 0; i < data.accounts.Size(); ++i) {
        const AccountRecord& a = data.accounts.GetByIndex(i);
        Unique(ids, a.userId); Unique(usernames, a.username);
        if (a.role == UserRole::STUDENT) FindStudent(data, a.username);
    }
    FileHelper::NextId(ids, "U");
    ids.Clear();
    for (int i = 0; i < data.documents.Size(); ++i) Unique(ids, data.documents.GetByIndex(i).GetDocumentId());
    ids.Clear();
    for (int i = 0; i < data.requests.Size(); ++i) {
        const Request& r = data.requests.GetByIndex(i);
        Unique(ids, r.GetRequestId()); FindStudent(data, r.GetStudentId()); FindDocument(data, r.GetDocumentId());
        bool accountFound = false;
        for (int j = 0; j < data.accounts.Size(); ++j)
            if (data.accounts.GetByIndex(j).role == UserRole::STUDENT && data.accounts.GetByIndex(j).username == r.GetStudentId()) accountFound = true;
        if (!accountFound) throw runtime_error("Don tham chieu sinh vien chua co tai khoan.");
    }
    FileHelper::NextId(ids, "RQ");
    ids.Clear();
    MyList<string> activeRequests, servingDepartments;
    for (int i = 0; i < data.tickets.Size(); ++i) {
        const Ticket& t = data.tickets.GetByIndex(i);
        Unique(ids, t.GetTicketId());
        const Request& r = data.requests.GetByIndex(FindRequest(data, t.GetRequestId()));
        if (r.GetStudentId() != t.GetStudentId() || RequestDepartment(data, r) != t.GetDepartment())
            throw runtime_error("Ve va don khong khop sinh vien/phong.");
        if (r.GetAppointmentAt().empty() || t.GetIssuedAt() < r.GetAppointmentAt())
            throw runtime_error("Ve duoc cap truoc lich hen.");
        if (t.IsActive()) {
            Unique(activeRequests, t.GetTicketDate() + "|" + t.GetRequestId());
            if (r.GetStatus() != RequestStatus::READY_FOR_COLLECTION && r.GetStatus() != RequestStatus::COLLECTED)
                throw runtime_error("Ve dang hoat dong nhung don khong san sang.");
            if (t.GetStatus() == TicketStatus::SERVING)
                Unique(servingDepartments, t.GetTicketDate() + "|" + ToString(t.GetDepartment()));
        }
        if (t.GetStatus() == TicketStatus::COMPLETED && r.GetStatus() != RequestStatus::COLLECTED)
            throw runtime_error("Ve hoan tat nhung don chua giao.");
    }
    return data;
}
void ServiceContext::RequireUser(const SystemData& data, const User& user, UserRole role) const {
    if (user.GetRole() != role) throw runtime_error("Ban khong co quyen thuc hien.");
    for (int i = 0; i < data.accounts.Size(); ++i) {
        const AccountRecord& a = data.accounts.GetByIndex(i);
        if (a.userId == user.GetUserId() && a.username == user.GetUsername() && a.role == role
            && a.department == user.GetDepartment()) {
            if (role == UserRole::STUDENT && user.GetStudentId() != a.username) break;
            return;
        }
    }
    throw runtime_error("Phien dang nhap khong hop le.");
}
void ServiceContext::SynchronizeQueues(SystemData& data, const string& now) const {
    bool ctsvChanged = false, daotaoChanged = false;
    for (int i = 0; i < data.tickets.Size(); ++i) {
        Ticket& ticket = data.tickets.GetByIndex(i);
        if (!ticket.IsActive()) continue;
        const Request& request = data.requests.GetByIndex(FindRequest(data, ticket.GetRequestId()));
        if (request.GetStatus() == RequestStatus::COLLECTED) ticket.Complete(request.GetCollectedAt());
        else if (ticket.GetTicketDate() < DateTimeHelper::Date(now)) ticket.Expire(now);
        else continue;
        if (ticket.GetDepartment() == Department::CTSV) ctsvChanged = true;
        else daotaoChanged = true;
    }
    if (ctsvChanged) queues.SaveDepartment(data.tickets, Department::CTSV);
    if (daotaoChanged) queues.SaveDepartment(data.tickets, Department::DAOTAO);
}
