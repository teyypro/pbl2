#pragma once
#include "ServiceContext.h"
#include "../templates/MyQueue.h"

using namespace std;
struct DashboardSnapshot {
    string updatedAt;
    QueueSnapshot ctsv;
    QueueSnapshot daotao;
};
class QueueService {
private:
    ServiceContext& context;
public:
    explicit QueueService(ServiceContext& context);
    void Refresh();
    MyList<RequestInfo> GetEligibleRequests(const string& studentId, Department department);
    Ticket GenerateTicket(const string& studentId, const string& requestId, Department department);
    DashboardSnapshot GetDashboard();
    static MyQueue<int> BuildWaitingQueue(const SystemData& data, Department department, const string& date);
};
