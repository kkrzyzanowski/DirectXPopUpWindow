#include "GUIButton.h"
#include <d3dcompiler.h>

#pragma comment(lib, "d3dcompiler.lib")

GUIButton::GUIButton(const std::wstring& text, float x, float y, float width, float height)
	: m_text(text), m_position(x, y), m_size(width, height), m_clicked(false), m_hovered(false),
	m_vertexBuffer(nullptr), m_indexBuffer(nullptr), m_vertexShader(nullptr),
	m_pixelShader(nullptr), m_inputLayout(nullptr), m_samplerState(nullptr), m_matrixBuffer(nullptr), m_buttonBuffer(nullptr),
	m_normalColor(0.2f, 0.2f, 0.8f, 1.0f),
	m_hoverColor(0.3f, 0.3f, 1.0f, 1.0f),
	m_clickColor(0.1f, 0.1f, 0.6f, 1.0f)
{
}

GUIButton::~GUIButton()
{
	Shutdown();
}

bool GUIButton::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	float screenWidth = 800.0f;
	float screenHeight = 100.0f;

	// Uproszczona struktura wierzcho³ka - tylko pozycja
	struct SimpleVertex
	{
		DirectX::XMFLOAT3 position;
	};

	SimpleVertex vertices[] =
	{
		{ DirectX::XMFLOAT3(0.0f, 0.0f, 0.5f) },
		{ DirectX::XMFLOAT3(1.0f, 0.0f, 0.5f) },
		{ DirectX::XMFLOAT3(0.0f, 1.0f, 0.5f) },
		{ DirectX::XMFLOAT3(1.0f, 1.0f, 0.5f) }
	};
	m_matrixBufferData =
	{
		DirectX::XMMatrixOrthographicLH(screenWidth, screenHeight, 0.1f, 100.0f)
	};

	D3D11_BUFFER_DESC matrixBufferDesc = {};
	matrixBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	matrixBufferDesc.ByteWidth = sizeof(m_matrixBufferData);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA matrixBufferInitData = {};
	matrixBufferInitData.pSysMem = &m_matrixBufferData;

	if (FAILED(device->CreateBuffer(&matrixBufferDesc, &matrixBufferInitData, &m_matrixBuffer)))
	{
		return false;
	}

	D3D11_BUFFER_DESC vertexBufferDesc = {};
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(vertices);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices;

	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &initData, &m_vertexBuffer))) {
		return false;
	}

	D3D11_BUFFER_DESC buttonBufferDesc = {};
	buttonBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	buttonBufferDesc.ByteWidth = sizeof(ButtonBufferData);
	buttonBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	ButtonBufferData buttonBufferData = {
		m_position,
		m_size,
		m_normalColor
	};

	D3D11_SUBRESOURCE_DATA buttonInitData = {};
	buttonInitData.pSysMem = &buttonBufferData;

	if (FAILED(device->CreateBuffer(
		&buttonBufferDesc,
		&buttonInitData,
		&m_buttonBuffer)))
	{
		return false;
	}

	deviceContext->VSSetConstantBuffers(1, 1, &m_buttonBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &m_buttonBuffer);

	// Uproszczony vertex shader - tylko pozycja
	const char* vsCode = R"(
    cbuffer MatrixBuffer : register(b0)
    {
        float4x4 projection;
    };

    cbuffer ButtonBuffer : register(b1)
    {
        float2 buttonPosition;
        float2 buttonSize;
        float4 normalColor;
    };

    struct VS_INPUT
    {
        float3 position : POSITION;
    };

    struct PS_INPUT
    {
        float4 position : SV_POSITION;
		float2 localUV : TEXCOORD0;
    };

    PS_INPUT main(VS_INPUT input)
    {
        PS_INPUT output;

        float2 pixelPosition =
            buttonPosition + input.position.xy * buttonSize;

        float2 centeredPosition = float2(
            pixelPosition.x - 400.0f,
            50.0f - pixelPosition.y
        );

        output.position = mul(
            float4(centeredPosition, input.position.z, 1.0f),
            projection
        );

		output.localUV = input.position.xy;
        return output;
    }
)";

	// Uproszczony pixel shader - zwraca sta³y kolor
	const char* psCode = R"(
    cbuffer ColorBuffer : register(b1)
    {
        float2 buttonPosition;
        float2 buttonSize;
        float4 normalColor;
    };

    struct PS_INPUT
    {
        float4 position : SV_POSITION;
		float2 localUV : TEXCOORD0;
    };

    float4 main(PS_INPUT input) : SV_TARGET
    {
return input.localUV.x >= 1.0f - (2.0f / buttonSize.x)
    ? float4(1, 1, 1, 1)
    : normalColor;
    }
)";

	ID3DBlob* vertexBlob = nullptr;
	ID3DBlob* pixelBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	// Kompiluj vertex shader
	if (FAILED(D3DCompile(vsCode, strlen(vsCode), nullptr, nullptr, nullptr,
		"main", "vs_5_0", 0, 0, &vertexBlob, &errorBlob))) {
		if (errorBlob) {
			OutputDebugStringA("VERTEX SHADER ERROR: ");
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			OutputDebugStringA("\n");
			errorBlob->Release();
		}
		return false;
	}

	// Kompiluj pixel shader
	if (FAILED(D3DCompile(psCode, strlen(psCode), nullptr, nullptr, nullptr,
		"main", "ps_5_0", 0, 0, &pixelBlob, &errorBlob))) {
		if (errorBlob) {
			OutputDebugStringA("PIXEL SHADER ERROR: ");
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			OutputDebugStringA("\n");
			errorBlob->Release();
		}
		if (vertexBlob) vertexBlob->Release();
		return false;
	}

	HRESULT hr1 = device->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), nullptr, &m_vertexShader);
	HRESULT hr2 = device->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), nullptr, &m_pixelShader);

	if (FAILED(hr1)) {
		OutputDebugStringA("FAILED TO CREATE VERTEX SHADER\n");
		return false;
	}
	if (FAILED(hr2)) {
		OutputDebugStringA("FAILED TO CREATE PIXEL SHADER\n");
		return false;
	}

	// Uproszczony input layout - tylko pozycja
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(device->CreateInputLayout(layout, 1, vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), &m_inputLayout))) {
		OutputDebugStringA("FAILED TO CREATE INPUT LAYOUT\n");
		return false;
	}

	vertexBlob->Release();
	pixelBlob->Release();

	return true;
}

