#include "DbContext.h"

DbContext::DbContext(const string& folder)
    : database_(folder),
      taiKhoan(&database_, this),
      sinhVien(&database_, this),
      phong(&database_, this),
      quay(&database_, this),
      vanBan(&database_, this),
      dangKy(&database_, this),
      phieu(&database_, this)
{
}