#ifndef REPOSITORY_H
#define REPOSITORY_H

// Giao diện Repository tổng quát (Repository Pattern).
// DbSet<T> của ORM là bản cài đặt cụ thể của giao diện này.
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