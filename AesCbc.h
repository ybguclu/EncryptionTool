#pragma once
#include "ICryptoAlgorithm.h"

class AesCbc : public ICryptoAlgorithm {
public:
    AesCbc() = default;
    ~AesCbc() override = default;

    std::wstring GetName() const override;
    AlgorithmType GetType() const override;

    bool Encrypt(
        const std::string& plaintext,
        const std::string& password,
        std::vector<uint8_t>& outData,
        std::wstring& outError
    ) override;

private:
    // Sürüm ve Algoritma Kimliği Başlığı
    static constexpr uint8_t FORMAT_VERSION = 0x01;
    static constexpr uint8_t ALGO_ID_CBC = 0x02;

    // Kriptografik Sabitler (Bayt cinsinden)
    static constexpr size_t SALT_SIZE = 16; // 128-bit Salt
    static constexpr size_t IV_SIZE = 16; // 128-bit CBC IV
    static constexpr size_t ENC_KEY_SIZE = 32; // 256-bit AES Şifreleme Anahtarı
    static constexpr size_t MAC_KEY_SIZE = 32; // 256-bit Bağımsız HMAC-SHA256 Anahtarı
    static constexpr size_t HMAC_SIZE = 32; // 256-bit HMAC-SHA256 Çıktısı
    static constexpr int    PBKDF2_ITER = 100000;

    // Paroladan birbirinden bağımsız 256-bit AES Anahtarı ve 256-bit HMAC Anahtarı türetir
    bool DeriveKeys(
        const std::string& password,
        const uint8_t* salt,
        uint8_t* outEncKey,
        uint8_t* outMacKey
    ) const;

    // Verilen veri bloğu üzerinde HMAC-SHA256 hesaplar
    bool ComputeHmacSha256(
        const uint8_t* macKey,
        const uint8_t* data,
        size_t dataLen,
        uint8_t* outHmac
    ) const;
};