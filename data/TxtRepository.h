#ifndef TXTREPOSITORY_H
#define TXTREPOSITORY_H

#include "../templates/Repository.h"
#include "../templates/MyList.h"
#include "FileManager.h"

// Repository tổng quát lưu vào file TXT.
// Yêu cầu kiểu T có: T::fromLine(string), toLine() const, getKey() const
template<class T>
class TxtRepository : public Repository<T>
{
private:
    string filePath;
    MyList<T> items;

public:
    TxtRepository(const string& path) : filePath(path) {}

    void load()
    {
        items.clear();
        MyList<string> lines = FileManager::readLines(filePath);
        for (int i = 0; i < lines.getSize(); i++) items.add(T::fromLine(lines[i]));
    }

    void save()
    {
        MyList<string> lines;
        for (int i = 0; i < items.getSize(); i++) lines.add(items[i].toLine());
        FileManager::writeLines(filePath, lines);
    }

    void add(const T& obj)
    {
        items.add(obj);
        save();
    }

    // Cập nhật bản ghi có cùng getKey()
    void update(const T& obj)
    {
        for (int i = 0; i < items.getSize(); i++)
        {
            if (items[i].getKey() == obj.getKey())
            {
                items[i] = obj;
                save();
                return;
            }
        }
    }

    const MyList<T>& getAll() const { return items; }

    bool findByKey(const string& key, T& out) const
    {
        for (int i = 0; i < items.getSize(); i++)
        {
            if (items[i].getKey() == key) { out = items[i]; return true; }
        }
        return false;
    }
};

#endif
