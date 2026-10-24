#ifndef ENTITY_H
#define ENTITY_H

#include <string>
#include "../templates/MyList.h"
#include "../utils/Utils.h"
using namespace std;

// Một dòng dữ liệu = danh sách giá trị các cột (dạng chuỗi)
typedef MyList<string> Row;

class DbContext;        // khai báo trước, Entity không cần biết chi tiết

// Lớp cơ sở cho mọi đối tượng được ORM ánh xạ vào file TXT.
// Mỗi đối tượng được tạo DUY NHẤT MỘT LẦN trên RAM (do DbSet sở hữu);
// các đối tượng liên kết với nhau bằng CON TRỎ -> không cho sao chép.
class Entity
{
public:
    Entity() {}
    virtual ~Entity() {}
    Entity(const Entity&) = delete;
    Entity& operator=(const Entity&) = delete;

    // ----- Entity Mapping: object <-> dòng trong file TXT -----
    virtual string tableName() const = 0;            // tên bảng (= tên file không đuôi)
    virtual Row columns() const = 0;                 // tên các cột, theo thứ tự
    virtual Row toRow() const = 0;                   // object  -> dòng dữ liệu
    virtual void fromRow(const Row& row) = 0;        // dòng dữ liệu -> object
    virtual string getKey() const = 0;               // giá trị khóa chính

    // ----- Object Relationship (thiết lập / gỡ liên kết hai chiều trên RAM) -----
    // linkRelations  : tìm các đối tượng liên quan theo khóa ngoại, gắn hai chiều
    //                  (this -> liên quan  và  liên quan -> this)
    // unlinkRelations: gỡ mọi liên kết trỏ tới this (gọi trước khi xóa this)
    // removeChildren : Composition - xóa các đối tượng con phụ thuộc vòng đời của this
    virtual void linkRelations(DbContext*) {}
    virtual void unlinkRelations() {}
    virtual void removeChildren(DbContext*) {}
};

#endif
