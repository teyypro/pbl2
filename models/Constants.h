#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
using namespace std;

const string ROLE_STUDENT = "SINHVIEN";
const string ROLE_PHONG   = "PHONG";
const string ROLE_QUAY    = "QUAY";

const string ST_PENDING  = "PENDING";
const string ST_APPROVED = "APPROVED";
const string ST_REJECTED = "REJECTED";
const string ST_DONE     = "DONE";

// Yêu cầu của phiếu hàng đợi: "TUVAN" hoặc mã văn bản (VBxxx)
const string REQ_CONSULT = "TUVAN";

const string Q_WAITING  = "WAITING";
const string Q_SERVING  = "SERVING";
const string Q_FINISHED = "FINISHED";

#endif
