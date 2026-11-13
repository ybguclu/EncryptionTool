#include "MainWindow.h"
#include "Base64.h"
#include "AboutDialog.h"
#include <commctrl.h>
#include <cstring>

#pragma comment(lib, "comctl32.lib")

// Modern Windows 10/11 Görsel Stilleri (Visual Styles)
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

MainWindow::MainWindow(HINSTANCE hInstance)
    : m_hInstance(hInstance), m_hwnd(nullptr),
    m_hFontTitle(nullptr), m_hFontSubTitle(nullptr),
    m_hFontBold(nullptr), m_hFontNormal(nullptr), m_hFontMono(nullptr),
    m_hBrushBg(nullptr), m_hBrushOutputBg(nullptr),
    m_hEditInput(nullptr), m_hListAlgo(nullptr), m_hEditPassword(nullptr),
    m_hChkShowPass(nullptr), m_hBtnEncrypt(nullptr), m_hBtnClear(nullptr),
    m_hEditOutput(nullptr), m_hBtnCopy(nullptr), m_hStatusBar(nullptr),
    m_hBtnAbout(nullptr) {
}

MainWindow::~MainWindow() {
    if (m_hFontTitle)     DeleteObject(m_hFontTitle);
    if (m_hFontSubTitle)  DeleteObject(m_hFontSubTitle);
    if (m_hFontBold)      DeleteObject(m_hFontBold);
    if (m_hFontNormal)    DeleteObject(m_hFontNormal);
    if (m_hFontMono)      DeleteObject(m_hFontMono);
    if (m_hBrushBg)       DeleteObject(m_hBrushBg);
    if (m_hBrushOutputBg) DeleteObject(m_hBrushOutputBg);
}

bool MainWindow::Create() {
    INITCOMMONCONTROLSEX icex = {};
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES | ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    const wchar_t CLASS_NAME[] = L"EncryptionToolWindowClass";

    m_hBrushBg = CreateSolidBrush(RGB(244, 246, 249));
    m_hBrushOutputBg = CreateSolidBrush(RGB(235, 240, 245));

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = MainWindow::WindowProc;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = m_hBrushBg;
    wc.hIcon = LoadIcon(nullptr, IDI_SHIELD);
    wc.hIconSm = LoadIcon(nullptr, IDI_SHIELD);

    if (!RegisterClassExW(&wc)) {
        return false;
    }

    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    m_hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"C++ Encryption Tool",
        dwStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, 580, 650,
        nullptr,
        nullptr,
        m_hInstance,
        this
    );

    if (!m_hwnd) {
        return false;
    }

    CenterWindow();
    return true;
}

