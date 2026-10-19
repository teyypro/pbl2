#ifndef REPOSITORY_H
#define REPOSITORY_H

// Giao diện Repository tổng quát (Repository Pattern)
template<class T>
class Repository
{
public:
    virtual ~Repository() {}
    virtual void load() = 0;
    virtual void save() = 0;
    virtual void add(const T& obj) = 0;
    virtual void update(const T& obj) = 0;
};

#endif
