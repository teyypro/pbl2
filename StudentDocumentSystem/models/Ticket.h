#pragma once
#include <string>
#include "../enums/Department.h"
#include "../enums/TicketStatus.h"

using namespace std;

class Ticket {
private:
    string ticketId;
    string ticketDate;
    Department department;
    long long sequenceNumber;
    string studentId;
    string requestId;
    TicketStatus status;
    string issuedAt;
    string calledAt;
    string finishedAt;
public:
    Ticket(const string& ticketId, const string& ticketDate, Department department, long long sequenceNumber, const string& studentId, const string& requestId, TicketStatus status, const string& issuedAt, const string& calledAt, const string& finishedAt);
    const string& GetTicketId() const;
    const string& GetTicketDate() const;
    Department GetDepartment() const;
    long long GetSequenceNumber() const;
    const string& GetStudentId() const;
    const string& GetRequestId() const;
    TicketStatus GetStatus() const;
    const string& GetIssuedAt() const;
    const string& GetCalledAt() const;
    const string& GetFinishedAt() const;
    string GetDisplayNumber() const;
    bool IsActive() const;
    void Call(const string& now);
    void Skip(const string& now);
    void Complete(const string& now);
    void Expire(const string& now);
};
