#pragma once
#include "BaseView.h"
#include "../services/CounterService.h"
class CounterView : public BaseView {
private:
    const User& user;
    CounterService& counter;
public:
    CounterView(const User& user, CounterService& counter);
    void showMenu() override;
};
