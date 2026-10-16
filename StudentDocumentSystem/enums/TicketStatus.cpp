#include "TicketStatus.h"
#include <stdexcept>

using namespace std;

string ToString(TicketStatus value) {
    switch (value) {
    case TicketStatus::WAITING: return "WAITING";
    case TicketStatus::SERVING: return "SERVING";
    case TicketStatus::COMPLETED: return "COMPLETED";
    case TicketStatus::SKIPPED: return "SKIPPED";
    case TicketStatus::EXPIRED: return "EXPIRED";
    }
    throw runtime_error("Gia tri enum khong hop le.");
}
TicketStatus ParseTicketStatus(const string& value) {
    if (value == "WAITING") return TicketStatus::WAITING;
    if (value == "SERVING") return TicketStatus::SERVING;
    if (value == "COMPLETED") return TicketStatus::COMPLETED;
    if (value == "SKIPPED") return TicketStatus::SKIPPED;
    if (value == "EXPIRED") return TicketStatus::EXPIRED;
    throw runtime_error("TicketStatus khong hop le: " + value);
}
