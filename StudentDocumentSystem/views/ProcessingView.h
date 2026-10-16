#pragma once
#include "BaseView.h"
#include "../services/ProcessingService.h"
class ProcessingView : public BaseView {
private:
    const User& user;
    ProcessingService& processing;
public:
    ProcessingView(const User& user, ProcessingService& processing);
    void showMenu() override;
};
