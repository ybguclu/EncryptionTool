#pragma once
#include <string>
#include <vector>
#include <cstdint>

// Sistemde desteklenen ve gelecekte eklenecek algoritmaların kimlikleri
enum class AlgorithmType {
    AES_256_GCM,
    // Gelecek aşamalarda eklenecek algoritmalar (Mimari hazırlığı):
    AES_128,
    AES_192,
    AES_256_CBC,
    CHACHA20_POLY1305,
    TRIPLE_DES,
    BLOWFISH,
    TWOFISH,
    SERPENT
};

// Tüm şifreleme sınıflarının türeyeceği ortak arayüz (Interface)
class ICryptoAlgorithm {
public:
    virtual ~ICryptoAlgorithm() = default;

    // Algoritmanın arayüzde (ListBox) görünecek adı
    virtual std::wstring GetName() const = 0;

    // Algoritmanın sistem içi kimliği
    virtual AlgorithmType GetType() const = 0;

    // Şifreleme fonksiyonu:
    // Girdi olarak düz metin (UTF-8) ve parola alır, çıktı olarak paketlenmiş ham şifreli veriyi (outData) üretir.
    virtual bool Encrypt(
        const std::string& plaintext,
        const std::string& password,
        std::vector<uint8_t>& outData,
        std::wstring& outError
    ) = 0;
};