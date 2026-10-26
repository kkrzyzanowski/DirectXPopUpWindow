#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "ApplicationClass.h"
#include "InputClass.h"

class SystemClass
{
public:
	static SystemClass& Instance()
	{
		static SystemClass instance;
		return instance;
	}

	// Delete copy/move constructors and assignment operators
	SystemClass(const SystemClass&) = delete;
	SystemClass& operator=(const SystemClass&) = delete;
	SystemClass(SystemClass&&) = delete;
	SystemClass& operator=(SystemClass&&) = delete;

	bool Initialize();
	void Shutdown();
	void Run();
	ApplicationClass* GetApplication() { return m_Application; }
	bool GetVisibility() { return isVisible; }
	void SetVisibility(bool val) { isVisible = val; }
	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	HWND GetHwnd() const { return m_hwnd; }

private:
	SystemClass();
	~SystemClass();

	bool Frame();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	bool isVisible = false;

	InputClass* m_Input;
	ApplicationClass* m_Application;
};

static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
    SystemClass& system = SystemClass::Instance();
    switch (umessage)
    {
        case WM_CREATE:
            SetTimer(hwnd, TIMER_ID, TIMER_INTERVAL, nullptr);
            ShowWindow(hwnd, SW_HIDE);
            break;
        case WM_TIMER: {
            POINT pt;
            GetCursorPos(&pt);
            if (pt.y == 0 && !system.GetVisibility()) {
                ShowWindow(hwnd, SW_SHOW);
                system.SetVisibility(true);
            }
            else if (pt.y > 100 && system.GetVisibility()) { // Hide if mouse leaves window area
                ShowWindow(hwnd, SW_HIDE);
                system.SetVisibility(false);
            }
            break;
        }
        case WM_DESTROY:
            KillTimer(hwnd, TIMER_ID);
            system.GetApplication()->Release();
            PostQuitMessage(0);
            break;

            // All other messages pass to the message handler in the system class.
        default:
        {
            return system.MessageHandler(hwnd, umessage, wparam, lparam);
        }
    }
    return 0;
}
