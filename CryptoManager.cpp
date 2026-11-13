#include "CryptoManager.h"
#include "AesGcm.h"
#include "AesCbc.h"
#include <openssl/crypto.h>

CryptoManager::CryptoManager() {
    // Yalnızca aktif olarak uygulanmış algoritmaları kaydediyoruz.
    // Henüz uygulanmayan algoritmalar ListBox'a eklenmez.
    m_algorithms.push_back(std::make_unique<AesGcm>());
    m_algorithms.push_back(std::make_unique<AesCbc>());
}

const std::vector<std::unique_ptr<ICryptoAlgorithm>>& CryptoManager::GetAvailableAlgorithms() const {
    return m_algorithms;
}

bool CryptoManager::Encrypt(
    AlgorithmType type,
    const std::string& plaintext,
    const std::string& password,
    std::vector<uint8_t>& outData,
    std::wstring& outError
) const {
    for (const auto& algo : m_algorithms) {
        if (algo->GetType() == type) {
            return algo->Encrypt(plaintext, password, outData, outError);
        }
    }

    outError = L"Seçilen şifreleme algoritması henüz uygulanmadı veya bulunamadı.";
    return false;
}

std::wstring CryptoManager::GetOpenSSLVersion() {
    const char* version = OpenSSL_version(OPENSSL_VERSION);
    if (!version) {
        return L"Bilinmeyen OpenSSL Sürümü";
    }
    std::string verStr(version);
    return std::wstring(verStr.begin(), verStr.end());
}