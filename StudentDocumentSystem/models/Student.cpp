#include "Student.h"

using namespace std;
Student::Student(const string& id, const string& studentId, const string& name,
                 const string& className, const string& faculty)
    : User(id, studentId, name, UserRole::STUDENT), studentId(studentId),
      className(className), faculty(faculty) {}
string Student::GetStudentId() const { return studentId; }
const string& Student::GetClassName() const { return className; }
const string& Student::GetFaculty() const { return faculty; }
