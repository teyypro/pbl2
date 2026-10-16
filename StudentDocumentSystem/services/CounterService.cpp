#include "CounterService.h"
#include "QueueService.h"
#include "../utils/DataLock.h"
#include <stdexcept>

using namespace std;
CounterService::CounterService(ServiceContext& context) : context(context) {}
MyList<CounterInfo> CounterService::GetTickets(const User& user) {
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    context.RequireUser(data, user, UserRole::COUNTER_STAFF);
    string now = context.clock.Now();
    context.SynchronizeQueues(data, now);
    MyList<CounterInfo> result;
    for (int i = 0; i < data.tickets.Size(); ++i) {
        const Ticket& t = data.tickets.GetByIndex(i);
        if (t.GetDepartment() == user.GetDepartment() && t.GetTicketDate() == DateTimeHelper::Date(now) && t.IsActive())
            result.Add({t, ServiceContext::Describe(data, data.requests.GetByIndex(ServiceContext::FindRequest(data, t.GetRequestId())))});
    }
    return result;
}
CounterInfo CounterService::CallNextTicket(const User& user) {
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    context.RequireUser(data, user, UserRole::COUNTER_STAFF);
    string now = context.clock.Now(), date = DateTimeHelper::Date(now);
    context.SynchronizeQueues(data, now);
    for (int i = 0; i < data.tickets.Size(); ++i) {
        const Ticket& t = data.tickets.GetByIndex(i);
        if (t.GetDepartment() == user.GetDepartment() && t.GetTicketDate() == date && t.GetStatus() == TicketStatus::SERVING)
            throw runtime_error("Hay hoan tat hoac bo luot ve dang phuc vu truoc.");
    }
    MyQueue<int> waiting = QueueService::BuildWaitingQueue(data, user.GetDepartment(), date);
    if (waiting.Empty()) throw runtime_error("Hang doi rong.");
    int index = waiting.Front(); waiting.Dequeue();
    Ticket& ticket = data.tickets.GetByIndex(index);
    ticket.Call(now);
    context.queues.SaveDepartment(data.tickets, user.GetDepartment());
    return {ticket, ServiceContext::Describe(data, data.requests.GetByIndex(ServiceContext::FindRequest(data, ticket.GetRequestId())))};
}
int CounterService::ServingIndex(SystemData& data, const User& user, const string& ticketId, const string& now) {
    context.RequireUser(data, user, UserRole::COUNTER_STAFF);
    context.SynchronizeQueues(data, now);
    int index = ServiceContext::FindTicket(data, ticketId);
    const Ticket& ticket = data.tickets.GetByIndex(index);
    if (ticket.GetDepartment() != user.GetDepartment() || ticket.GetTicketDate() != DateTimeHelper::Date(now)
        || ticket.GetStatus() != TicketStatus::SERVING) throw runtime_error("Ve khong dang phuc vu tai phong cua ban hoac da het han.");
    return index;
}
void CounterService::SkipCurrentTicket(const User& user, const string& ticketId) {
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    string now = context.clock.Now();
    data.tickets.GetByIndex(ServingIndex(data, user, ticketId, now)).Skip(now);
    context.queues.SaveDepartment(data.tickets, user.GetDepartment());
}
void CounterService::RecordPayment(const User& user, const string& ticketId, bool verified) {
    if (!verified) throw runtime_error("Can xac minh the sinh vien/CCCD truoc.");
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    string now = context.clock.Now();
    const Ticket& ticket = data.tickets.GetByIndex(ServingIndex(data, user, ticketId, now));
    Request& request = data.requests.GetByIndex(ServiceContext::FindRequest(data, ticket.GetRequestId()));
    request.RecordPayment(now);
    context.requests.Save(data.requests);
}
void CounterService::CompleteCollection(const User& user, const string& ticketId, bool verified) {
    if (!verified) throw runtime_error("Can xac minh the sinh vien/CCCD truoc.");
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    string now = context.clock.Now();
    Ticket& ticket = data.tickets.GetByIndex(ServingIndex(data, user, ticketId, now));
    Request& request = data.requests.GetByIndex(ServiceContext::FindRequest(data, ticket.GetRequestId()));
    request.Collect(now);
    // Don la can cu phuc hoi neu ghi file ve that bai.
    context.requests.Save(data.requests);
    ticket.Complete(now);
    try { context.queues.SaveDepartment(data.tickets, user.GetDepartment()); }
    catch (const exception&) {
        throw runtime_error("Don da luu COLLECTED; ve chua dong bo. Tai lai hang doi de phuc hoi, khong giao/thu tien lai.");
    }
}
