#include <windows.h>
#include "SystemClass.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
	SystemClass& g_System = SystemClass::Instance();
    if (!g_System.Initialize()) {
        return -1;
    }

    // Window starts hidden, shown by timer logic

    MSG msg = {};
    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT)
                return (int)msg.wParam;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // Only render if window is visible
        if (IsWindowVisible(g_System.GetHwnd())) {
            g_System.Run();
        }
    }
    return 0;
}