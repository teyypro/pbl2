#ifndef DBSET_H
#define DBSET_H

#include <cstddef>
#include "../templates/Repository.h"
#include "Database.h"
#include "Entity.h"

// Đại diện cho MỘT BẢNG chứa các đối tượng kiểu T (T kế thừa Entity,
// có constructor mặc định) và GIỮ TOÀN BỘ chúng trên RAM.
//
//  - load()  : đọc file TXT -> tạo đối tượng (gọi MỘT LẦN lúc khởi động, xem DbContext::loadAll)
//  - get     : findAll / findByKey / findBy ... chỉ duyệt trên RAM, KHÔNG đọc lại file
//  - modify  : service sửa thẳng đối tượng trên RAM rồi gọi update() -> ghi file
//  - add/remove: sửa RAM + cập nhật quan hệ hai chiều + ghi file
//
// DbSet là chủ sở hữu duy nhất của các đối tượng (new trong load/add, delete khi remove/hủy).
// Nơi khác chỉ giữ con trỏ T* (không delete).
template<class T>
class DbSet : public Repository<T>
{
private:
    Database* db;
    DbContext* context;          // truyền cho entity khi liên kết quan hệ
    MyList<T*> items;

    string table() const
    {
        T sample;
        return sample.tableName();
    }

    int columnIndex(const string& column) const
    {
        T sample;
        Row cols = sample.columns();
        for (int i = 0; i < cols.getSize(); i++)
            if (cols[i] == column) return i;
        return -1;
    }

    void clearItems()
    {
        for (int i = 0; i < items.getSize(); i++) delete items[i];
        items.clear();
    }

public:
    DbSet(Database* database, DbContext* ctx) : db(database), context(ctx) {}
    ~DbSet() { clearItems(); }

    // Không cho sao chép (sở hữu con trỏ)
    DbSet(const DbSet&) = delete;
    DbSet& operator=(const DbSet&) = delete;

    // ---------- Giao diện Repository ----------
    // file -> RAM (chưa liên kết quan hệ). Chỉ gọi lúc khởi động,
    // vì gọi lại sẽ làm các con trỏ quan hệ cũ bị treo.
    void load()
    {
        clearItems();
        T sample;
        int n = sample.columns().getSize();

        MyList<Row> rows = db->readTable(table());
        for (int i = 0; i < rows.getSize(); i++)
        {
            Row r = rows[i];
            while (r.getSize() < n) r.add("");      // dòng thiếu cột -> bù rỗng
            T* obj = new T();
            obj->fromRow(r);
            items.add(obj);
        }
    }

    // RAM -> file
    void save()
    {
        MyList<Row> rows;
        for (int i = 0; i < items.getSize(); i++) rows.add(items[i]->toRow());
        db->writeTable(table(), rows);
    }

    // Thiết lập quan hệ hai chiều cho mọi đối tượng (gọi sau khi TẤT CẢ bảng đã load)
    void linkAll()
    {
        for (int i = 0; i < items.getSize(); i++) items[i]->linkRelations(context);
    }

    void add(T* obj)                                 // INSERT
    {
        items.add(obj);
        obj->linkRelations(context);                 // gắn vào các đối tượng liên quan (hai chiều)
        save();
    }

    void update(T* obj)                              // UPDATE: obj đã được sửa trên RAM -> ghi file
    {
        (void)obj;
        save();
    }

    void remove(const string& key)                   // DELETE theo khóa chính
    {
        T* obj = findByKey(key);
        if (obj == NULL) return;

        obj->removeChildren(context);                // Composition: xóa các đối tượng con trước
        obj->unlinkRelations();                      // gỡ liên kết hai chiều

        for (int i = 0; i < items.getSize(); i++)
        {
            if (items[i] == obj) { items.remove(i); break; }
        }
        delete obj;
        save();
    }

    // ---------- Truy vấn trên RAM ----------
    const MyList<T*>& all() const { return items; }  // SELECT *
    int count() const { return items.getSize(); }

    T* findByKey(const string& key) const            // WHERE khóa = key; NULL nếu không có
    {
        for (int i = 0; i < items.getSize(); i++)
            if (items[i]->getKey() == key) return items[i];
        return NULL;
    }

    MyList<T*> findBy(const string& column, const string& value) const   // WHERE cột = giá trị
    {
        MyList<T*> result;
        int idx = columnIndex(column);
        if (idx < 0) return result;
        for (int i = 0; i < items.getSize(); i++)
        {
            Row r = items[i]->toRow();
            if (r[idx] == value) result.add(items[i]);
        }
        return result;
    }

    T* findOneBy(const string& column, const string& value) const
    {
        MyList<T*> r = findBy(column, value);
        return r.isEmpty() ? NULL : r[0];
    }

    int countBy(const string& column, const string& value) const
    {
        return findBy(column, value).getSize();
    }

    // Sinh khóa tiếp theo: nextId("DK", 3) -> DK001, DK002, ...
    string nextId(const string& prefix, int width) const
    {
        int mx = 0;
        for (int i = 0; i < items.getSize(); i++)
        {
            string key = items[i]->getKey();
            if (key.size() > prefix.size())
            {
                int n = toInt(key.substr(prefix.size()), 0);
                if (n > mx) mx = n;
            }
        }
        return prefix + pad(mx + 1, width);
    }
};

#endif
