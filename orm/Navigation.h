#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <string>
#include "../templates/MyList.h"
using namespace std;

// Khai báo trước; định nghĩa đầy đủ trong DbSet.h
template<class T> class DbSet;

// =====================================================================
// Reference<T>: Navigation Property trỏ tới MỘT đối tượng (N-1 hoặc 1-1).
// Đối tượng liên quan được nạp ngay khi gọi load() (eager loading).
// =====================================================================
template<class T>
class Reference
{
private:
    T* value;                   // đối tượng liên quan, NULL nếu không có

    void clear() { delete value; value = NULL; }

public:
    Reference() : value(NULL) {}
    Reference(const Reference& o) : value(o.value != NULL ? new T(*o.value) : NULL) {}
    Reference& operator=(const Reference& o)
    {
        if (this != &o)
        {
            clear();
            if (o.value != NULL) value = new T(*o.value);
        }
        return *this;
    }
    ~Reference() { delete value; }

    // Nạp đối tượng liên quan, tìm theo KHÓA CHÍNH của T
    void load(DbSet<T>* set, const string& keyValue)
    {
        clear();
        T found;
        if (set != NULL && set->findByKey(keyValue, found, false)) value = new T(found);
    }

    // Nạp đối tượng liên quan, tìm theo một cột bất kỳ (dùng cho 1-1 chiều ngược)
    void load(DbSet<T>* set, const string& column, const string& columnValue)
    {
        clear();
        T found;
        if (set != NULL && set->findOneBy(column, columnValue, found, false)) value = new T(found);
    }

    T* get() const { return value; }              // NULL nếu không có
    T* operator->() const { return value; }       // chú ý: kiểm tra hasValue() nếu có thể NULL
    bool hasValue() const { return value != NULL; }
};

// =====================================================================
// Collection<T>: Navigation Property trỏ tới NHIỀU đối tượng (1-N).
// =====================================================================
template<class T>
class Collection
{
private:
    MyList<T> list;

public:
    // Nạp các đối tượng T có cột `column` bằng `columnValue`
    void load(DbSet<T>* set, const string& column, const string& columnValue)
    {
        list.clear();
        if (set != NULL) list = set->findBy(column, columnValue, false);
    }

    const MyList<T>& items() const { return list; }
    int count() const { return list.getSize(); }
};

#endif