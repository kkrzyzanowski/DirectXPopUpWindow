#pragma once
#include <windows.h>
#include "D3dx11Class.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.3f;
const int TIMER_ID = 1;
const int TIMER_INTERVAL = 50; // ms

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool EndFrame();
	void Release();
	D3dx11Class* GetDirect3D() { return m_Direct3D; }
private:
	bool Render();
	D3dx11Class* m_Direct3D;

};


