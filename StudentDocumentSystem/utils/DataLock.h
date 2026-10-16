#pragma once
#include <string>

using namespace std;
class DataLock {
private:
    int descriptor = -1;
public:
    explicit DataLock(const string& directory);
    ~DataLock();
    DataLock(const DataLock&) = delete;
    DataLock& operator=(const DataLock&) = delete;
};
