#pragma once
#include <string>
#include "../models/Records.h"
#include "../templates/MyList.h"

using namespace std;

class UserRepository {
private:
    string path;
public:
    explicit UserRepository(const string& directory);
    MyList<AccountRecord> Load() const;
    void Save(const MyList<AccountRecord>& items) const;
};
