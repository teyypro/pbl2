#include "RequestStatus.h"
#include <stdexcept>

using namespace std;

string ToString(RequestStatus value) {
    switch (value) {
    case RequestStatus::PENDING: return "PENDING";
    case RequestStatus::APPROVED: return "APPROVED";
    case RequestStatus::REJECTED: return "REJECTED";
    case RequestStatus::PRINTED: return "PRINTED";
    case RequestStatus::READY_FOR_COLLECTION: return "READY_FOR_COLLECTION";
    case RequestStatus::COLLECTED: return "COLLECTED";
    }
    throw runtime_error("Gia tri enum khong hop le.");
}
RequestStatus ParseRequestStatus(const string& value) {
    if (value == "PENDING") return RequestStatus::PENDING;
    if (value == "APPROVED") return RequestStatus::APPROVED;
    if (value == "REJECTED") return RequestStatus::REJECTED;
    if (value == "PRINTED") return RequestStatus::PRINTED;
    if (value == "READY_FOR_COLLECTION") return RequestStatus::READY_FOR_COLLECTION;
    if (value == "COLLECTED") return RequestStatus::COLLECTED;
    throw runtime_error("RequestStatus khong hop le: " + value);
}
