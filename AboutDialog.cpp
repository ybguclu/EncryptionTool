#include "AboutDialog.h"
#include "CryptoManager.h"
#include <objidl.h>
#include <gdiplus.h>
#include <shellapi.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "shell32.lib")

void AboutDialog::Show(HWND hParent, HINSTANCE hInstance) {
    // GDI+ başlat (PNG/JPG fotoğraf çizimi ve yuvarlak kenar yumuşatma için)
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken = 0;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    const wchar_t CLASS_NAME[] = L"EncryptionToolAboutDialog";

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = AboutDialog::DialogProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.hIcon = LoadIcon(nullptr, IDI_INFORMATION);

    RegisterClassExW(&wc);

    // Ana pencerenin ortasını hesapla
    int dlgWidth = 460;
    int dlgHeight = 430;
    RECT rcParent = {};
    GetWindowRect(hParent, &rcParent);
    int x = rcParent.left + ((rcParent.right - rcParent.left) - dlgWidth) / 2;
    int y = rcParent.top + ((rcParent.bottom - rcParent.top) - dlgHeight) / 2;

    // Ana pencereyi geçici olarak kilitle (Modal Dialog davranışı)
    EnableWindow(hParent, FALSE);

    HWND hDlg = CreateWindowExW(
        WS_EX_DLGMODALFRAME,
        CLASS_NAME,
        L"Hakkında - Geliştirici Bilgileri",
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE,
        x, y, dlgWidth, dlgHeight,
        hParent,
        nullptr,
        hInstance,
        nullptr
    );

    // Modal pencere mesaj döngüsü
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        if (!IsDialogMessage(hDlg, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // Ana pencereyi tekrar aktif et ve öne getir
    EnableWindow(hParent, TRUE);
    SetForegroundWindow(hParent);

    Gdiplus::GdiplusShutdown(gdiplusToken);
}

LRESULT CALLBACK AboutDialog::DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HFONT hFontBtn = nullptr;

    switch (uMsg) {
    case WM_CREATE: {
        hFontBtn = CreateFontW(16, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
            DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

        HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);

        // Alt Butonlar
        HWND hBtnWeb = CreateWindowExW(0, L"BUTTON", L"🌐 GitHub / Web Sitem",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
            25, 335, 145, 34, hwnd, (HMENU)ID_BTN_WEBSITE, hInst, nullptr);
        SendMessageW(hBtnWeb, WM_SETFONT, (WPARAM)hFontBtn, TRUE);

        HWND hBtnUpdate = CreateWindowExW(0, L"BUTTON", L"🔄 Sürüm Kontrolü",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
            180, 335, 135, 34, hwnd, (HMENU)ID_BTN_UPDATE, hInst, nullptr);
        SendMessageW(hBtnUpdate, WM_SETFONT, (WPARAM)hFontBtn, TRUE);

        HWND hBtnClose = CreateWindowExW(0, L"BUTTON", L"Kapat",
            WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
            325, 335, 95, 34, hwnd, (HMENU)ID_BTN_CLOSE, hInst, nullptr);
        SendMessageW(hBtnClose, WM_SETFONT, (WPARAM)hFontBtn, TRUE);

        return 0;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawDialog(hwnd, hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        if (wmId == ID_BTN_WEBSITE) {
            ShellExecuteW(hwnd, L"open", DEV_WEBSITE, nullptr, nullptr, SW_SHOWNORMAL);
            return 0;
        }
        else if (wmId == ID_BTN_UPDATE) {
            CheckForUpdates(hwnd);
            return 0;
        }
        else if (wmId == ID_BTN_CLOSE || wmId == IDCANCEL) {
            DestroyWindow(hwnd);
            return 0;
        }
        break;
    }

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        if (hFontBtn) {
            DeleteObject(hFontBtn);
            hFontBtn = nullptr;
        }
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void AboutDialog::DrawDialog(HWND hwnd, HDC hdc) {
    RECT rc;
    GetClientRect(hwnd, &rc);

    Gdiplus::Graphics g(hdc);
    g.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

    // 1. Arka Plan ve Üst Koyu Lacivert Banner
    Gdiplus::SolidBrush bgBrush(Gdiplus::Color(255, 245, 247, 250));
    g.FillRectangle(&bgBrush, 0, 0, rc.right, rc.bottom);

    Gdiplus::SolidBrush bannerBrush(Gdiplus::Color(255, 26, 43, 76));
    g.FillRectangle(&bannerBrush, 0, 0, rc.right, 115);

    // Banner altındaki mavi şerit
    Gdiplus::SolidBrush accentBrush(Gdiplus::Color(255, 59, 130, 246));
    g.FillRectangle(&accentBrush, 0, 115, rc.right, 3);

    // 2. Yuvarlak Profil Fotoğrafı Çizimi (Sol Üst)
    int avatarX = 28;
    int avatarY = 22;
    int avatarSize = 95;

    // Fotoğrafı yüklemeyi dene (Önce profile.png, bulamazsa profile.jpg dener)
// Akıllı Fotoğraf Bulucu: Hem çalışma dizinini hem de .exe dizinini tüm uzantılarla tarar
    wchar_t exePath[MAX_PATH] = {};
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    std::wstring exeDir(exePath);
    size_t lastSlash = exeDir.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) {
        exeDir = exeDir.substr(0, lastSlash + 1);
    }

    // Bakılacak olası tüm dosya adları (çift uzantı ihtimalleri dahil)
    const wchar_t* fileNames[] = {
        PHOTO_PATH,
        L"profile.png", L"profile.jpg", L"profile.jpeg", L"profile.bmp",
        L"profile.png.png", L"profile.jpg.jpg", L"profile.jpg.jpeg", L"profile.png.jpg"
    };

    // Bakılacak klasörler (Mevcut dizin, .exe dizini ve üst proje dizinleri)
    std::wstring searchDirs[] = {
        L"",
        exeDir,
        exeDir + L"..\\",
        exeDir + L"..\\..\\EncryptionTool\\"
    };

    Gdiplus::Image* img = nullptr;
    for (const auto& dir : searchDirs) {
        for (const auto* name : fileNames) {
            std::wstring fullPath = dir + name;
            Gdiplus::Image* tempImg = Gdiplus::Image::FromFile(fullPath.c_str());
            if (tempImg && tempImg->GetLastStatus() == Gdiplus::Ok) {
                img = tempImg;
                break;
            }
            delete tempImg;
        }
        if (img) break;
    }

    // Yuvarlak kırpma alanı (Circular Clipping Path)
    Gdiplus::GraphicsPath path;
    path.AddEllipse(avatarX, avatarY, avatarSize, avatarSize);

    if (img && img->GetLastStatus() == Gdiplus::Ok) {
        g.SetClip(&path);
        g.DrawImage(img, avatarX, avatarY, avatarSize, avatarSize);
        g.ResetClip();
    }
    else {
        // Eğer henüz fotoğraf konulmadıysa şık bir varsayılan avatar çiz
        Gdiplus::SolidBrush avatarBg(Gdiplus::Color(255, 59, 130, 246));
        g.FillEllipse(&avatarBg, avatarX, avatarY, avatarSize, avatarSize);

        Gdiplus::FontFamily fontFam(L"Segoe UI");
        Gdiplus::Font avatarFont(&fontFam, 28, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
        Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 255, 255, 255));
        Gdiplus::StringFormat sf;
        sf.SetAlignment(Gdiplus::StringAlignmentCenter);
        sf.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        Gdiplus::RectF avatarRect((Gdiplus::REAL)avatarX, (Gdiplus::REAL)avatarY, (Gdiplus::REAL)avatarSize, (Gdiplus::REAL)avatarSize);
        g.DrawString(L"C++", -1, &avatarFont, avatarRect, &sf, &whiteBrush);
    }
    if (img) delete img;

    // Profil fotoğrafı etrafına beyaz/mavi şık çerçeve
    Gdiplus::Pen borderPen(Gdiplus::Color(255, 255, 255, 255), 3.0f);
    g.DrawEllipse(&borderPen, avatarX, avatarY, avatarSize, avatarSize);

    // 3. Geliştirici Adı, Unvanı ve E-posta (Banner Üzerinde, Fotoğrafın Sağında)
    Gdiplus::FontFamily fontFamily(L"Segoe UI");
    Gdiplus::Font fontName(&fontFamily, 22, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::Font fontTitle(&fontFamily, 14, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
    Gdiplus::Font fontBold(&fontFamily, 14, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
    Gdiplus::Font fontText(&fontFamily, 13, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);

    Gdiplus::SolidBrush whiteText(Gdiplus::Color(255, 255, 255, 255));
    Gdiplus::SolidBrush lightBlueText(Gdiplus::Color(255, 180, 205, 240));
    Gdiplus::SolidBrush darkText(Gdiplus::Color(255, 35, 45, 60));
    Gdiplus::SolidBrush grayText(Gdiplus::Color(255, 90, 100, 115));

    g.DrawString(DEV_NAME, -1, &fontName, Gdiplus::PointF(140.0f, 25.0f), &whiteText);
    g.DrawString(DEV_TITLE, -1, &fontTitle, Gdiplus::PointF(142.0f, 56.0f), &lightBlueText);
    g.DrawString(DEV_EMAIL, -1, &fontText, Gdiplus::PointF(142.0f, 80.0f), &lightBlueText);

    // 4. Beyaz Bilgi Kartı (Hakkında & Sürüm Bilgileri)
    Gdiplus::SolidBrush cardBrush(Gdiplus::Color(255, 255, 255, 255));
    Gdiplus::Pen cardBorder(Gdiplus::Color(255, 220, 226, 235), 1.0f);
    g.FillRectangle(&cardBrush, 25, 135, rc.right - 50, 180);
    g.DrawRectangle(&cardBorder, 25, 135, rc.right - 50, 180);

    g.DrawString(L"Geliştirici & Proje Hakkında:", -1, &fontBold, Gdiplus::PointF(40.0f, 148.0f), &darkText);

    Gdiplus::RectF bioRect(40.0f, 173.0f, (Gdiplus::REAL)(rc.right - 80), 65.0f);
    g.DrawString(DEV_BIO, -1, &fontText, bioRect, nullptr, &grayText);

    // Ayırıcı Çizgi
    Gdiplus::Pen linePen(Gdiplus::Color(255, 235, 240, 245), 1.0f);
    g.DrawLine(&linePen, 40, 242, rc.right - 40, 242);

    // Teknik Bilgiler
    std::wstring verInfo = L"• Uygulama Sürümü:  " + std::wstring(APP_VERSION);
    std::wstring sslInfo = L"• Kriptografi Motoru:  " + CryptoManager::GetOpenSSLVersion();
    std::wstring licInfo = L"• Lisans Durumu:  Ücretsiz / Kişisel & Açık Kullanım";

    g.DrawString(verInfo.c_str(), -1, &fontText, Gdiplus::PointF(40.0f, 250.0f), &darkText);
    g.DrawString(sslInfo.c_str(), -1, &fontText, Gdiplus::PointF(40.0f, 270.0f), &darkText);
    g.DrawString(licInfo.c_str(), -1, &fontText, Gdiplus::PointF(40.0f, 290.0f), &darkText);
}

void AboutDialog::CheckForUpdates(HWND hwnd) {
    int res = MessageBoxW(
        hwnd,
        L"✔ Uygulamanız güncel!\r\n\r\n"
        L"Mevcut Sürümünüz: v1.0.1 (x64)\r\n\r\n"
        L"v1.0.1 Yenilikleri:\r\n"
        L"• AES-256-CBC (Encrypt-then-MAC / HMAC-SHA256) eklendi.\r\n"
        L"• Sürümlenmiş Base64 çıktı paketi formatı eklendi.\r\n"
        L"• Geliştirici Hakkında (About) penceresi eklendi.\r\n\r\n"
        L"GitHub proje sayfasını ziyaret etmek ister misiniz?",
        L"Sürüm Kontrolü - v1.0.1",
        MB_YESNO | MB_ICONINFORMATION
    );

    if (res == IDYES) {
        ShellExecuteW(hwnd, L"open", DEV_WEBSITE, nullptr, nullptr, SW_SHOWNORMAL);
    }
}