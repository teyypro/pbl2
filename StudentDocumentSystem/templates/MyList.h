#pragma once
#include <stdexcept>

using namespace std;

template <typename T>
class MyList {
private:
    struct Node {
        T value;
        Node* next;
        explicit Node(const T& item) : value(item), next(nullptr) {}
    };
    Node* head = nullptr;
    Node* tail = nullptr;
    int count = 0;
    void Swap(MyList& other) noexcept {
        Node* oldHead = head; head = other.head; other.head = oldHead;
        Node* oldTail = tail; tail = other.tail; other.tail = oldTail;
        int oldCount = count; count = other.count; other.count = oldCount;
    }
public:
    MyList() = default;
    ~MyList() { Clear(); }
    // Sao chep sau: moi danh sach so huu cac node rieng.
    MyList(const MyList& other) {
        try {
            for (Node* node = other.head; node; node = node->next) Add(node->value);
        } catch (...) { Clear(); throw; }
    }
    MyList& operator=(const MyList& other) {
        if (this != &other) { MyList copy(other); Swap(copy); }
        return *this;
    }
    void Add(const T& value) {
        Node* node = new Node(value);
        if (tail) tail->next = node;
        else head = node;
        tail = node;
        ++count;
    }
    void AddFirst(const T& value) {
        Node* node = new Node(value);
        node->next = head;
        head = node;
        if (!tail) tail = node;
        ++count;
    }
    void Remove(int index) {
        if (index < 0 || index >= count) throw out_of_range("Chi so khong hop le.");
        Node* previous = nullptr;
        Node* node = head;
        for (int i = 0; i < index; ++i) { previous = node; node = node->next; }
        if (previous) previous->next = node->next;
        else head = node->next;
        if (tail == node) tail = previous;
        delete node;
        --count;
    }
    T& GetByIndex(int index) {
        if (index < 0 || index >= count) throw out_of_range("Chi so khong hop le.");
        Node* node = head;
        for (int i = 0; i < index; ++i) node = node->next;
        return node->value;
    }
    const T& GetByIndex(int index) const {
        if (index < 0 || index >= count) throw out_of_range("Chi so khong hop le.");
        Node* node = head;
        for (int i = 0; i < index; ++i) node = node->next;
        return node->value;
    }
    int Find(const T& value) const {
        int index = 0;
        for (Node* node = head; node; node = node->next, ++index)
            if (node->value == value) return index;
        return -1;
    }
    int Size() const { return count; }
    bool Empty() const { return count == 0; }
    void Clear() noexcept {
        while (head) { Node* old = head; head = head->next; delete old; }
        tail = nullptr;
        count = 0;
    }
};