void GUIButton::Shutdown()
{
	if (m_vertexBuffer) m_vertexBuffer->Release();
	if (m_indexBuffer) m_indexBuffer->Release();
	if (m_vertexShader) m_vertexShader->Release();
	if (m_pixelShader) m_pixelShader->Release();
	if (m_inputLayout) m_inputLayout->Release();
	if (m_samplerState) m_samplerState->Release();
	if (m_matrixBuffer) m_matrixBuffer->Release();
	if (m_buttonBuffer) m_buttonBuffer->Release();
}

void GUIButton::Render(ID3D11DeviceContext* deviceContext)
{
	// Ustaw shader i layout
	deviceContext->IASetInputLayout(m_inputLayout);
	deviceContext->VSSetShader(m_vertexShader, nullptr, 0);
	deviceContext->PSSetShader(m_pixelShader, nullptr, 0);

	// Ustaw vertex buffer
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// Ustaw constant buffer
	deviceContext->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
	deviceContext->PSSetConstantBuffers(0, 1, &m_matrixBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, &m_buttonBuffer);
	deviceContext->PSSetConstantBuffers(1, 1, &m_buttonBuffer);

	// Renderuj
	deviceContext->Draw(4, 0);
}

bool GUIButton::IsMouseOver(float mouseX, float mouseY) const
{
	float left = m_position.x;
	float right = m_position.x + m_size.x;
	float top = m_position.y;
	float bottom = m_position.y + m_size.y;

	return mouseX >= left && mouseX <= right &&
		mouseY >= top && mouseY <= bottom;
}