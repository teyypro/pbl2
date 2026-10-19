#ifndef DOCUMENTSERVICE_H
#define DOCUMENTSERVICE_H

#include "../data/Repositories.h"

class DocumentService
{
private:
    PhongRepo& phongRepo;
    VanBanRepo& vbRepo;

public:
    DocumentService(PhongRepo& p, VanBanRepo& v);

    MyList<Phong> getAllPhong();

    MyList<VanBan> getVanBanByPhong(const string& maPHG);

    bool findPhongByUsername(const string& username, Phong& out);
};

#endif
