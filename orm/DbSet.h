#ifndef DBSET_H
#define DBSET_H

#include "../templates/Repository.h"
#include "Database.h"
#include "Entity.h"

// Đại diện cho MỘT BẢNG chứa các đối tượng kiểu T (T kế thừa Entity,
// có constructor mặc định). Mỗi thao tác đều: đọc file -> xử lý -> ghi file.
//
// Các hàm find* có tham số `relations` (mặc định true):
//   true  = nạp sẵn Navigation Property của đối tượng trả về (eager loading, 1 cấp)
//   false = không nạp quan hệ (nhanh hơn, dùng khi chỉ cần dữ liệu của chính bảng)
template<class T>
class DbSet : public Repository<T>
{
private:
    Database* db;
    DbContext* context;          // để entity nạp quan hệ (xem Entity::loadRelations)
    MyList<T> items;

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

    void loadRelationsOf(MyList<T>& list)
    {
        for (int i = 0; i < list.getSize(); i++) list[i].loadRelations(context);
    }

public:
    DbSet(Database* database, DbContext* ctx) : db(database), context(ctx) {}

    // ---------- Giao diện Repository (không nạp quan hệ) ----------
    void load()
    {
        items.clear();
        T sample;
        int n = sample.columns().getSize();

        MyList<Row> rows = db->readTable(table());
        for (int i = 0; i < rows.getSize(); i++)
        {
            Row r = rows[i];
            while (r.getSize() < n) r.add("");      // dòng thiếu cột -> bù rỗng
            T obj;
            obj.fromRow(r);
            items.add(obj);
        }
    }

    void save()
    {
        MyList<Row> rows;
        for (int i = 0; i < items.getSize(); i++) rows.add(items[i].toRow());
        db->writeTable(table(), rows);
    }

    void add(const T& obj)                           // INSERT
    {
        load();
        items.add(obj);
        save();
    }

    void update(const T& obj)                        // UPDATE theo khóa chính
    {
        load();
        for (int i = 0; i < items.getSize(); i++)
        {
            if (items[i].getKey() == obj.getKey()) { items[i] = obj; break; }
        }
        save();
    }

    // ---------- Mở rộng của ORM ----------
    void remove(const string& key)                   // DELETE theo khóa chính
    {
        load();
        for (int i = 0; i < items.getSize(); i++)
        {
            if (items[i].getKey() == key) { items.remove(i); break; }
        }
        save();
    }

    MyList<T> findAll(bool relations = true)         // SELECT *
    {
        load();
        MyList<T> result = items;
        if (relations) loadRelationsOf(result);
        return result;
    }

    bool findByKey(const string& key, T& out, bool relations = true)   // WHERE khóa = key
    {
        load();
        for (int i = 0; i < items.getSize(); i++)
        {
            if (items[i].getKey() == key)
            {
                out = items[i];
                if (relations) out.loadRelations(context);
                return true;
            }
        }
        return false;
    }

    MyList<T> findBy(const string& column, const string& value, bool relations = true)   // WHERE cột = giá trị
    {
        load();
        MyList<T> result;
        int idx = columnIndex(column);
        if (idx < 0) return result;
        for (int i = 0; i < items.getSize(); i++)
        {
            Row r = items[i].toRow();
            if (r[idx] == value) result.add(items[i]);
        }
        if (relations) loadRelationsOf(result);
        return result;
    }

    bool findOneBy(const string& column, const string& value, T& out, bool relations = true)
    {
        MyList<T> r = findBy(column, value, false);
        if (r.isEmpty()) return false;
        out = r[0];
        if (relations) out.loadRelations(context);
        return true;
    }

    int countBy(const string& column, const string& value)
    {
        return findBy(column, value, false).getSize();
    }

    // Sinh khóa tiếp theo: nextId("DK", 3) -> DK001, DK002, ...
    string nextId(const string& prefix, int width)
    {
        load();
        int mx = 0;
        for (int i = 0; i < items.getSize(); i++)
        {
            string key = items[i].getKey();
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