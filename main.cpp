#include <windows.h>
#include "MainWindow.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    // Kullanılmayan parametre uyarılarını bastır
    (void)hPrevInstance;
    (void)pCmdLine;

    MainWindow mainWindow(hInstance);

    if (!mainWindow.Create()) {
        MessageBoxW(nullptr, L"Ana pencere oluşturulamadı!", L"Kritik Hata", MB_OK | MB_ICONERROR);
        return -1;
    }

    mainWindow.Show(nCmdShow);

    // Win32 Mesaj Döngüsü
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(msg.wParam);
}