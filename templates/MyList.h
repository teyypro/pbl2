#ifndef MYLIST_H
#define MYLIST_H

#include <stdexcept>

// Danh sách động tự cài đặt (mảng tự giãn nở)
template<class T>
class MyList
{
private:
    T* data;
    int size;
    int capacity;

    void grow()
    {
        int newCap = capacity * 2;
        T* nd = new T[newCap];
        for (int i = 0; i < size; i++) nd[i] = data[i];
        delete[] data;
        data = nd;
        capacity = newCap;
    }

public:
    MyList() : data(new T[4]), size(0), capacity(4) {}

    MyList(const MyList& o) : data(new T[o.capacity]), size(o.size), capacity(o.capacity)
    {
        for (int i = 0; i < size; i++) data[i] = o.data[i];
    }

    MyList& operator=(const MyList& o)
    {
        if (this != &o)
        {
            T* nd = new T[o.capacity];
            for (int i = 0; i < o.size; i++) nd[i] = o.data[i];
            delete[] data;
            data = nd;
            size = o.size;
            capacity = o.capacity;
        }
        return *this;
    }

    ~MyList() { delete[] data; }

    void add(const T& value)
    {
        if (size == capacity) grow();
        data[size++] = value;
    }

    void remove(int index)
    {
        if (index < 0 || index >= size) throw std::out_of_range("MyList::remove");
        for (int i = index; i < size - 1; i++) data[i] = data[i + 1];
        size--;
    }

    T& get(int index)
    {
        if (index < 0 || index >= size) throw std::out_of_range("MyList::get");
        return data[index];
    }

    const T& get(int index) const
    {
        if (index < 0 || index >= size) throw std::out_of_range("MyList::get");
        return data[index];
    }

    T& operator[](int index) { return get(index); }
    const T& operator[](int index) const { return get(index); }

    int getSize() const { return size; }
    bool isEmpty() const { return size == 0; }
    void clear() { size = 0; }
};

#endif