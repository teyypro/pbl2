#include "DocumentService.h"

DocumentService::DocumentService(DbContext& d) : db(d) {}

MyList<Phong> DocumentService::getAllPhong()
{
    return db.phong.findAll(false);          // chỉ cần danh sách phòng, không cần quan hệ
}

// Navigation 1-N: Phong -> các VanBan của phòng
MyList<VanBan> DocumentService::getVanBanByPhong(const string& maPHG)
{
    MyList<VanBan> result;
    Phong p;
    if (db.phong.findByKey(maPHG, p)) result = p.vanBans.items();
    return result;
}