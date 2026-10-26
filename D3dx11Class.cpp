#include "D3dx11Class.h"

D3dx11Class::D3dx11Class()
{

}

D3dx11Class::D3dx11Class(const D3dx11Class& other)
{
}

D3dx11Class::~D3dx11Class()
{
}

bool D3dx11Class::InitializeDirectX(int screenWidth, int screenHeight, HWND hwnd)
{
	HRESULT result;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	unsigned int numerator, denominator;
	float fieldOfView, screenAspect;
	m_numberOfButtons = 8;

	// Initialize the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = hwnd;

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;

	swapChainDesc.Windowed = TRUE;

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// Don't set the advanced flags.
	swapChainDesc.Flags = 0;

	// Set the feature level to DirectX 11.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// Get the pointer to the back buffer.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// Create the render target view with the back buffer pointer.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// Release pointer to the back buffer as we no longer need it.
	backBufferPtr->Release();
	backBufferPtr = 0;

	// Setup the viewport for rendering.
	m_viewport.Width = screenWidth;
	m_viewport.Height = screenHeight;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;

	// Create the viewport.
	m_deviceContext->RSSetViewports(1, &m_viewport);

	// **DODAJ TO:**
	rasterDesc = {};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;  // WY£¥CZ CULLING
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.DepthBias = 0;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.ScissorEnable = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.AntialiasedLineEnable = false;

	m_device->CreateRasterizerState(&rasterDesc, &m_rasterizerState);
	m_deviceContext->RSSetState(m_rasterizerState);

	// Wy³¹cz depth testing
	depthStencilDesc = {};
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

	m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);

	m_view = new View();
	m_view->InitializeView(m_device, m_deviceContext);

	return true;
}

void D3dx11Class::Shutdown()
{
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	if (m_swapChain)
	{
		m_swapChain->SetFullscreenState(false, NULL);
	}

	if (m_renderTargetView)
	{
		m_renderTargetView->Release();
		m_renderTargetView = 0;
	}

	if (m_deviceContext)
	{
		m_deviceContext->Release();
		m_deviceContext = 0;
	}

	if (m_device)
	{
		m_device->Release();
		m_device = 0;
	}

	if (m_swapChain)
	{
		m_swapChain->Release();
		m_swapChain = 0;
	}

	if (m_rasterizerState) {
		m_rasterizerState->Release();
		m_rasterizerState = nullptr;
	}
	if (m_depthStencilState) {
		m_depthStencilState->Release();
		m_depthStencilState = nullptr;
	}

	return;
}

void D3dx11Class::BeginScene(float red, float green, float blue, float alpha)
{
	// Clear to cornflower blue
	float clearColor[4] = { red, green, blue, alpha };

	// Ustaw render target
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, nullptr);

	// Clear the back buffer.
	m_deviceContext->ClearRenderTargetView(m_renderTargetView, clearColor);

	// Diagnostyka
	m_view->RenderView(m_deviceContext);
}

void D3dx11Class::EndScene()
{
	// Present buffer - TYLKO RAZ NA KLATKÊ
	m_swapChain->Present(0, 0);
}

ID3D11Device* D3dx11Class::GetDevice()
{
	return m_device;
}

ID3D11DeviceContext* D3dx11Class::GetDeviceContext()
{
	return m_deviceContext;
}

void D3dx11Class::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
	return;
}


void D3dx11Class::ResetViewport()
{
	// Set the viewport.
	m_deviceContext->RSSetViewports(1, &m_viewport);

	return;
}
