#pragma once
#include "MyList.h"

template <typename T>
class MyStack {
private:
    MyList<T> items;
public:
    void Push(const T& value) { items.AddFirst(value); }
    void Pop() { items.Remove(0); }
    T& Top() { return items.GetByIndex(0); }
    const T& Top() const { return items.GetByIndex(0); }
    bool Empty() const { return items.Empty(); }
    int Size() const { return items.Size(); }
    void Clear() { items.Clear(); }
};
