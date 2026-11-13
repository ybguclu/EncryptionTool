#pragma once
#include <windows.h>
#include <string>
#include "CryptoManager.h"

class MainWindow {
public:
    MainWindow(HINSTANCE hInstance);
    ~MainWindow();

    bool Create();
    void Show(int nCmdShow);

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void CreateControls();
    void CenterWindow();
    void DrawHeader(HDC hdc);
    void SetStatusText(const std::wstring& text);

    void OnEncryptClicked();
    void OnCopyClicked();
    void OnClearClicked();
    void OnTogglePassword();
    void OnAboutClicked();

    std::wstring GetControlText(HWND hCtrl) const;
    std::string WideToUtf8(const std::wstring& wstr) const;
    bool CopyToClipboard(const std::wstring& text) const;

    HINSTANCE m_hInstance;
    HWND m_hwnd;

    // Yazı Tipleri ve Fırçalar
    HFONT m_hFontTitle;
    HFONT m_hFontSubTitle;
    HFONT m_hFontBold;
    HFONT m_hFontNormal;
    HFONT m_hFontMono;
    HBRUSH m_hBrushBg;
    HBRUSH m_hBrushOutputBg;

    CryptoManager m_cryptoManager;

    // Arayüz Bileşenleri
    HWND m_hEditInput;
    HWND m_hListAlgo;
    HWND m_hEditPassword;
    HWND m_hChkShowPass;
    HWND m_hBtnEncrypt;
    HWND m_hBtnClear;
    HWND m_hEditOutput;
    HWND m_hBtnCopy;
    HWND m_hStatusBar;
    HWND m_hBtnAbout;

    // Kontrol Kimlikleri (ID)
    static constexpr int ID_EDIT_INPUT = 101;
    static constexpr int ID_LIST_ALGO = 102;
    static constexpr int ID_EDIT_PASSWORD = 103;
    static constexpr int ID_CHK_SHOWPASS = 104;
    static constexpr int ID_BTN_ENCRYPT = 105;
    static constexpr int ID_BTN_CLEAR = 106;
    static constexpr int ID_EDIT_OUTPUT = 107;
    static constexpr int ID_BTN_COPY = 108;
    static constexpr int ID_STATUS_BAR = 109;
    static constexpr int ID_BTN_ABOUT = 110;
};