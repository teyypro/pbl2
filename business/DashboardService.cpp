#include "DashboardService.h"

DashboardService::DashboardService(DbContext& d) : db(d) {}

void DashboardService::refresh()
{
    const MyList<Quay*>& quays = db.quay.all();

    MyList<string> lines;
    for (int i = 0; i < quays.getSize(); i++)
    {
        Quay* q = quays[i];

        PhieuHangDoi* serving = q->servingTicket();             // phiếu đang phục vụ
        string so = (serving != NULL) ? serving->getDisplay() : string("---");

        MyList<PhieuHangDoi*> cho = q->waitingTickets();        // đã sắp theo số thứ tự
        string dsCho;                                           // "A001,A002,..."
        for (int j = 0; j < cho.getSize(); j++)
        {
            if (j > 0) dsCho += ",";
            dsCho += cho[j]->getDisplay();
        }

        lines.add(q->getMaQuay() + "|" + so + "|" +
                  (q->isActive() ? "1" : "0") + "|" + to_string(cho.getSize()) + "|" + dsCho);
    }
    db.database().writeLines("CurrentServing.txt", lines);
}

MyList<CounterDisplay> DashboardService::readCurrent()
{
    MyList<CounterDisplay> result;
    MyList<string> lines = db.database().readLines("CurrentServing.txt");
    for (int i = 0; i < lines.getSize(); i++)
    {
        MyList<string> f = split(lines[i], '|');
        if (f.getSize() < 3) continue;
        CounterDisplay c;
        c.maQuay    = f[0];
        c.soHienThi = f[1];
        c.active    = (f[2] != "0");

        // cột 5: danh sách số chờ, phân tách bằng dấu phẩy
        if (f.getSize() >= 5 && !f[4].empty())
        {
            MyList<string> parts = split(f[4], ',');
            for (int j = 0; j < parts.getSize(); j++)
            {
                string s = trim(parts[j]);
                if (!s.empty()) c.danhSachCho.add(s);
            }
        }
        result.add(c);
    }
    return result;
}
