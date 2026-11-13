#include "AesCbc.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <openssl/crypto.h>
#include <memory>
#include <cstring>

std::wstring AesCbc::GetName() const {
    return L"AES-256-CBC";
}

AlgorithmType AesCbc::GetType() const {
    return AlgorithmType::AES_256_CBC;
}

bool AesCbc::DeriveKeys(
    const std::string& password,
    const uint8_t* salt,
    uint8_t* outEncKey,
    uint8_t* outMacKey
) const {
    // 64 bayt (512-bit) anahtar materyali türetip iki bağımsız 256-bit anahtara bölüyoruz
    uint8_t keyMaterial[ENC_KEY_SIZE + MAC_KEY_SIZE];

    int res = PKCS5_PBKDF2_HMAC(
        password.c_str(),
        static_cast<int>(password.length()),
        salt,
        static_cast<int>(SALT_SIZE),
        PBKDF2_ITER,
        EVP_sha256(),
        static_cast<int>(sizeof(keyMaterial)),
        keyMaterial
    );

    if (res != 1) {
        OPENSSL_cleanse(keyMaterial, sizeof(keyMaterial));
        return false;
    }

    std::memcpy(outEncKey, keyMaterial, ENC_KEY_SIZE);
    std::memcpy(outMacKey, keyMaterial + ENC_KEY_SIZE, MAC_KEY_SIZE);

    OPENSSL_cleanse(keyMaterial, sizeof(keyMaterial));
    return true;
}

bool AesCbc::ComputeHmacSha256(
    const uint8_t* macKey,
    const uint8_t* data,
    size_t dataLen,
    uint8_t* outHmac
) const {
    unsigned int hmacLen = 0;
    unsigned char* res = HMAC(
        EVP_sha256(),
        macKey,
        static_cast<int>(MAC_KEY_SIZE),
        data,
        dataLen,
        outHmac,
        &hmacLen
    );

    return (res != nullptr && hmacLen == HMAC_SIZE);
}

bool AesCbc::Encrypt(
    const std::string& plaintext,
    const std::string& password,
    std::vector<uint8_t>& outData,
    std::wstring& outError
) {
    outData.clear();

    // 1. Kriptografik olarak güvenli rastgele 128-bit Salt ve 128-bit IV üret
    uint8_t salt[SALT_SIZE];
    uint8_t iv[IV_SIZE];

    if (RAND_bytes(salt, sizeof(salt)) != 1 || RAND_bytes(iv, sizeof(iv)) != 1) {
        outError = L"OpenSSL kriptografik rastgele sayı (Salt/IV) üretemedi.";
        return false;
    }

    // 2. Birbirinden bağımsız AES-256 anahtarı ve HMAC-SHA256 anahtarı türet
    uint8_t encKey[ENC_KEY_SIZE];
    uint8_t macKey[MAC_KEY_SIZE];

    if (!DeriveKeys(password, salt, encKey, macKey)) {
        outError = L"Paroladan AES ve HMAC anahtarları türetilemedi.";
        return false;
    }

    // 3. OpenSSL EVP bağlamını oluştur
    std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> ctx(
        EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free
    );

    if (!ctx) {
        OPENSSL_cleanse(encKey, sizeof(encKey));
        OPENSSL_cleanse(macKey, sizeof(macKey));
        outError = L"OpenSSL şifreleme bağlamı (EVP_CIPHER_CTX) oluşturulamadı.";
        return false;
    }

    // 4. AES-256-CBC şifrelemesini başlat
    if (EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_cbc(), nullptr, encKey, iv) != 1) {
        OPENSSL_cleanse(encKey, sizeof(encKey));
        OPENSSL_cleanse(macKey, sizeof(macKey));
        outError = L"AES-256-CBC algoritması başlatılamadı.";
        return false;
    }

    // PKCS#7 Padding'i açıkça aktif et (OpenSSL'de varsayılan olarak da açıktır)
    EVP_CIPHER_CTX_set_padding(ctx.get(), 1);

    // AES anahtarı bağlama yüklendiği için RAM'den temizle
    OPENSSL_cleanse(encKey, sizeof(encKey));

    // 5. Metni şifrele (PKCS#7 blok dolgusu için ekstra 1 blok [16 bayt] yer ayır)
    std::vector<uint8_t> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0;
    int ciphertextLen = 0;

    if (!plaintext.empty()) {
        if (EVP_EncryptUpdate(
            ctx.get(),
            ciphertext.data(),
            &len,
            reinterpret_cast<const uint8_t*>(plaintext.data()),
            static_cast<int>(plaintext.size())
        ) != 1) {
            OPENSSL_cleanse(macKey, sizeof(macKey));
            outError = L"AES-256-CBC şifreleme sırasında hata oluştu (EVP_EncryptUpdate).";
            return false;
        }
        ciphertextLen = len;
    }

    // 6. PKCS#7 Padding bloğunu ekle ve şifrelemeyi tamamla
    if (EVP_EncryptFinal_ex(ctx.get(), ciphertext.data() + ciphertextLen, &len) != 1) {
        OPENSSL_cleanse(macKey, sizeof(macKey));
        outError = L"AES-256-CBC PKCS#7 padding sonlandırılamadı (EVP_EncryptFinal_ex).";
        return false;
    }
    ciphertextLen += len;
    ciphertext.resize(ciphertextLen);

    // 7. Sürümlenmiş Paket Gövdesini Hazırla: [Version(1B)] + [AlgoID(1B)] + [Salt(16B)] + [IV(16B)] + [Ciphertext]
    outData.reserve(2 + SALT_SIZE + IV_SIZE + ciphertext.size() + HMAC_SIZE);
    outData.push_back(FORMAT_VERSION);
    outData.push_back(ALGO_ID_CBC);
    outData.insert(outData.end(), salt, salt + SALT_SIZE);
    outData.insert(outData.end(), iv, iv + IV_SIZE);
    outData.insert(outData.end(), ciphertext.begin(), ciphertext.end());

    // 8. Encrypt-then-MAC: Tüm paket gövdesi (Başlık + Salt + IV + Ciphertext) üzerinden HMAC-SHA256 hesapla
    uint8_t hmac[HMAC_SIZE];
    bool macSuccess = ComputeHmacSha256(macKey, outData.data(), outData.size(), hmac);

    // HMAC anahtarını RAM'den güvenli şekilde temizle
    OPENSSL_cleanse(macKey, sizeof(macKey));

    if (!macSuccess) {
        outData.clear();
        outError = L"HMAC-SHA256 doğrulama etiketi hesaplanamadı.";
        return false;
    }

    // 9. HMAC-SHA256 etiketini paketin sonuna ekle
    outData.insert(outData.end(), hmac, hmac + HMAC_SIZE);

    return true;
}