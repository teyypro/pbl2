#pragma once
#include <string>
#include "../models/Records.h"
#include "../templates/MyList.h"

using namespace std;

class StudentRepository {
private:
    string path;
public:
    explicit StudentRepository(const string& directory);
    MyList<StudentRecord> Load() const;
    void Save(const MyList<StudentRecord>& items) const;
};
