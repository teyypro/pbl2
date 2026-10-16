#pragma once
#include "ServiceContext.h"

using namespace std;
class RequestService {
private:
    ServiceContext& context;
public:
    explicit RequestService(ServiceContext& context);
    MyList<DocumentType> GetDocuments();
    Request CreateRequest(const User& user, const string& documentId, int quantity, const string& note);
    MyList<RequestInfo> GetMyRequests(const User& user);
};
