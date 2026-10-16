#include "ProcessingService.h"
#include "../utils/DataLock.h"
#include "../utils/FileHelper.h"
#include <stdexcept>

using namespace std;
ProcessingService::ProcessingService(ServiceContext& context) : context(context) {}
MyList<RequestInfo> ProcessingService::GetDepartmentRequests(const User& user) {
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    context.RequireUser(data, user, UserRole::PROCESSING_STAFF);
    MyList<RequestInfo> result;
    for (int i = 0; i < data.requests.Size(); ++i) {
        const Request& request = data.requests.GetByIndex(i);
        if (ServiceContext::RequestDepartment(data, request) == user.GetDepartment())
            result.Add(ServiceContext::Describe(data, request));
    }
    return result;
}
void ProcessingService::Update(const User& user, const string& requestId, Action action, const string& value) {
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    context.RequireUser(data, user, UserRole::PROCESSING_STAFF);
    Request& request = data.requests.GetByIndex(ServiceContext::FindRequest(data, requestId));
    if (ServiceContext::RequestDepartment(data, request) != user.GetDepartment()) throw runtime_error("Don thuoc phong khac.");
    string now = context.clock.Now();
    switch (action) {
    case Action::APPROVE: request.Approve(now); break;
    case Action::REJECT: FileHelper::ValidateText(value); request.Reject(value); break;
    case Action::PRINT: request.MarkPrinted(now); break;
    case Action::READY:
        if (!DateTimeHelper::IsValid(value) || value < now) throw runtime_error("Lich hen phai hop le va khong truoc hien tai.");
        request.SetReady(now, value); break;
    }
    context.requests.Save(data.requests);
}
void ProcessingService::ApproveRequest(const User& user, const string& id) { Update(user, id, Action::APPROVE, ""); }
void ProcessingService::RejectRequest(const User& user, const string& id, const string& reason) { Update(user, id, Action::REJECT, reason); }
void ProcessingService::MarkPrinted(const User& user, const string& id) { Update(user, id, Action::PRINT, ""); }
void ProcessingService::SetReadyForCollection(const User& user, const string& id, const string& appointment) {
    // Chuoi rong tu giao dien co nghia hen nhan ngay, lay gio chinh xac duoi khoa.
    if (!appointment.empty()) { Update(user, id, Action::READY, appointment); return; }
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    context.RequireUser(data, user, UserRole::PROCESSING_STAFF);
    Request& request = data.requests.GetByIndex(ServiceContext::FindRequest(data, id));
    if (ServiceContext::RequestDepartment(data, request) != user.GetDepartment()) throw runtime_error("Don thuoc phong khac.");
    string now = context.clock.Now();
    request.SetReady(now, now);
    context.requests.Save(data.requests);
}
