#include "RequestService.h"
#include "../utils/DataLock.h"
#include "../utils/FileHelper.h"
#include <limits>
#include <stdexcept>

using namespace std;
RequestService::RequestService(ServiceContext& context) : context(context) {}
MyList<DocumentType> RequestService::GetDocuments() {
    DataLock lock(context.GetDirectory());
    return context.Load().documents;
}
Request RequestService::CreateRequest(const User& user, const string& documentId, int quantity, const string& note) {
    FileHelper::ValidateText(note, false);
    if (quantity < 1) throw runtime_error("So luong phai lon hon 0.");
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    context.RequireUser(data, user, UserRole::STUDENT);
    const DocumentType& document = data.documents.GetByIndex(ServiceContext::FindDocument(data, documentId));
    if (document.GetFee() > (numeric_limits<long long>::max)() / quantity)
        throw runtime_error("Tong phi vuot gioi han.");
    MyList<string> ids;
    for (int i = 0; i < data.requests.Size(); ++i) ids.Add(data.requests.GetByIndex(i).GetRequestId());
    Request request(FileHelper::NextId(ids, "RQ"), user.GetStudentId(), documentId, quantity,
        document.GetFee(), note, RequestStatus::PENDING, context.clock.Now(), "", "", "", "", "", "",
        document.GetFee() == 0 ? PaymentStatus::NOT_REQUIRED : PaymentStatus::UNPAID, "");
    data.requests.Add(request);
    context.requests.Save(data.requests);
    return request;
}
MyList<RequestInfo> RequestService::GetMyRequests(const User& user) {
    DataLock lock(context.GetDirectory());
    SystemData data = context.Load();
    context.RequireUser(data, user, UserRole::STUDENT);
    MyList<RequestInfo> result;
    for (int i = 0; i < data.requests.Size(); ++i) {
        const Request& request = data.requests.GetByIndex(i);
        if (request.GetStudentId() == user.GetStudentId()) result.Add(ServiceContext::Describe(data, request));
    }
    return result;
}
