#include "PaymentStatus.h"
#include <stdexcept>

using namespace std;

string ToString(PaymentStatus value) {
    switch (value) {
    case PaymentStatus::NOT_REQUIRED: return "NOT_REQUIRED";
    case PaymentStatus::UNPAID: return "UNPAID";
    case PaymentStatus::PAID: return "PAID";
    }
    throw runtime_error("Gia tri enum khong hop le.");
}
PaymentStatus ParsePaymentStatus(const string& value) {
    if (value == "NOT_REQUIRED") return PaymentStatus::NOT_REQUIRED;
    if (value == "UNPAID") return PaymentStatus::UNPAID;
    if (value == "PAID") return PaymentStatus::PAID;
    throw runtime_error("PaymentStatus khong hop le: " + value);
}
