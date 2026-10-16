#pragma once
#include <string>

using namespace std;

enum class TicketStatus { WAITING, SERVING, COMPLETED, SKIPPED, EXPIRED };
string ToString(TicketStatus value);
TicketStatus ParseTicketStatus(const string& value);
