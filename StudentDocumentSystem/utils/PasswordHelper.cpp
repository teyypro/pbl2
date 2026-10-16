#include "PasswordHelper.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
#include <stdexcept>

using namespace std;
namespace {
    string Hex(const unsigned char* bytes, int size) {
        const char* digits = "0123456789abcdef";
        string output;
        for (int i = 0; i < size; ++i) { output += digits[bytes[i] >> 4]; output += digits[bytes[i] & 15]; }
        return output;
    }
}
bool PasswordHelper::IsHex(const string& value, int length) {
    if (static_cast<int>(value.size()) != length) return false;
    for (char c : value) if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) return false;
    return true;
}
string PasswordHelper::GenerateSalt() {
    unsigned char bytes[16]{};
    if (RAND_bytes(bytes, sizeof(bytes)) != 1)
        throw runtime_error("Khong tao duoc salt.");
    return Hex(bytes, sizeof(bytes));
}
string PasswordHelper::Hash(const string& password, const string& salt) {
    if (!IsHex(salt, 32)) throw runtime_error("Salt khong hop le.");
    unsigned char output[32]{};
    // Goi thu vien OpenSSL, giu nguyen dinh dang hash cua tai khoan cu.
    int result = PKCS5_PBKDF2_HMAC(password.data(), static_cast<int>(password.size()),
        reinterpret_cast<const unsigned char*>(salt.data()), static_cast<int>(salt.size()),
        600000, EVP_sha256(), sizeof(output), output);
    if (result != 1) throw runtime_error("Khong bam duoc mat khau.");
    return Hex(output, sizeof(output));
}
bool PasswordHelper::Verify(const string& password, const string& salt, const string& hash) {
    string actual = Hash(password, salt);
    if (actual.size() != hash.size()) return false;
    return CRYPTO_memcmp(actual.data(), hash.data(), actual.size()) == 0;
}
