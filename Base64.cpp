#include "Base64.h"
#include <openssl/evp.h>

std::wstring Base64::Encode(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        return L"";
    }

    // Base64 çıktısı her 3 bayt için 4 karakter üretir (+1 null-terminator için)
    size_t encodedLen = 4 * ((data.size() + 2) / 3);
    std::string buffer(encodedLen + 1, '\0');

    int actualLen = EVP_EncodeBlock(
        reinterpret_cast<unsigned char*>(&buffer[0]),
        data.data(),
        static_cast<int>(data.size())
    );

    if (actualLen <= 0) {
        return L"";
    }

    buffer.resize(actualLen);
    return std::wstring(buffer.begin(), buffer.end());
}