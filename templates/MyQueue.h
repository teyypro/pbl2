#ifndef MYQUEUE_H
#define MYQUEUE_H

#include <stdexcept>

// Hàng đợi vòng (circular queue) tự cài đặt, tự giãn nở
template<class T>
class MyQueue
{
private:
    T* data;
    int front;
    int rear;
    int count;
    int capacity;

    void grow()
    {
        T* nd = new T[capacity * 2];
        for (int i = 0; i < count; i++) nd[i] = data[(front + i) % capacity];
        delete[] data;
        data = nd;
        front = 0;
        rear = count;
        capacity *= 2;
    }

public:
    MyQueue() : data(new T[8]), front(0), rear(0), count(0), capacity(8) {}

    MyQueue(const MyQueue& o) : data(new T[o.capacity]), front(0), rear(o.count), count(o.count), capacity(o.capacity)
    {
        for (int i = 0; i < count; i++) data[i] = o.data[(o.front + i) % o.capacity];
        if (rear == capacity) rear = 0;
    }

    MyQueue& operator=(const MyQueue& o)
    {
        if (this != &o)
        {
            T* nd = new T[o.capacity];
            for (int i = 0; i < o.count; i++) nd[i] = o.data[(o.front + i) % o.capacity];
            delete[] data;
            data = nd;
            front = 0;
            count = o.count;
            capacity = o.capacity;
            rear = (count == capacity) ? 0 : count;
        }
        return *this;
    }

    ~MyQueue() { delete[] data; }

    void enqueue(const T& item)
    {
        if (count == capacity) grow();
        data[rear] = item;
        rear = (rear + 1) % capacity;
        count++;
    }

    void dequeue()
    {
        if (count == 0) throw std::out_of_range("MyQueue::dequeue");
        front = (front + 1) % capacity;
        count--;
    }

    T& peek()
    {
        if (count == 0) throw std::out_of_range("MyQueue::peek");
        return data[front];
    }

    bool isEmpty() const { return count == 0; }
    int size() const { return count; }
};

#endif