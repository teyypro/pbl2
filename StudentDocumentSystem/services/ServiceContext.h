#pragma once
#include "../repositories/UserRepository.h"
#include "../repositories/StudentRepository.h"
#include "../repositories/DocumentRepository.h"
#include "../repositories/RequestRepository.h"
#include "../repositories/QueueRepository.h"
#include "../models/User.h"
#include "../utils/DateTimeHelper.h"

using namespace std;

struct SystemData {
    MyList<AccountRecord> accounts;
    MyList<StudentRecord> students;
    MyList<DocumentType> documents;
    MyList<Request> requests;
    MyList<Ticket> tickets;
};
struct RequestInfo {
    Request request;
    string documentName;
    string studentName;
};
struct CounterInfo {
    Ticket ticket;
    RequestInfo details;
};
struct QueueSnapshot {
    string current;
    MyList<string> waiting;
};

// Noi chia se repository va cac phep tra cuu. Service ngoai cung so huu DataLock.
class ServiceContext {
private:
    string directory;
    RequestRepository requestRepository;
public:
    UserRepository users;
    StudentRepository students;
    DocumentRepository documents;
    IRequestRepository& requests;
    QueueRepository queues;
    DateTimeHelper& clock;
    ServiceContext(const string& directory, DateTimeHelper& clock);
    const string& GetDirectory() const;
    SystemData Load() const;
    void RequireUser(const SystemData& data, const User& user, UserRole role) const;
    static int FindStudent(const SystemData& data, const string& id);
    static int FindDocument(const SystemData& data, const string& id);
    static int FindRequest(const SystemData& data, const string& id);
    static int FindTicket(const SystemData& data, const string& id);
    static Department RequestDepartment(const SystemData& data, const Request& request);
    static RequestInfo Describe(const SystemData& data, const Request& request);
    void SynchronizeQueues(SystemData& data, const string& now) const;
    static void CheckDepartment(Department department);
};
