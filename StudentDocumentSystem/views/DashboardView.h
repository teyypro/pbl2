#pragma once
#include "BaseView.h"
#include "../services/QueueService.h"

using namespace std;
class DashboardView : public BaseView {
private:
    QueueService& queue;
    static void PrintDepartment(const string& title, const QueueSnapshot& snapshot);
public:
    explicit DashboardView(QueueService& queue);
    void showMenu() override;
};
