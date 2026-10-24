#include "DocumentService.h"

DocumentService::DocumentService(DbContext& d) : db(d) {}

MyList<Phong*> DocumentService::getAllPhong()
{
    return db.phong.all();                   // danh sách con trỏ trên RAM
}

// Navigation 1-N: Phong -> các VanBan của phòng
MyList<VanBan*> DocumentService::getVanBanByPhong(const string& maPHG)
{
    MyList<VanBan*> result;
    Phong* p = db.phong.findByKey(maPHG);
    if (p != NULL) result = p->vanBans.items();
    return result;
}
