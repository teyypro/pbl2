#pragma once
#include "BaseView.h"
#include "../services/QueueService.h"
class GuestView : public BaseView {
private:
    QueueService& queue;
public:
    explicit GuestView(QueueService& queue);
    void showMenu() override;
};
