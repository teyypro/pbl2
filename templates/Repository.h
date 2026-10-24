#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <string>
using namespace std;

// Giao diện Repository tổng quát (Repository Pattern).
// DbSet<T> của ORM là bản cài đặt cụ thể của giao diện này.
// Repository giữ các đối tượng T* trên RAM; thay đổi trên RAM được ghi về file TXT.
template<class T>
class Repository
{
public:
    virtual ~Repository() {}
    virtual void load() = 0;                       // file -> RAM
    virtual void save() = 0;                       // RAM  -> file
    virtual void add(T* obj) = 0;                  // INSERT (nhận quyền sở hữu obj)
    virtual void update(T* obj) = 0;               // UPDATE (obj đã được sửa trên RAM)
    virtual void remove(const string& key) = 0;    // DELETE theo khóa chính
};

#endif
