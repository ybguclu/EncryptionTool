#include "AesGcm.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <memory>

std::wstring AesGcm::GetName() const {
    return L"AES-256-GCM";
}

AlgorithmType AesGcm::GetType() const {
    return AlgorithmType::AES_256_GCM;
}

bool AesGcm::DeriveKey(const std::string& password, const uint8_t* salt, uint8_t* outKey) const {
    // Kullanıcı parolasını doğrudan anahtar yapmak yerine PBKDF2-HMAC-SHA256 ile 256-bit anahtar türetiyoruz
    int res = PKCS5_PBKDF2_HMAC(
        password.c_str(),
        static_cast<int>(password.length()),
        salt,
        static_cast<int>(SALT_SIZE),
        PBKDF2_ITER,
        EVP_sha256(),
        static_cast<int>(KEY_SIZE),
        outKey
    );
    return (res == 1);
}

bool AesGcm::Encrypt(
    const std::string& plaintext,
    const std::string& password,
    std::vector<uint8_t>& outData,
    std::wstring& outError
) {
    outData.clear();

    // 1. Güvenli rastgele Salt (128-bit) ve Nonce/IV (96-bit) üret
    uint8_t salt[SALT_SIZE];
    uint8_t nonce[NONCE_SIZE];

    if (RAND_bytes(salt, sizeof(salt)) != 1 || RAND_bytes(nonce, sizeof(nonce)) != 1) {
        outError = L"OpenSSL kriptografik rastgele sayı (Salt/Nonce) üretemedi.";
        return false;
    }

    // 2. Paroladan 256-bit anahtar türet
    uint8_t key[KEY_SIZE];
    if (!DeriveKey(password, salt, key)) {
        outError = L"Paroladan şifreleme anahtarı türetilemedi.";
        return false;
    }

    // 3. OpenSSL EVP Cipher bağlamını (Context) oluştur (RAII ile otomatik bellek yönetimi)
    std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> ctx(
        EVP_CIPHER_CTX_new(), EVP_CIPHER_CTX_free
    );

    if (!ctx) {
        OPENSSL_cleanse(key, sizeof(key));
        outError = L"OpenSSL şifreleme bağlamı (EVP_CIPHER_CTX) oluşturulamadı.";
        return false;
    }

    // 4. AES-256-GCM şifrelemesini başlat
    if (EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1) {
        OPENSSL_cleanse(key, sizeof(key));
        outError = L"AES-256-GCM algoritması başlatılamadı.";
        return false;
    }

    // 5. Nonce (IV) uzunluğunu 96-bit (12 bayt) olarak ayarla
    if (EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_SET_IVLEN, static_cast<int>(NONCE_SIZE), nullptr) != 1) {
        OPENSSL_cleanse(key, sizeof(key));
        outError = L"GCM Nonce/IV uzunluğu ayarlanamadı.";
        return false;
    }

    // 6. Türetilen anahtarı ve Nonce değerini bağlama yükle
    if (EVP_EncryptInit_ex(ctx.get(), nullptr, nullptr, key, nonce) != 1) {
        OPENSSL_cleanse(key, sizeof(key));
        outError = L"Anahtar ve Nonce değerleri yüklenemedi.";
        return false;
    }

    // Anahtar artık OpenSSL bağlamına yüklendiği için RAM'deki kopyasını güvenlice temizle
    OPENSSL_cleanse(key, sizeof(key));

    // 7. Metni şifrele (Ciphertext üretimi)
    std::vector<uint8_t> ciphertext(plaintext.size());
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
            outError = L"Metin şifrelenirken hata oluştu (EVP_EncryptUpdate).";
            return false;
        }
        ciphertextLen = len;
    }

    // 8. Şifrelemeyi sonlandır
    if (EVP_EncryptFinal_ex(ctx.get(), ciphertext.data() + len, &len) != 1) {
        outError = L"Şifreleme sonlandırılamadı (EVP_EncryptFinal_ex).";
        return false;
    }
    ciphertextLen += len;
    ciphertext.resize(ciphertextLen);

    // 9. GCM Authentication Tag (16 bayt) değerini al
    uint8_t tag[TAG_SIZE];
    if (EVP_CIPHER_CTX_ctrl(ctx.get(), EVP_CTRL_GCM_GET_TAG, static_cast<int>(TAG_SIZE), tag) != 1) {
        outError = L"GCM doğrulama etiketi (Authentication Tag) alınamadı.";
        return false;
    }

    // 10. Çıktı paketini oluştur: [Salt (16B)] + [Nonce (12B)] + [Tag (16B)] + [Ciphertext]
 // 10. Sürümlenmiş Çıktı paketini oluştur: [Version(1B)] + [AlgoID(1B)] + [Salt(16B)] + [Nonce(12B)] + [Tag(16B)] + [Ciphertext]
    constexpr uint8_t FORMAT_VERSION = 0x01;
    constexpr uint8_t ALGO_ID_GCM = 0x01;

    outData.reserve(2 + SALT_SIZE + NONCE_SIZE + TAG_SIZE + ciphertext.size());
    outData.push_back(FORMAT_VERSION);
    outData.push_back(ALGO_ID_GCM);
    outData.insert(outData.end(), salt, salt + SALT_SIZE);
    outData.insert(outData.end(), nonce, nonce + NONCE_SIZE);
    outData.insert(outData.end(), tag, tag + TAG_SIZE);
    outData.insert(outData.end(), ciphertext.begin(), ciphertext.end());

    return true;
}