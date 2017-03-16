#include "Button.h"

#include "starlight/_incLib/json.hpp"

#include "starlight/InputManager.h"
#include "starlight/GFXManager.h"
#include "starlight/ThemeManager.h"

using starlight::Vector2;
using starlight::Color;

using starlight::gfx::Font;

using starlight::InputManager;
using starlight::GFXManager;
using starlight::ThemeManager;

using starlight::ui::Button;

void Button::SetText(const std::string& text) {
    label = text;
    MarkForRedraw();
}

void Button::Draw() {
    //static auto font = ThemeManager::GetFont("default.12");
    
    static auto idle = ThemeManager::GetAsset("controls/button.idle");
    static auto press = ThemeManager::GetAsset("controls/button.press");
    
    static TextConfig tc = ThemeManager::GetMetric("/controls/button/text", TextConfig());
    
    auto rect = (this->rect + GFXManager::GetOffset()).IntSnap();
    
    if (InputManager::GetDragHandle() == this) {
        press->Draw(rect);
    } else {
        idle->Draw(rect);
    }
    
    //font->Print(rect, label, 1, cl/*Color::white*/, Vector2(0.5f, 0.5f), Color::black);
    tc.Print(rect, label);
}

void Button::OnTouchOn() {
    if (InputManager::Pressed(Keys::Touch)) {
        InputManager::GetDragHandle().Grab(this);
        MarkForRedraw();
    }
}

void Button::OnTouchOff() {
    auto& drag = InputManager::GetDragHandle();
    if (drag == this) drag.Release();
}

void Button::OnDragStart() {
    // do we need to do anything here?
}

void Button::OnDragHold() {
    if (InputManager::TouchDragDist().Length() > InputManager::dragThreshold) {
        InputManager::GetDragHandle().PassUp();
    }
}

void Button::OnDragRelease() {
    if (InputManager::Released(Keys::Touch)) {
        if (eOnTap) eOnTap(*this);
    }
    MarkForRedraw();
}
