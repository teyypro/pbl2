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
// Quy ước: CỘT ĐẦU TIÊN LÀ KHÓA CHÍNH.
class Entity
{
public:
    virtual ~Entity() {}

    virtual string tableName() const = 0;            // tên bảng (= tên file không đuôi)
    virtual Row columns() const = 0;                 // tên các cột, theo thứ tự
    virtual Row toRow() const = 0;                   // object  -> dòng dữ liệu
    virtual void fromRow(const Row& row) = 0;        // dòng dữ liệu -> object

    // Relationship Mapping: entity có Navigation Property thì ghi đè hàm này,
    // nạp các đối tượng liên quan vào navigation (eager loading).
    // DbSet gọi hàm này cho mỗi đối tượng trả về. Mặc định: không có quan hệ.
    virtual void loadRelations(DbContext*) {}

    string getKey() const;                           // giá trị khóa chính
};

#endif