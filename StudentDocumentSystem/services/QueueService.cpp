#include "QueueService.h"
#include "../utils/DataLock.h"
#include "../utils/FileHelper.h"
#include <limits>
#include <stdexcept>

using namespace std;
QueueService::QueueService(ServiceContext& context) : context(context) {}
void QueueService::Refresh() {
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    context.SynchronizeQueues(data, context.clock.Now());
}
MyList<RequestInfo> QueueService::GetEligibleRequests(const string& studentId, Department department) {
    ServiceContext::CheckDepartment(department);
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    string now = context.clock.Now();
    context.SynchronizeQueues(data, now);
    ServiceContext::FindStudent(data, studentId);
    MyList<RequestInfo> result;
    for (int i = 0; i < data.requests.Size(); ++i) {
        const Request& r = data.requests.GetByIndex(i);
        if (r.GetStudentId() != studentId || r.GetStatus() != RequestStatus::READY_FOR_COLLECTION
            || r.GetAppointmentAt() > now || ServiceContext::RequestDepartment(data, r) != department) continue;
        bool active = false;
        for (int j = 0; j < data.tickets.Size(); ++j)
            if (data.tickets.GetByIndex(j).GetRequestId() == r.GetRequestId() && data.tickets.GetByIndex(j).IsActive()) active = true;
        if (!active) result.Add(ServiceContext::Describe(data, r));
    }
    return result;
}
Ticket QueueService::GenerateTicket(const string& studentId, const string& requestId, Department department) {
    ServiceContext::CheckDepartment(department);
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    string now = context.clock.Now(), date = DateTimeHelper::Date(now);
    context.SynchronizeQueues(data, now);
    ServiceContext::FindStudent(data, studentId);
    const Request& r = data.requests.GetByIndex(ServiceContext::FindRequest(data, requestId));
    if (r.GetStudentId() != studentId || r.GetStatus() != RequestStatus::READY_FOR_COLLECTION
        || r.GetAppointmentAt() > now || ServiceContext::RequestDepartment(data, r) != department)
        throw runtime_error("Don chua du dieu kien lay so.");
    long long largest = 0;
    for (int i = 0; i < data.tickets.Size(); ++i) {
        const Ticket& t = data.tickets.GetByIndex(i);
        if (t.GetRequestId() == requestId && t.IsActive()) throw runtime_error("Don da co ve dang hoat dong: " + t.GetDisplayNumber());
        if (t.GetDepartment() == department && t.GetTicketDate() == date && t.GetSequenceNumber() > largest) largest = t.GetSequenceNumber();
    }
    if (largest >= (numeric_limits<long long>::max)() - 1) throw runtime_error("So thu tu vuot gioi han kieu so.");
    Ticket preview("", date, department, largest + 1, studentId, requestId, TicketStatus::WAITING, now, "", "");
    string dateKey = date; dateKey.erase(7, 1); dateKey.erase(4, 1);
    Ticket ticket(dateKey + "-" + preview.GetDisplayNumber(), date, department, largest + 1,
        studentId, requestId, TicketStatus::WAITING, now, "", "");
    data.tickets.Add(ticket);
    context.queues.SaveDepartment(data.tickets, department);
    return ticket;
}
MyQueue<int> QueueService::BuildWaitingQueue(const SystemData& data, Department department, const string& date) {
    MyQueue<int> result;
    long long previous = 0;
    while (true) {
        int best = -1;
        for (int i = 0; i < data.tickets.Size(); ++i) {
            const Ticket& t = data.tickets.GetByIndex(i);
            if (t.GetDepartment() != department || t.GetTicketDate() != date || t.GetStatus() != TicketStatus::WAITING
                || t.GetSequenceNumber() <= previous) continue;
            if (best < 0 || t.GetSequenceNumber() < data.tickets.GetByIndex(best).GetSequenceNumber()) best = i;
        }
        if (best < 0) break;
        result.Enqueue(best);
        previous = data.tickets.GetByIndex(best).GetSequenceNumber();
    }
    return result;
}
DashboardSnapshot QueueService::GetDashboard() {
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    DashboardSnapshot result;
    result.updatedAt = context.clock.Now();
    context.SynchronizeQueues(data, result.updatedAt);
    Department departments[] = {Department::CTSV, Department::DAOTAO};
    for (Department department : departments) {
        QueueSnapshot& snapshot = department == Department::CTSV ? result.ctsv : result.daotao;
        for (int i = 0; i < data.tickets.Size(); ++i) {
            const Ticket& t = data.tickets.GetByIndex(i);
            if (t.GetDepartment() == department && t.GetTicketDate() == DateTimeHelper::Date(result.updatedAt)
                && t.GetStatus() == TicketStatus::SERVING) snapshot.current = t.GetDisplayNumber();
        }
        MyQueue<int> waiting = BuildWaitingQueue(data, department, DateTimeHelper::Date(result.updatedAt));
        while (!waiting.Empty()) {
            snapshot.waiting.Add(data.tickets.GetByIndex(waiting.Front()).GetDisplayNumber());
            waiting.Dequeue();
        }
    }
    return result;
}
