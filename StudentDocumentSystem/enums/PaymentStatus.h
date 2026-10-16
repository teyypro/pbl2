#pragma once
#include <string>

using namespace std;

enum class PaymentStatus { NOT_REQUIRED, UNPAID, PAID };
string ToString(PaymentStatus value);
PaymentStatus ParsePaymentStatus(const string& value);
