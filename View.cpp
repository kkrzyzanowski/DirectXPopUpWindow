#include "View.h"

void View::InitializeView(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_buttons = new GUIButton * [m_numberOfButtons];
	for (int i = 0; i < m_numberOfButtons; ++i) {
		m_buttons[i] = new GUIButton(L"Test", i * 100.0f, 0.0f, 100.0f, 100.0f);
		m_buttons[i]->Initialize(device, deviceContext);
	}

	// przyklad: pierwszy button = otworz ten komputer
	m_buttons[0]->SetOnClick([]()
		{
			ShellExecuteW(nullptr, L"open", L"explorer.exe", L"shell:MyComputerFolder", nullptr, SW_SHOWNORMAL);
		});

	// drugi button = otworz dysk C:
	m_buttons[1]->SetOnClick([]()
		{
			ShellExecuteW(nullptr, L"open", L"explorer.exe", L"C:\\", nullptr, SW_SHOWNORMAL);
		});

	// trzeci button = otworz katalog projektu
	m_buttons[2]->SetOnClick([]()
		{
			ShellExecuteW(nullptr, L"open", L"explorer.exe", L"C:\\DyskD\\ProjectsCpp\\DirectXPopUpWindow", nullptr, SW_SHOWNORMAL);
		});
}


void View::RenderView(ID3D11DeviceContext* deviceContext)
{
	if (!m_buttons) {
		OutputDebugStringA("ERROR: m_buttons is NULL!\n");
	}
	else {
		for (int i = 0; i < m_numberOfButtons; ++i) {
			m_buttons[i]->Render(deviceContext);
		}
	}
}

void View::HandleMouseClick(float mouseX, float mouseY)
{
	for (int i = 0; i < m_numberOfButtons; ++i)
	{
		if (!m_buttons[i]) continue;

		if (m_buttons[i]->IsMouseOver(mouseX, mouseY))
		{
			m_buttons[i]->SetClicked(true);
			m_buttons[i]->InvokeClick(); // wywołanie akcji
			return;
		}
	}
}


