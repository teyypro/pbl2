#include "Request.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>

using namespace std;
Request::Request(const string& requestId, const string& studentId, const string& documentId, int quantity, long long unitFee, const string& note, RequestStatus status, const string& requestedAt, const string& approvedAt, const string& printedAt, const string& readyAt, const string& appointmentAt, const string& collectedAt, const string& rejectReason, PaymentStatus paymentStatus, const string& paidAt)
    : requestId(requestId), studentId(studentId), documentId(documentId), quantity(quantity), unitFee(unitFee), note(note), status(status), requestedAt(requestedAt), approvedAt(approvedAt), printedAt(printedAt), readyAt(readyAt), appointmentAt(appointmentAt), collectedAt(collectedAt), rejectReason(rejectReason), paymentStatus(paymentStatus), paidAt(paidAt) {}
const string& Request::GetRequestId() const { return requestId; }
const string& Request::GetStudentId() const { return studentId; }
const string& Request::GetDocumentId() const { return documentId; }
int Request::GetQuantity() const { return quantity; }
long long Request::GetUnitFee() const { return unitFee; }
const string& Request::GetNote() const { return note; }
RequestStatus Request::GetStatus() const { return status; }
const string& Request::GetRequestedAt() const { return requestedAt; }
const string& Request::GetApprovedAt() const { return approvedAt; }
const string& Request::GetPrintedAt() const { return printedAt; }
const string& Request::GetReadyAt() const { return readyAt; }
const string& Request::GetAppointmentAt() const { return appointmentAt; }
const string& Request::GetCollectedAt() const { return collectedAt; }
const string& Request::GetRejectReason() const { return rejectReason; }
PaymentStatus Request::GetPaymentStatus() const { return paymentStatus; }
const string& Request::GetPaidAt() const { return paidAt; }

long long Request::GetTotalFee() const { return unitFee * quantity; }
void Request::Approve(const string& now) {
    if (status != RequestStatus::PENDING) throw runtime_error("Chi duyet don PENDING.");
    status = RequestStatus::APPROVED; approvedAt = now;
}
void Request::Reject(const string& reason) {
    if (status != RequestStatus::PENDING || reason.empty()) throw runtime_error("Khong the tu choi don.");
    status = RequestStatus::REJECTED; rejectReason = reason;
}
void Request::MarkPrinted(const string& now) {
    if (status != RequestStatus::APPROVED) throw runtime_error("Don chua duoc duyet.");
    status = RequestStatus::PRINTED; printedAt = now;
}
void Request::SetReady(const string& now, const string& appointment) {
    if (status != RequestStatus::PRINTED) throw runtime_error("Don chua duoc in.");
    status = RequestStatus::READY_FOR_COLLECTION; readyAt = now; appointmentAt = appointment;
}
void Request::RecordPayment(const string& now) {
    if (status != RequestStatus::READY_FOR_COLLECTION || paymentStatus != PaymentStatus::UNPAID)
        throw runtime_error("Don khong can thu tien hoac da thanh toan.");
    paymentStatus = PaymentStatus::PAID; paidAt = now;
}
void Request::Collect(const string& now) {
    if (status != RequestStatus::READY_FOR_COLLECTION || paymentStatus == PaymentStatus::UNPAID)
        throw runtime_error("Don chua du dieu kien giao giay.");
    status = RequestStatus::COLLECTED; collectedAt = now;
}
