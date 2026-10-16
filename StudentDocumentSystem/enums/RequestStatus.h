#pragma once
#include <string>

using namespace std;

enum class RequestStatus { PENDING, APPROVED, REJECTED, PRINTED, READY_FOR_COLLECTION, COLLECTED };
string ToString(RequestStatus value);
RequestStatus ParseRequestStatus(const string& value);
