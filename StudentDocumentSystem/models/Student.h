#pragma once
#include "User.h"

using namespace std;
class Student : public User {
private:
    string studentId;
    string className;
    string faculty;
public:
    Student(const string& id, const string& studentId, const string& name,
            const string& className, const string& faculty);
    string GetStudentId() const override;
    const string& GetClassName() const;
    const string& GetFaculty() const;
};
