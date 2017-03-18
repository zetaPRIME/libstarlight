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

using starlight::TextConfig;

using starlight::ui::Button;

std::function<TextConfig&()> Button::defCfg = []() -> TextConfig& {
    static TextConfig _tc = ThemeManager::GetMetric("/controls/button/text", TextConfig());
    return _tc;
};

void Button::SetText(const std::string& text) {
    label = text;
    MarkForRedraw();
}

void Button::Draw() {
    //static auto font = ThemeManager::GetFont("default.12");
    
    static auto idle = ThemeManager::GetAsset("controls/button.idle");
    static auto press = ThemeManager::GetAsset("controls/button.press");
    
    TextConfig& tc = style.textConfig.ROGet();
    
    auto rect = (this->rect + GFXManager::GetOffset()).IntSnap();
    
    if (InputManager::GetDragHandle() == this) {
        (style.press ? style.press : press)->Draw(rect);
    } else {
        (style.idle ? style.idle : idle)->Draw(rect);
    }
    
    tc.Print(rect, label);
    if (style.glyph) style.glyph->Draw(rect.Center(), Vector2::half, nullptr, tc.textColor);
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
