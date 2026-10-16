#pragma once
#include <string>
#include "../models/DocumentType.h"
#include "../templates/MyList.h"

using namespace std;

class DocumentRepository {
private:
    string path;
public:
    explicit DocumentRepository(const string& directory);
    MyList<DocumentType> Load() const;
    void Save(const MyList<DocumentType>& items) const;
};
