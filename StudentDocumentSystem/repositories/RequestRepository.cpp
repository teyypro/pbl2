#include "RequestRepository.h"
#include "../utils/FileHelper.h"
#include "../utils/DateTimeHelper.h"
#include "../utils/PasswordHelper.h"
#include <limits>
#include <sstream>
#include <stdexcept>

using namespace std;
namespace { void CheckTime(const string& value, bool required) { if ((required || !value.empty()) && !DateTimeHelper::IsValid(value)) throw runtime_error("Ngay gio khong hop le."); } }
RequestRepository::RequestRepository(const string& directory) : path(directory + "/requests.txt") {}
MyList<Request> RequestRepository::Load() const {
    MyList<Request> items;
    MyList<string> lines = FileHelper::ReadLines(path);
    for (int i = 0; i < lines.Size(); ++i) {
        try {
            MyList<string> f = FileHelper::Split(lines.GetByIndex(i), 16);
            FileHelper::ValidateText(f.GetByIndex(0), true);
            FileHelper::ValidateText(f.GetByIndex(1), true);
            FileHelper::ValidateText(f.GetByIndex(2), true);
            FileHelper::ValidateText(f.GetByIndex(3), true);
            FileHelper::ValidateText(f.GetByIndex(4), true);
            FileHelper::ValidateText(f.GetByIndex(5), false);
            FileHelper::ValidateText(f.GetByIndex(6), true);
            FileHelper::ValidateText(f.GetByIndex(7), true);
            FileHelper::ValidateText(f.GetByIndex(8), false);
            FileHelper::ValidateText(f.GetByIndex(9), false);
            FileHelper::ValidateText(f.GetByIndex(10), false);
            FileHelper::ValidateText(f.GetByIndex(11), false);
            FileHelper::ValidateText(f.GetByIndex(12), false);
            FileHelper::ValidateText(f.GetByIndex(13), false);
            FileHelper::ValidateText(f.GetByIndex(14), true);
            FileHelper::ValidateText(f.GetByIndex(15), false);
            int quantity = static_cast<int>(FileHelper::Number(f.GetByIndex(3), 1, (numeric_limits<int>::max)()));
    long long unitFee = FileHelper::Number(f.GetByIndex(4), 0, (numeric_limits<long long>::max)() / quantity);
    Request item(f.GetByIndex(0), f.GetByIndex(1), f.GetByIndex(2), quantity, unitFee, f.GetByIndex(5),
        ParseRequestStatus(f.GetByIndex(6)), f.GetByIndex(7), f.GetByIndex(8), f.GetByIndex(9),
        f.GetByIndex(10), f.GetByIndex(11), f.GetByIndex(12), f.GetByIndex(13),
        ParsePaymentStatus(f.GetByIndex(14)), f.GetByIndex(15));
    for (int j = 7; j <= 12; ++j) CheckTime(f.GetByIndex(j), j == 7);
    CheckTime(item.GetPaidAt(), item.GetPaymentStatus() == PaymentStatus::PAID);
    RequestStatus s = item.GetStatus();
    if (s == RequestStatus::REJECTED) FileHelper::ValidateText(item.GetRejectReason());
    if (s == RequestStatus::APPROVED || s == RequestStatus::PRINTED || s == RequestStatus::READY_FOR_COLLECTION || s == RequestStatus::COLLECTED)
        CheckTime(item.GetApprovedAt(), true);
    if (s == RequestStatus::PRINTED || s == RequestStatus::READY_FOR_COLLECTION || s == RequestStatus::COLLECTED)
        CheckTime(item.GetPrintedAt(), true);
    if (s == RequestStatus::READY_FOR_COLLECTION || s == RequestStatus::COLLECTED) {
        CheckTime(item.GetReadyAt(), true); CheckTime(item.GetAppointmentAt(), true);
        if (item.GetAppointmentAt() < item.GetReadyAt()) throw runtime_error("Lich hen truoc khi san sang.");
    }
    if (s == RequestStatus::COLLECTED) CheckTime(item.GetCollectedAt(), true);
    if ((unitFee == 0) != (item.GetPaymentStatus() == PaymentStatus::NOT_REQUIRED))
        throw runtime_error("Trang thai phi khong phu hop.");
    if (s == RequestStatus::COLLECTED && item.GetPaymentStatus() == PaymentStatus::UNPAID)
        throw runtime_error("Don da giao nhung chua thanh toan.");
            items.Add(item);
        } catch (const exception& error) {
            throw runtime_error(path + ": dong " + to_string(i + 1) + ": " + error.what());
        }
    }
    return items;
}
void RequestRepository::Save(const MyList<Request>& items) const {
    MyList<string> lines;
    for (int i = 0; i < items.Size(); ++i) {
        const Request& item = items.GetByIndex(i);
        ostringstream line;
        line << item.GetRequestId() << "|" << item.GetStudentId() << "|" << item.GetDocumentId() << "|" << item.GetQuantity() << "|" << item.GetUnitFee() << "|" << item.GetNote() << "|" << ToString(item.GetStatus()) << "|" << item.GetRequestedAt() << "|" << item.GetApprovedAt() << "|" << item.GetPrintedAt() << "|" << item.GetReadyAt() << "|" << item.GetAppointmentAt() << "|" << item.GetCollectedAt() << "|" << item.GetRejectReason() << "|" << ToString(item.GetPaymentStatus()) << "|" << item.GetPaidAt();
        lines.Add(line.str());
    }
    FileHelper::WriteLines(path, lines);
}
