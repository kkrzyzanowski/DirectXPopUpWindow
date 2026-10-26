#pragma once
#include "GUIButton.h"
#include <d3d11.h>

class View
{
public:
	View() = default;
	~View() = default;
	void InitializeView(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void RenderView(ID3D11DeviceContext* deviceContext);
	void HandleMouseClick(float mouseX, float mouseY);
private:
	GUIButton** m_buttons;
	int m_numberOfButtons = 8;
};

