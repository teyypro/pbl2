#pragma once
#include "../services/ServiceContext.h"

using namespace std;
class ViewHelper {
public:
    static void Documents(const MyList<DocumentType>& documents);
    static void Requests(const MyList<RequestInfo>& requests);
    static void RequestDetails(const RequestInfo& info);
    static void CounterDetails(const CounterInfo& info);
    static Department ChooseDepartment();
    static void Error(const exception& error);
};
