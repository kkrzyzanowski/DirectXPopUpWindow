#include "ApplicationClass.h"
ApplicationClass::ApplicationClass()
{
}


ApplicationClass::ApplicationClass(const ApplicationClass& other)
{
}


ApplicationClass::~ApplicationClass()
{
}


bool ApplicationClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	m_Direct3D = new D3dx11Class();
	return m_Direct3D->InitializeDirectX(800, 100, hwnd);
}


void ApplicationClass::Shutdown()
{
	return;
}


bool ApplicationClass::Frame()
{
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	return true;
}

bool ApplicationClass::EndFrame()
{
	m_Direct3D->EndScene();
	return true;
}

void ApplicationClass::Release()
{
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}
	return;
}


bool ApplicationClass::Render()
{
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	return true;
}