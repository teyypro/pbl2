#include "Ticket.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>

using namespace std;
Ticket::Ticket(const string& ticketId, const string& ticketDate, Department department, long long sequenceNumber, const string& studentId, const string& requestId, TicketStatus status, const string& issuedAt, const string& calledAt, const string& finishedAt)
    : ticketId(ticketId), ticketDate(ticketDate), department(department), sequenceNumber(sequenceNumber), studentId(studentId), requestId(requestId), status(status), issuedAt(issuedAt), calledAt(calledAt), finishedAt(finishedAt) {}
const string& Ticket::GetTicketId() const { return ticketId; }
const string& Ticket::GetTicketDate() const { return ticketDate; }
Department Ticket::GetDepartment() const { return department; }
long long Ticket::GetSequenceNumber() const { return sequenceNumber; }
const string& Ticket::GetStudentId() const { return studentId; }
const string& Ticket::GetRequestId() const { return requestId; }
TicketStatus Ticket::GetStatus() const { return status; }
const string& Ticket::GetIssuedAt() const { return issuedAt; }
const string& Ticket::GetCalledAt() const { return calledAt; }
const string& Ticket::GetFinishedAt() const { return finishedAt; }

string Ticket::GetDisplayNumber() const {
    ostringstream output;
    output << (department == Department::CTSV ? "CTSV-" : "DT-")
           << setw(3) << setfill('0') << sequenceNumber;
    return output.str();
}
bool Ticket::IsActive() const { return status == TicketStatus::WAITING || status == TicketStatus::SERVING; }
void Ticket::Call(const string& now) {
    if (status != TicketStatus::WAITING) throw runtime_error("Ve khong dang cho.");
    status = TicketStatus::SERVING; calledAt = now;
}
void Ticket::Skip(const string& now) {
    if (status != TicketStatus::SERVING) throw runtime_error("Ve khong dang phuc vu.");
    status = TicketStatus::SKIPPED; finishedAt = now;
}
void Ticket::Complete(const string& now) { status = TicketStatus::COMPLETED; finishedAt = now; }
void Ticket::Expire(const string& now) { status = TicketStatus::EXPIRED; finishedAt = now; }
