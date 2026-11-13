#pragma once
#include "ICryptoAlgorithm.h"
#include <memory>
#include <vector>
#include <string>

class CryptoManager {
public:
    CryptoManager();
    ~CryptoManager() = default;

    // Sistemde aktif olarak uygulanmış algoritmaların listesini döndürür
    const std::vector<std::unique_ptr<ICryptoAlgorithm>>& GetAvailableAlgorithms() const;

    // Seçilen algoritma türüne göre şifreleme işlemini yönlendirir
    bool Encrypt(
        AlgorithmType type,
        const std::string& plaintext,
        const std::string& password,
        std::vector<uint8_t>& outData,
        std::wstring& outError
    ) const;

    // OpenSSL bağlantısının çalıştığını doğrulamak için sürüm bilgisini döndürür
    static std::wstring GetOpenSSLVersion();

private:
    std::vector<std::unique_ptr<ICryptoAlgorithm>> m_algorithms;
};