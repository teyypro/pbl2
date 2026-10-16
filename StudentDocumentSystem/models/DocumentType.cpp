#include "DocumentType.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>

using namespace std;
DocumentType::DocumentType(const string& documentId, const string& documentName, Department department, long long fee)
    : documentId(documentId), documentName(documentName), department(department), fee(fee) {}
const string& DocumentType::GetDocumentId() const { return documentId; }
const string& DocumentType::GetDocumentName() const { return documentName; }
Department DocumentType::GetDepartment() const { return department; }
long long DocumentType::GetFee() const { return fee; }
