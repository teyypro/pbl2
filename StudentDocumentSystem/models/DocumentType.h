#pragma once
#include <string>
#include "../enums/Department.h"

using namespace std;

class DocumentType {
private:
    string documentId;
    string documentName;
    Department department;
    long long fee;
public:
    DocumentType(const string& documentId, const string& documentName, Department department, long long fee);
    const string& GetDocumentId() const;
    const string& GetDocumentName() const;
    Department GetDepartment() const;
    long long GetFee() const;

};
