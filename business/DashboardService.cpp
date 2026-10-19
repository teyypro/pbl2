#include "DashboardService.h"

CounterDisplay::CounterDisplay() : active(true) {}

DashboardService::DashboardService(PhieuRepo& p, QuayRepo& q, const string& file) : phieuRepo(p), quayRepo(q), path(file) {}

void DashboardService::refresh()
{
    phieuRepo.load();
    quayRepo.load();
    MyList<string> lines;
    const MyList<Quay>& qs = quayRepo.getAll();
    const MyList<PhieuHangDoi>& tk = phieuRepo.getAll();
    for (int i = 0; i < qs.getSize(); i++)
    {
        string so = "---";
        for (int j = 0; j < tk.getSize(); j++)
        {
            if (tk[j].getMaQuay() == qs[i].getMaQuay() && tk[j].getTrangThai() == Q_SERVING)
                so = tk[j].getDisplay();
        }
        lines.add(qs[i].getMaQuay() + "|" + so + "|" + (qs[i].isActive() ? "1" : "0"));
    }
    FileManager::writeLines(path, lines);
}

MyList<CounterDisplay> DashboardService::readCurrent()
{
    MyList<CounterDisplay> r;
    MyList<string> lines = FileManager::readLines(path);
    for (int i = 0; i < lines.getSize(); i++)
    {
        MyList<string> f = splitN(lines[i], 3);
        CounterDisplay c;
        c.maQuay = f[0];
        c.soHienThi = f[1];
        c.active = (f[2] != "0");
        r.add(c);
    }
    return r;
}
