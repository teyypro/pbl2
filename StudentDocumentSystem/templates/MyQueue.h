#pragma once
#include "MyList.h"

// Danh sach lien ket giu ca head/tail, them cuoi va xoa dau O(1).
template <typename T>
class MyQueue {
private:
    MyList<T> items;
public:
    void Enqueue(const T& value) { items.Add(value); }
    void Dequeue() { items.Remove(0); }
    T& Front() { return items.GetByIndex(0); }
    const T& Front() const { return items.GetByIndex(0); }
    bool Empty() const { return items.Empty(); }
    int Size() const { return items.Size(); }
    void Clear() { items.Clear(); }
};
