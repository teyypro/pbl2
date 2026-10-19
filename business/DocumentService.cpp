#include "DocumentService.h"

DocumentService::DocumentService(PhongRepo& p, VanBanRepo& v) : phongRepo(p), vbRepo(v) {}

MyList<Phong> DocumentService::getAllPhong()
{
    phongRepo.load();
    return phongRepo.getAll();
}

MyList<VanBan> DocumentService::getVanBanByPhong(const string& maPHG)
{
    vbRepo.load();
    MyList<VanBan> r;
    const MyList<VanBan>& all = vbRepo.getAll();
    for (int i = 0; i < all.getSize(); i++)
        if (all[i].getMaPHG() == maPHG) r.add(all[i]);
    return r;
}

bool DocumentService::findPhongByUsername(const string& username, Phong& out)
{
    phongRepo.load();
    const MyList<Phong>& all = phongRepo.getAll();
    for (int i = 0; i < all.getSize(); i++)
    {
        if (all[i].getUsername() == username) { out = all[i]; return true; }
    }
    return false;
}
