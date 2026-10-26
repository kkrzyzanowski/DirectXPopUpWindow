#pragma once

#include <vector>
#include <functional>
#include "GUIButton.h"

class ButtonActionManager
{
public:
    void Register(GUIButton* button, std::function<void()> action)
    {
        if (button)
            button->SetOnClick(action);
    }

    void HandleClickedButtons()
    {
        for (auto* button : m_buttons)
        {
            if (button && button->WasClicked())
                button->InvokeClick();
        }
    }

    void AddButton(GUIButton* button)
    {
        if (button)
            m_buttons.push_back(button);
    }

private:
    std::vector<GUIButton*> m_buttons;
};