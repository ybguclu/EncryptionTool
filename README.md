# 🔒 C++ Encryption Tool (v1.0.1)

Windows x64 platformu için **Native C++17**, **Win32 API** ve **OpenSSL** kullanılarak geliştirilmiş; hafif, hızlı ve modern bir masaüstü metin şifreleme uygulaması.

Herhangi bir harici ağır framework (Qt, .NET, MFC vb.) kullanmadan doğrudan yerel Windows API'leri ile çalışır.

---

## ✨ Özellikler (v1.0.1)

- **Çift Kriptografik Motor Desteği:**
  - **AES-256-GCM (Authenticated Encryption):** 256-bit anahtar, her işlemde kriptografik rastgele 96-bit Nonce/IV ve 128-bit GCM Authentication Tag.
  - **AES-256-CBC (Encrypt-then-MAC):** 256-bit anahtar, kriptografik rastgele 128-bit IV, PKCS#7 Padding ve şifreli paket bütünlüğü için bağımsız anahtarla üretilen **HMAC-SHA256** doğrulama etiketi.
- **Güvenli Anahtar Türetimi (PBKDF2):**
  - Kullanıcı parolaları doğrudan anahtar olarak kullanılmaz; **128-bit rastgele Salt** ve **PBKDF2-HMAC-SHA256 (100.000 iterasyon)** ile her şifrelemede benzersiz 256-bit anahtarlar türetilir.
- **Sürümlenmiş Base64 Çıktı Paketi:**
  - Üretilen çıktılar algoritma kimliği, sürüm bilgisi, Salt, IV/Nonce, Tag/HMAC ve Ciphertext verilerini tek bir taşınabilir Base64 paketinde birleştirir.
- **RAM Güvenliği:**
  - İşlem tamamlandığında düz metin ve parola kopyaları `SecureZeroMemory` ve `OPENSSL_cleanse` ile bellekten temizlenir.
- **Modern Win32 Arayüzü:**
  - Parola göster/gizle seçeneği, tek tıkla panoya kopyalama, durum çubuğu ve GDI+ destekli geliştirici bilgi ekranı.

---

## 🛠️ Kullanılan Teknolojiler

- **Dil:** C++17 (ISO Standard)
- **Arayüz:** Native Win32 API & GDI+
- **Kriptografi:** OpenSSL 3.x (`libcrypto`)
- **Mimari:** Windows x64

---

## 📦 Kurulum ve Kullanım

1. Sağ taraftaki **[Releases](../../releases)** bölümünden en son sürümü (`EncryptionTool_v1.0.1_x64.zip`) indirin.
2. Arşivi bir klasöre çıkartın ve `EncryptionTool.exe` dosyasını çalıştırın.

---

## 📄 Lisans
Bu proje tamamen ücretsizdir ve eğitim/güvenlik amaçlı açık kaynak olarak paylaşılmıştır.
