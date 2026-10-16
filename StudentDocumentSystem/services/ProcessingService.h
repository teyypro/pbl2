#pragma once
#include "ServiceContext.h"

using namespace std;
class ProcessingService {
private:
    ServiceContext& context;
    enum class Action { APPROVE, REJECT, PRINT, READY };
    void Update(const User& user, const string& requestId, Action action, const string& value);
public:
    explicit ProcessingService(ServiceContext& context);
    MyList<RequestInfo> GetDepartmentRequests(const User& user);
    void ApproveRequest(const User& user, const string& requestId);
    void RejectRequest(const User& user, const string& requestId, const string& reason);
    void MarkPrinted(const User& user, const string& requestId);
    void SetReadyForCollection(const User& user, const string& requestId, const string& appointment);
};
