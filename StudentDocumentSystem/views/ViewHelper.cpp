#include "ViewHelper.h"
#include "../utils/InputHelper.h"
#include <iostream>

using namespace std;
void ViewHelper::Documents(const MyList<DocumentType>& documents) {
    cout << "\nMA | TEN GIAY | PHONG | DON GIA (VND)\n";
    for (int i = 0; i < documents.Size(); ++i) {
        const DocumentType& d = documents.GetByIndex(i);
        cout << d.GetDocumentId() << " | " << d.GetDocumentName() << " | "
            << ToString(d.GetDepartment()) << " | " << d.GetFee() << '\n';
    }
}
void ViewHelper::Requests(const MyList<RequestInfo>& requests) {
    cout << "\nMA DON | MSSV | GIAY TO | SO LUONG | TONG PHI | TRANG THAI | HEN NHAN\n";
    if (requests.Empty()) cout << "Khong co don.\n";
    for (int i = 0; i < requests.Size(); ++i) {
        const RequestInfo& info = requests.GetByIndex(i);
        const Request& r = info.request;
        cout << r.GetRequestId() << " | " << r.GetStudentId() << " | " << info.documentName
            << " | " << r.GetQuantity() << " | " << r.GetTotalFee() << " | " << ToString(r.GetStatus())
            << " | " << r.GetAppointmentAt() << '\n';
    }
}
void ViewHelper::RequestDetails(const RequestInfo& info) {
    const Request& r = info.request;
    cout << "\nDon: " << r.GetRequestId() << " - " << info.documentName
        << "\nSinh vien: " << r.GetStudentId() << " - " << info.studentName
        << "\nSo luong: " << r.GetQuantity() << "; don gia: " << r.GetUnitFee()
        << "; tong phi: " << r.GetTotalFee() << " VND"
        << "\nTrang thai: " << ToString(r.GetStatus()) << "\nGhi chu: " << r.GetNote()
        << "\nLap luc: " << r.GetRequestedAt() << "\nDuyet luc: " << r.GetApprovedAt()
        << "\nIn luc: " << r.GetPrintedAt() << "\nSan sang luc: " << r.GetReadyAt()
        << "\nHen nhan: " << r.GetAppointmentAt() << "\nDa nhan luc: " << r.GetCollectedAt()
        << "\nLy do tu choi: " << r.GetRejectReason()
        << "\nThanh toan: " << ToString(r.GetPaymentStatus()) << "\nThu tien luc: " << r.GetPaidAt() << '\n';
}
void ViewHelper::CounterDetails(const CounterInfo& info) {
    cout << "\nVE: " << info.ticket.GetDisplayNumber() << " (" << info.ticket.GetTicketId()
        << ") - " << ToString(info.ticket.GetStatus()) << '\n';
    RequestDetails(info.details);
}
Department ViewHelper::ChooseDepartment() {
    return InputHelper::Integer("Phong (1: CTSV, 2: Dao tao): ", 1, 2) == 1 ? Department::CTSV : Department::DAOTAO;
}
void ViewHelper::Error(const exception& error) { cout << "Loi: " << error.what() << '\n'; }
