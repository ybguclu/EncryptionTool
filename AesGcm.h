#pragma once
#include "ICryptoAlgorithm.h"

class AesGcm : public ICryptoAlgorithm {
public:
    AesGcm() = default;
    ~AesGcm() override = default;

    std::wstring GetName() const override;
    AlgorithmType GetType() const override;

    bool Encrypt(
        const std::string& plaintext,
        const std::string& password,
        std::vector<uint8_t>& outData,
        std::wstring& outError
    ) override;

private:
    // Kriptografik sabitler (Bayt cinsinden)
    static constexpr size_t SALT_SIZE = 16; // 128-bit Salt (PBKDF2 anahtar türetimi için)
    static constexpr size_t NONCE_SIZE = 12; // 96-bit Nonce / IV (GCM standardı)
    static constexpr size_t KEY_SIZE = 32; // 256-bit AES Anahtarı
    static constexpr size_t TAG_SIZE = 16; // 128-bit GCM Authentication Tag
    static constexpr int PBKDF2_ITER = 100000; // Anahtar türetme iterasyon sayısı

    // Paroladan güvenli 256-bit anahtar türeten yardımcı fonksiyon (PBKDF2-HMAC-SHA256)
    bool DeriveKey(
        const std::string& password,
        const uint8_t* salt,
        uint8_t* outKey
    ) const;
};