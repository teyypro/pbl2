#pragma once
#include "../services/AuthService.h"
#include "../services/RequestService.h"
#include "../services/ProcessingService.h"
#include "../services/QueueService.h"
#include "../services/CounterService.h"

using namespace std;
class Application {
private:
    DateTimeHelper clock;
    ServiceContext context;
    AuthService auth;
    RequestService requests;
    ProcessingService processing;
    QueueService queue;
    CounterService counter;
    unique_ptr<User> session;
    void OpenSession();
public:
    explicit Application(const string& directory);
    void Run(bool dashboardOnly);
};
