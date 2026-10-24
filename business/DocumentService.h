#ifndef DOCUMENTSERVICE_H
#define DOCUMENTSERVICE_H

#include "../orm/DbContext.h"

class DocumentService
{
private:
    DbContext& db;

public:
    DocumentService(DbContext& db);

    MyList<Phong*> getAllPhong();
    MyList<VanBan*> getVanBanByPhong(const string& maPHG);
};

#endif
