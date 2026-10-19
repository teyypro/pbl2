#include "AuthService.h"

AuthService::AuthService(TaiKhoanRepo& r) : repo(r) {}

bool AuthService::login(const string& username, const string& password, TaiKhoan& out)
{
    repo.load();
    const MyList<TaiKhoan>& all = repo.getAll();
    for (int i = 0; i < all.getSize(); i++)
    {
        if (all[i].getUsername() == username && all[i].getPassword() == password)
        {
            out = all[i];
            return true;
        }
    }
    return false;
}
