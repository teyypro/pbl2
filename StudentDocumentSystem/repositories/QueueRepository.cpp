#include "QueueRepository.h"
#include "../utils/FileHelper.h"
#include "../utils/DateTimeHelper.h"
#include <stdexcept>
#include <limits>
#include <sstream>

using namespace std;
QueueRepository::QueueRepository(const string& directory) : directory(directory) {}
string QueueRepository::Path(Department department) const {
    if (department == Department::CTSV) return directory + "/queue_ctsv.txt";
    if (department == Department::DAOTAO) return directory + "/queue_daotao.txt";
    throw runtime_error("Phong khong hop le.");
}
MyList<Ticket> QueueRepository::Load() const {
    MyList<Ticket> items;
    Department departments[] = {Department::CTSV, Department::DAOTAO};
    for (Department department : departments) {
        string path = Path(department);
        MyList<string> lines = FileHelper::ReadLines(path);
        for (int i = 0; i < lines.Size(); ++i) {
            try {
                MyList<string> f = FileHelper::Split(lines.GetByIndex(i), 10);
                for (int j = 0; j < 10; ++j) FileHelper::ValidateText(f.GetByIndex(j), j < 8);
                Ticket item(f.GetByIndex(0), f.GetByIndex(1), ParseDepartment(f.GetByIndex(2)),
                    FileHelper::Number(f.GetByIndex(3), 1, (numeric_limits<long long>::max)() - 1),
                    f.GetByIndex(4), f.GetByIndex(5), ParseTicketStatus(f.GetByIndex(6)),
                    f.GetByIndex(7), f.GetByIndex(8), f.GetByIndex(9));
                if (item.GetDepartment() != department || !DateTimeHelper::IsValid(item.GetIssuedAt())
                    || DateTimeHelper::Date(item.GetIssuedAt()) != item.GetTicketDate())
                    throw runtime_error("Phong/ngay cap ve khong hop le.");
                string dateKey = item.GetTicketDate();
                dateKey.erase(7, 1); dateKey.erase(4, 1);
                if (item.GetTicketId() != dateKey + "-" + item.GetDisplayNumber())
                    throw runtime_error("Ma ve khong khop ngay va so thu tu.");
                if ((!item.GetCalledAt().empty() && !DateTimeHelper::IsValid(item.GetCalledAt()))
                    || (!item.GetFinishedAt().empty() && !DateTimeHelper::IsValid(item.GetFinishedAt())))
                    throw runtime_error("Thoi gian ve khong hop le.");
                if ((item.GetStatus() == TicketStatus::SERVING || item.GetStatus() == TicketStatus::SKIPPED
                    || item.GetStatus() == TicketStatus::COMPLETED) && item.GetCalledAt().empty())
                    throw runtime_error("Thieu thoi gian goi ve.");
                if (!item.IsActive() && item.GetFinishedAt().empty()) throw runtime_error("Thieu thoi gian ket thuc ve.");
                items.Add(item);
            } catch (const exception& error) {
                throw runtime_error(path + ": dong " + to_string(i + 1) + ": " + error.what());
            }
        }
    }
    return items;
}
void QueueRepository::SaveDepartment(const MyList<Ticket>& tickets, Department department) const {
    MyList<string> lines;
    for (int i = 0; i < tickets.Size(); ++i) {
        const Ticket& t = tickets.GetByIndex(i);
        if (t.GetDepartment() != department) continue;
        ostringstream line;
        line << t.GetTicketId() << '|' << t.GetTicketDate() << '|' << ToString(t.GetDepartment()) << '|'
             << t.GetSequenceNumber() << '|' << t.GetStudentId() << '|' << t.GetRequestId() << '|'
             << ToString(t.GetStatus()) << '|' << t.GetIssuedAt() << '|' << t.GetCalledAt() << '|' << t.GetFinishedAt();
        lines.Add(line.str());
    }
    FileHelper::WriteLines(Path(department), lines);
}
