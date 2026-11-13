#pragma once
#include <windows.h>
#include <string>

class AboutDialog {
public:
    // Hakkında penceresini ana pencerenin (hParent) ortasında açar
    static void Show(HWND hParent, HINSTANCE hInstance);

private:
    static LRESULT CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static void DrawDialog(HWND hwnd, HDC hdc);
    static void CheckForUpdates(HWND hwnd);

    // ========================================================================
    // KİŞİSEL BİLGİLERİNİ BURADAN DÜZENLEYEBİLİRSİN:
    // ========================================================================
    static constexpr const wchar_t* PHOTO_PATH = L"profile.jpg"; // Proje klasöründeki fotoğrafının adı (profile.png veya profile.jpg)
    static constexpr const wchar_t* DEV_NAME = L"Yunus Güçlü";
    static constexpr const wchar_t* DEV_TITLE = L"C++ Yazılım Geliştirici";
    static constexpr const wchar_t* DEV_EMAIL = L"yunusberkan125@gmail.com";
    static constexpr const wchar_t* DEV_BIO = L"Bu uygulama, Windows üzerinde yüksek güvenlikli ve hızlı metin şifreleme ihtiyacı için Native C++17 ve OpenSSL kullanılarak geliştirilmiştir. Tamamen ücretsizdir.";
    static constexpr const wchar_t* DEV_WEBSITE = L"https://github.com/ybguclu";
    static constexpr const wchar_t* APP_VERSION = L"v1.0.0 (x64)";

    // Buton Kimlikleri
    static constexpr int ID_BTN_WEBSITE = 201;
    static constexpr int ID_BTN_UPDATE = 202;
    static constexpr int ID_BTN_CLOSE = 203;
};