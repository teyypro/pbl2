#pragma once
#include <string>
#include "../enums/RequestStatus.h"
#include "../enums/PaymentStatus.h"

using namespace std;

class Request {
private:
    string requestId;
    string studentId;
    string documentId;
    int quantity;
    long long unitFee;
    string note;
    RequestStatus status;
    string requestedAt;
    string approvedAt;
    string printedAt;
    string readyAt;
    string appointmentAt;
    string collectedAt;
    string rejectReason;
    PaymentStatus paymentStatus;
    string paidAt;
public:
    Request(const string& requestId, const string& studentId, const string& documentId, int quantity, long long unitFee, const string& note, RequestStatus status, const string& requestedAt, const string& approvedAt, const string& printedAt, const string& readyAt, const string& appointmentAt, const string& collectedAt, const string& rejectReason, PaymentStatus paymentStatus, const string& paidAt);
    const string& GetRequestId() const;
    const string& GetStudentId() const;
    const string& GetDocumentId() const;
    int GetQuantity() const;
    long long GetUnitFee() const;
    const string& GetNote() const;
    RequestStatus GetStatus() const;
    const string& GetRequestedAt() const;
    const string& GetApprovedAt() const;
    const string& GetPrintedAt() const;
    const string& GetReadyAt() const;
    const string& GetAppointmentAt() const;
    const string& GetCollectedAt() const;
    const string& GetRejectReason() const;
    PaymentStatus GetPaymentStatus() const;
    const string& GetPaidAt() const;
    long long GetTotalFee() const;
    void Approve(const string& now);
    void Reject(const string& reason);
    void MarkPrinted(const string& now);
    void SetReady(const string& now, const string& appointment);
    void RecordPayment(const string& now);
    void Collect(const string& now);
};
