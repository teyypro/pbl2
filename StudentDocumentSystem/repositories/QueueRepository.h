#pragma once
#include "../models/Ticket.h"
#include "../templates/MyList.h"

using namespace std;
class QueueRepository {
private:
    string directory;
    string Path(Department department) const;
public:
    explicit QueueRepository(const string& directory);
    MyList<Ticket> Load() const;
    void SaveDepartment(const MyList<Ticket>& tickets, Department department) const;
};
