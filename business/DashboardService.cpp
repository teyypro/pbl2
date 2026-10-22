#include "DashboardService.h"

DashboardService::DashboardService(DbContext& d) : db(d) {}

void DashboardService::refresh()
{
    MyList<Quay> quays = db.quay.findAll();

    MyList<string> lines;
    for (int i = 0; i < quays.getSize(); i++)
    {
        string so = "---";
        int waiting = 0;
        string dsCho;                                   // "A001,A002,..."

        const MyList<PhieuHangDoi>& tickets = quays[i].phieus.items();

        // Gom các phiếu WAITING rồi sắp xếp tăng dần theo số thứ tự
        // (thứ tự gọi thực tế là theo số thứ tự, không phải thứ tự dòng trong file)
        MyList<PhieuHangDoi> cho;
        for (int j = 0; j < tickets.getSize(); j++)
        {
            if (tickets[j].getTrangThai() == Q_SERVING) so = tickets[j].getDisplay();
            if (tickets[j].getTrangThai() == Q_WAITING) cho.add(tickets[j]);
        }

        // sắp xếp nổi bọt theo soThuTu
        for (int a = 0; a < cho.getSize(); a++)
            for (int b = a + 1; b < cho.getSize(); b++)
                if (cho[b].getSoThuTu() < cho[a].getSoThuTu())
                {
                    PhieuHangDoi tmp = cho[a]; cho[a] = cho[b]; cho[b] = tmp;
                }

        for (int j = 0; j < cho.getSize(); j++)
        {
            if (j > 0) dsCho += ",";
            dsCho += cho[j].getDisplay();
        }
        waiting = cho.getSize();

        lines.add(quays[i].getMaQuay() + "|" + so + "|" +
                  (quays[i].isActive() ? "1" : "0") + "|" + to_string(waiting) + "|" + dsCho);
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