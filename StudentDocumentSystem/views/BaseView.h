#pragma once
class BaseView {
public:
    virtual ~BaseView() = default;
    virtual void showMenu() = 0;
};