void MainWindow::CenterWindow() {
    RECT rcWin = {};
    GetWindowRect(m_hwnd, &rcWin);
    int winWidth = rcWin.right - rcWin.left;
    int winHeight = rcWin.bottom - rcWin.top;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int x = (screenWidth - winWidth) / 2;
    int y = (screenHeight - winHeight) / 2;

    SetWindowPos(m_hwnd, nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
}

void MainWindow::Show(int nCmdShow) {
    ShowWindow(m_hwnd, nCmdShow);
    UpdateWindow(m_hwnd);
}

LRESULT CALLBACK MainWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    MainWindow* pThis = nullptr;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = reinterpret_cast<MainWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        pThis->m_hwnd = hwnd;
    }
    else {
        pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    if (pThis) {
        return pThis->HandleMessage(uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        CreateControls();
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);
        DrawHeader(hdc);
        EndPaint(m_hwnd, &ps);
        return 0;
    }

    case WM_CTLCOLORSTATIC: {
        HDC hdcStatic = (HDC)wParam;
        HWND hCtrl = (HWND)lParam;

        if (hCtrl == m_hEditOutput) {
            SetTextColor(hdcStatic, RGB(20, 45, 76));
            SetBkColor(hdcStatic, RGB(235, 240, 245));
            return (INT_PTR)m_hBrushOutputBg;
        }

        SetTextColor(hdcStatic, RGB(35, 45, 60));
        SetBkColor(hdcStatic, RGB(244, 246, 249));
        return (INT_PTR)m_hBrushBg;
    }

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        if (wmEvent == BN_CLICKED) {
            switch (wmId) {
            case ID_BTN_ENCRYPT:
                OnEncryptClicked();
                return 0;
            case ID_BTN_CLEAR:
                OnClearClicked();
                return 0;
            case ID_BTN_COPY:
                OnCopyClicked();
                return 0;
            case ID_CHK_SHOWPASS:
                OnTogglePassword();
                return 0;
            case ID_BTN_ABOUT:
                OnAboutClicked();
                return 0;
            }
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void MainWindow::DrawHeader(HDC hdc) {
    RECT rcClient = {};
    GetClientRect(m_hwnd, &rcClient);

    // Üst Başlık Bandı (Koyu Lacivert #1A2B4C)
    RECT rcHeader = { 0, 0, rcClient.right, 68 };
    HBRUSH hHeaderBrush = CreateSolidBrush(RGB(26, 43, 76));
    FillRect(hdc, &rcHeader, hHeaderBrush);
    DeleteObject(hHeaderBrush);

    // Alt ince vurgu çizgisi (Mavi #3B82F6)
    RECT rcAccent = { 0, 68, rcClient.right, 71 };
    HBRUSH hAccentBrush = CreateSolidBrush(RGB(59, 130, 246));
    FillRect(hdc, &rcAccent, hAccentBrush);
    DeleteObject(hAccentBrush);

    SetBkMode(hdc, TRANSPARENT);

    // Ana Başlık
    SelectObject(hdc, m_hFontTitle);
    SetTextColor(hdc, RGB(255, 255, 255));
    TextOutW(hdc, 24, 12, L"C++ Encryption Tool", 19);

    // Alt Başlık
    SelectObject(hdc, m_hFontSubTitle);
    SetTextColor(hdc, RGB(180, 200, 230));
    TextOutW(hdc, 25, 40, L"OpenSSL Tabanlı Güvenli Metin Şifreleme Aracı", 45);
}

void MainWindow::CreateControls() {
    m_hFontTitle = CreateFontW(24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    m_hFontSubTitle = CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    m_hFontBold = CreateFontW(17, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    m_hFontNormal = CreateFontW(17, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");

    m_hFontMono = CreateFontW(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY,
        FIXED_PITCH | FF_MODERN, L"Consolas");

    auto SetFont = [](HWND hCtrl, HFONT hFont) {
        SendMessageW(hCtrl, WM_SETFONT, (WPARAM)hFont, TRUE);
        };

    // --- BÖLÜM 1: Metin Giriş Alanı ---
    HWND hLblInput = CreateWindowExW(0, L"STATIC", L"1. Şifrelenecek Metin:",
        WS_CHILD | WS_VISIBLE, 24, 86, 250, 22, m_hwnd, nullptr, m_hInstance, nullptr);
    SetFont(hLblInput, m_hFontBold);

    m_hEditInput = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_WANTRETURN,
        24, 112, 516, 105, m_hwnd, (HMENU)ID_EDIT_INPUT, m_hInstance, nullptr);
    SetFont(m_hEditInput, m_hFontNormal);
    SendMessageW(m_hEditInput, EM_SETCUEBANNER, TRUE, (LPARAM)L"Şifrelemek istediğiniz gizli metni buraya yazın...");

    // --- BÖLÜM 2: Algoritma ve Parola Yan Yana Düzen ---
    HWND hLblAlgo = CreateWindowExW(0, L"STATIC", L"2. Şifreleme Algoritması:",
        WS_CHILD | WS_VISIBLE, 24, 232, 220, 22, m_hwnd, nullptr, m_hInstance, nullptr);
    SetFont(hLblAlgo, m_hFontBold);

    m_hListAlgo = CreateWindowExW(WS_EX_CLIENTEDGE, L"LISTBOX", L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | LBS_NOTIFY,
        24, 258, 230, 68, m_hwnd, (HMENU)ID_LIST_ALGO, m_hInstance, nullptr);
    SetFont(m_hListAlgo, m_hFontNormal);

    // CryptoManager üzerinden aktif algoritmaları (AES-256-GCM ve AES-256-CBC) yükle
    const auto& algorithms = m_cryptoManager.GetAvailableAlgorithms();
    for (const auto& algo : algorithms) {
        LRESULT index = SendMessageW(m_hListAlgo, LB_ADDSTRING, 0, (LPARAM)algo->GetName().c_str());
        SendMessageW(m_hListAlgo, LB_SETITEMDATA, index, static_cast<LPARAM>(algo->GetType()));
    }
    SendMessageW(m_hListAlgo, LB_SETCURSEL, 0, 0);

    // Parola Alanı (Sağ Sütun)
    HWND hLblPass = CreateWindowExW(0, L"STATIC", L"3. Şifreleme Anahtarı (Parola):",
        WS_CHILD | WS_VISIBLE, 274, 232, 260, 22, m_hwnd, nullptr, m_hInstance, nullptr);
    SetFont(hLblPass, m_hFontBold);

    m_hEditPassword = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_PASSWORD | ES_AUTOHSCROLL,
        274, 258, 266, 28, m_hwnd, (HMENU)ID_EDIT_PASSWORD, m_hInstance, nullptr);
    SetFont(m_hEditPassword, m_hFontNormal);
    SendMessageW(m_hEditPassword, EM_SETCUEBANNER, TRUE, (LPARAM)L"Parolanızı girin...");

    // Parolayı Göster Checkbox'ı
    m_hChkShowPass = CreateWindowExW(0, L"BUTTON", L"Parolayı Göster",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_AUTOCHECKBOX,
        274, 294, 160, 24, m_hwnd, (HMENU)ID_CHK_SHOWPASS, m_hInstance, nullptr);
    SetFont(m_hChkShowPass, m_hFontNormal);

    // --- BÖLÜM 3: Aksiyon Butonları (Şifrele & Temizle) ---
    m_hBtnEncrypt = CreateWindowExW(0, L"BUTTON", L"🔒  Metni Şifrele",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON | BS_DEFPUSHBUTTON,
        24, 340, 380, 40, m_hwnd, (HMENU)ID_BTN_ENCRYPT, m_hInstance, nullptr);
    SetFont(m_hBtnEncrypt, m_hFontBold);

    m_hBtnClear = CreateWindowExW(0, L"BUTTON", L"Temizle",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        414, 340, 126, 40, m_hwnd, (HMENU)ID_BTN_CLEAR, m_hInstance, nullptr);
    SetFont(m_hBtnClear, m_hFontNormal);

    // --- BÖLÜM 4: Şifreli Metin Çıktı Alanı ---
    HWND hLblOutput = CreateWindowExW(0, L"STATIC", L"Şifreli Çıktı (Sürümlenmiş Base64 Paketi):",
        WS_CHILD | WS_VISIBLE, 24, 398, 500, 22, m_hwnd, nullptr, m_hInstance, nullptr);
    SetFont(hLblOutput, m_hFontBold);

    m_hEditOutput = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
        24, 424, 516, 95, m_hwnd, (HMENU)ID_EDIT_OUTPUT, m_hInstance, nullptr);
    SetFont(m_hEditOutput, m_hFontMono);

    // --- BÖLÜM 5: Panoya Kopyala Butonu ---
    m_hBtnCopy = CreateWindowExW(0, L"BUTTON", L"📋  Şifreli Metni Panoya Kopyala",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        24, 530, 516, 38, m_hwnd, (HMENU)ID_BTN_COPY, m_hInstance, nullptr);
    SetFont(m_hBtnCopy, m_hFontBold);

    // --- BÖLÜM 6: Alt Durum Çubuğu (Status Bar) ---
    m_hStatusBar = CreateWindowExW(0, STATUSCLASSNAMEW, nullptr,
        WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, m_hwnd, (HMENU)ID_STATUS_BAR, m_hInstance, nullptr);
    SetFont(m_hStatusBar, m_hFontSubTitle);

    std::wstring initStatus = L" Hazır  |  Motor: " + CryptoManager::GetOpenSSLVersion();
    SetStatusText(initStatus);

    // --- BÖLÜM 7: Üst Başlık Bandındaki "Hakkında" Butonu ---
    m_hBtnAbout = CreateWindowExW(0, L"BUTTON", L"ℹ Hakkında",
        WS_CHILD | WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON,
        435, 20, 105, 30, m_hwnd, (HMENU)ID_BTN_ABOUT, m_hInstance, nullptr);
    SetFont(m_hBtnAbout, m_hFontBold);
}

void MainWindow::SetStatusText(const std::wstring& text) {
    if (m_hStatusBar) {
        SendMessageW(m_hStatusBar, SB_SETTEXTW, 0, (LPARAM)text.c_str());
    }
}

void MainWindow::OnTogglePassword() {
    LRESULT checked = SendMessageW(m_hChkShowPass, BM_GETCHECK, 0, 0);
    if (checked == BST_CHECKED) {
        SendMessageW(m_hEditPassword, EM_SETPASSWORDCHAR, 0, 0);
    }
    else {
        SendMessageW(m_hEditPassword, EM_SETPASSWORDCHAR, (WPARAM)L'●', 0);
    }
    InvalidateRect(m_hEditPassword, nullptr, TRUE);
}

void MainWindow::OnClearClicked() {
    SetWindowTextW(m_hEditInput, L"");
    SetWindowTextW(m_hEditPassword, L"");
    SetWindowTextW(m_hEditOutput, L"");
    SetStatusText(L" Tüm alanlar temizlendi.");
    SetFocus(m_hEditInput);
}

void MainWindow::OnAboutClicked() {
    AboutDialog::Show(m_hwnd, m_hInstance);
}

std::wstring MainWindow::GetControlText(HWND hCtrl) const {
    int len = GetWindowTextLengthW(hCtrl);
    if (len <= 0) {
        return L"";
    }
    std::wstring text(len + 1, L'\0');
    GetWindowTextW(hCtrl, &text[0], len + 1);
    text.resize(len);
    return text;
}

std::string MainWindow::WideToUtf8(const std::wstring& wstr) const {
    if (wstr.empty()) {
        return "";
    }
    int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    if (sizeNeeded <= 0) {
        return "";
    }
    std::string utf8Str(sizeNeeded, '\0');
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &utf8Str[0], sizeNeeded, nullptr, nullptr);
    return utf8Str;
}

bool MainWindow::CopyToClipboard(const std::wstring& text) const {
    if (!OpenClipboard(m_hwnd)) {
        return false;
    }

    EmptyClipboard();

    size_t byteSize = (text.size() + 1) * sizeof(wchar_t);
    HGLOBAL hGlob = GlobalAlloc(GMEM_MOVEABLE, byteSize);
    if (!hGlob) {
        CloseClipboard();
        return false;
    }

    void* pMem = GlobalLock(hGlob);
    if (pMem) {
        std::memcpy(pMem, text.c_str(), byteSize);
        GlobalUnlock(hGlob);
    }

    if (!SetClipboardData(CF_UNICODETEXT, hGlob)) {
        GlobalFree(hGlob);
        CloseClipboard();
        return false;
    }

    CloseClipboard();
    return true;
}

void MainWindow::OnEncryptClicked() {
    std::wstring inputWide = GetControlText(m_hEditInput);
    std::wstring passWide = GetControlText(m_hEditPassword);

    if (inputWide.empty()) {
        SetStatusText(L" ⚠ Uyarı: Şifrelenecek metin alanı boş!");
        MessageBoxW(m_hwnd, L"Lütfen şifrelenecek bir metin girin.", L"Eksik Bilgi", MB_OK | MB_ICONWARNING);
        SetFocus(m_hEditInput);
        return;
    }

    if (passWide.empty()) {
        SetStatusText(L" ⚠ Uyarı: Parola alanı boş!");
        MessageBoxW(m_hwnd, L"Lütfen bir şifreleme anahtarı (parola) girin.", L"Eksik Bilgi", MB_OK | MB_ICONWARNING);
        SetFocus(m_hEditPassword);
        return;
    }

    LRESULT selIndex = SendMessageW(m_hListAlgo, LB_GETCURSEL, 0, 0);
    if (selIndex == LB_ERR) {
        MessageBoxW(m_hwnd, L"Lütfen listeden bir şifreleme algoritması seçin.", L"Eksik Seçim", MB_OK | MB_ICONWARNING);
        SetFocus(m_hListAlgo);
        return;
    }

    LRESULT itemData = SendMessageW(m_hListAlgo, LB_GETITEMDATA, selIndex, 0);
    AlgorithmType selectedAlgo = static_cast<AlgorithmType>(itemData);

    // Seçilen algoritmanın adını ListBox'tan al
    wchar_t algoNameBuf[64] = {};
    SendMessageW(m_hListAlgo, LB_GETTEXT, selIndex, (LPARAM)algoNameBuf);
    std::wstring selectedAlgoName(algoNameBuf);

    std::string plaintext = WideToUtf8(inputWide);
    std::string password = WideToUtf8(passWide);

    std::vector<uint8_t> encryptedPacket;
    std::wstring errorMsg;

    bool success = m_cryptoManager.Encrypt(selectedAlgo, plaintext, password, encryptedPacket, errorMsg);

    // RAM'den hassas verileri temizle
    if (!passWide.empty())  SecureZeroMemory(&passWide[0], passWide.size() * sizeof(wchar_t));
    if (!password.empty())  SecureZeroMemory(&password[0], password.size());
    if (!plaintext.empty()) SecureZeroMemory(&plaintext[0], plaintext.size());

    if (!success) {
        SetWindowTextW(m_hEditOutput, L"");
        SetStatusText(L" ❌ Şifreleme sırasında hata oluştu.");
        MessageBoxW(m_hwnd, errorMsg.c_str(), L"Şifreleme Hatası", MB_OK | MB_ICONERROR);
        return;
    }

    std::wstring base64Output = Base64::Encode(encryptedPacket);

    // 6. Madde Gereksinimi: Seçilen algoritmanın adını çıktı alanında da belirt
    std::wstring formattedOutput = L"[" + selectedAlgoName + L"]\r\n" + base64Output;
    SetWindowTextW(m_hEditOutput, formattedOutput.c_str());

    std::wstring statusMsg = L" ✔ Başarıyla şifrelendi (" + selectedAlgoName
        + L" | Toplam Paket: " + std::to_wstring(encryptedPacket.size()) + L" bayt)";
    SetStatusText(statusMsg);
}

void MainWindow::OnCopyClicked() {
    std::wstring outputText = GetControlText(m_hEditOutput);

    if (outputText.empty()) {
        SetStatusText(L" ⚠ Kopyalanacak şifreli metin yok.");
        MessageBoxW(m_hwnd, L"Kopyalanacak şifreli metin bulunamadı. Lütfen önce bir metni şifreleyin.", L"Uyarı", MB_OK | MB_ICONWARNING);
        return;
    }

    if (CopyToClipboard(outputText)) {
        SetStatusText(L" ✔ Şifreli Base64 metni panoya kopyalandı!");
    }
    else {
        SetStatusText(L" ❌ Panoya kopyalama başarısız oldu.");
        MessageBoxW(m_hwnd, L"Metin panoya kopyalanırken bir sistem hatası oluştu.", L"Hata", MB_OK | MB_ICONERROR);
    }
}