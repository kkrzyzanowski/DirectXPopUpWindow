#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <string>
#include <functional>

class GUIButton
{
public:

    using ClickHandler = std::function<void()> ;

    GUIButton(const std::wstring& text, float x, float y, float width, float height);
    ~GUIButton();

    bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
    void Shutdown();
    void Render(ID3D11DeviceContext* deviceContext);

    bool IsMouseOver(float mouseX, float mouseY) const;
    bool WasClicked() { bool clicked = m_clicked; m_clicked = false; return clicked; }
    void SetClicked(bool state) { m_clicked = state; }


    void SetOnClick(ClickHandler handler) { m_clickHandler = std::move(handler); }
    void InvokeClick() { if (m_clickHandler) m_clickHandler(); }

    DirectX::XMFLOAT2 GetPosition() const { return m_position; }
    DirectX::XMFLOAT2 GetSize() const { return m_size; }

private:
    struct Vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT4 color;
        DirectX::XMFLOAT2 texCoord;
    };

    struct ButtonBufferData
    {
        DirectX::XMFLOAT2 buttonPosition;
        DirectX::XMFLOAT2 buttonSize;
        DirectX::XMFLOAT4 normalColor;
	};

    struct MatrixBufferType
    {
        DirectX::XMMATRIX projection;
    };

    std::wstring m_text;
    DirectX::XMFLOAT2 m_position;
    DirectX::XMFLOAT2 m_size;
    bool m_clicked;
    bool m_hovered;

    ID3D11Buffer* m_vertexBuffer;
    ID3D11Buffer* m_indexBuffer;
    ID3D11VertexShader* m_vertexShader;
    ID3D11PixelShader* m_pixelShader;
    ID3D11InputLayout* m_inputLayout;
    ID3D11SamplerState* m_samplerState;
    ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_buttonBuffer;

    DirectX::XMFLOAT4 m_normalColor;
    DirectX::XMFLOAT4 m_hoverColor;
    DirectX::XMFLOAT4 m_clickColor;

	MatrixBufferType m_matrixBufferData;

	ClickHandler m_clickHandler;
};