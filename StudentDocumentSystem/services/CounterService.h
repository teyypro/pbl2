#pragma once
#include "ServiceContext.h"

using namespace std;
class CounterService {
private:
    ServiceContext& context;
    int ServingIndex(SystemData& data, const User& user, const string& ticketId, const string& now);
public:
    explicit CounterService(ServiceContext& context);
    MyList<CounterInfo> GetTickets(const User& user);
    CounterInfo CallNextTicket(const User& user);
    void SkipCurrentTicket(const User& user, const string& ticketId);
    void RecordPayment(const User& user, const string& ticketId, bool verified);
    void CompleteCollection(const User& user, const string& ticketId, bool verified);
};
