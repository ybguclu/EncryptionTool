#pragma once
#include <vector>
#include <string>
#include <cstdint>

class Base64 {
public:
    // Ham bayt verisini Base64 formatında geniş karakterli (Unicode) metne dönüştürür
    static std::wstring Encode(const std::vector<uint8_t>& data);
};