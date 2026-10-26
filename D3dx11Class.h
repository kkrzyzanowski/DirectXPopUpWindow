#pragma once
#include <d3d11.h>
#include "GUIButton.h"
#include "View.h"

#pragma comment(lib, "d3d11.lib")
class D3dx11Class
{
public:
    D3dx11Class();
    D3dx11Class(const D3dx11Class&);
    ~D3dx11Class();

    bool InitializeDirectX(int screenWidth, int screenHeight, HWND hwnd);
    void Shutdown();

    void BeginScene(float, float, float, float);
    void EndScene();

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetDeviceContext();

    void GetVideoCardInfo(char*, int&);
    void ResetViewport();
	View* GetView() { return m_view; }
private:
    int m_videoCardMemory;
	int m_numberOfButtons;
    char m_videoCardDescription[128];


    IDXGISwapChain* m_swapChain = nullptr;
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_deviceContext = nullptr;
    ID3D11RenderTargetView* m_renderTargetView = nullptr;
    D3D11_VIEWPORT m_viewport;
	GUIButton** m_button = nullptr;
	ID3D11RasterizerState* m_rasterizerState;
	ID3D11DepthStencilState* m_depthStencilState;

	View* m_view = nullptr;
};



