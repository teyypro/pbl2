#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <cstddef>
#include <string>
#include "../templates/MyList.h"
using namespace std;

// =====================================================================
// Reference<T>: Navigation Property trỏ tới MỘT đối tượng (N-1 hoặc 1-1).
// KHÔNG sở hữu đối tượng: chỉ giữ con trỏ tới đối tượng nằm trong DbSet (RAM).
// =====================================================================
template<class T>
class Reference
{
private:
    T* value;

public:
    Reference() : value(NULL) {}

    void set(T* p) { value = p; }
    void clear() { value = NULL; }

    T* get() const { return value; }              // NULL nếu không có
    T* operator->() const { return value; }       // kiểm tra hasValue() nếu có thể NULL
    bool hasValue() const { return value != NULL; }
};

// =====================================================================
// Collection<T>: Navigation Property trỏ tới NHIỀU đối tượng (1-N).
// KHÔNG sở hữu các đối tượng. Không chứa trùng con trỏ.
// =====================================================================
template<class T>
class Collection
{
private:
    MyList<T*> list;

public:
    int indexOf(T* p) const
    {
        for (int i = 0; i < list.getSize(); i++)
            if (list[i] == p) return i;
        return -1;
    }

    void add(T* p)
    {
        if (p != NULL && indexOf(p) < 0) list.add(p);
    }

    void remove(T* p)
    {
        int i = indexOf(p);
        if (i >= 0) list.remove(i);
    }

    bool contains(T* p) const { return indexOf(p) >= 0; }
    void clear() { list.clear(); }

    const MyList<T*>& items() const { return list; }
    int count() const { return list.getSize(); }
};

#endif
