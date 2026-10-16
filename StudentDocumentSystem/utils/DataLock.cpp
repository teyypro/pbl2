#include "DataLock.h"
#include <io.h>
#include <share.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cerrno>
#include <chrono>
#include <thread>
#include <stdexcept>

using namespace std;
DataLock::DataLock(const string& directory) {
    string path = directory + "/system.lock";
    auto start = chrono::steady_clock::now();
    while (true) {
        // CRT mo file voi che do cam tien trinh khac cung mo doc/ghi.
        // He dieu hanh tu dong dong file khi chuong trinh thoat/bat thuong.
        errno_t result = _sopen_s(&descriptor, path.c_str(), _O_CREAT | _O_RDWR | _O_NOINHERIT,
            _SH_DENYRW, _S_IREAD | _S_IWRITE);
        if (result == 0) return;
        if (result != EACCES || chrono::steady_clock::now() - start > chrono::seconds(15))
            throw runtime_error("Khong lay duoc khoa du lieu. Kiem tra thu muc data hoac thu lai.");
        this_thread::sleep_for(chrono::milliseconds(50));
    }
}
DataLock::~DataLock() {
    if (descriptor >= 0) _close(descriptor);
}
