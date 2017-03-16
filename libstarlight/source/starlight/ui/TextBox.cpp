#include "TextBox.h"

#include <cmath>

#include "starlight/_incLib/json.hpp"

#include "starlight/InputManager.h"
#include "starlight/GFXManager.h"
#include "starlight/ThemeManager.h"

#include "starlight/dialog/OSK.h"

using starlight::Vector2;
using starlight::Color;

using starlight::gfx::Font;

using starlight::InputManager;
using starlight::GFXManager;
using starlight::ThemeManager;

using starlight::TextConfig;

using starlight::dialog::OSK;
using starlight::dialog::osk::InputHandlerBuffered;

using starlight::ui::TextBox;

void TextBox::SetText(const std::string& text) {
    this->text = text;
    textView.reset();
    MarkForRedraw();
}

void TextBox::PreDraw() {
    if (!textView) {
        static Vector2 margin = ThemeManager::GetMetric("/controls/textBox/margin", Vector2::zero);
        
        textView = std::make_unique<gfx::DrawContextCanvas>(rect.size - margin*2);
        textView->Clear();
        GFXManager::PushContext(textView.get());
        
        static TextConfig tc = ThemeManager::GetMetric("/controls/textBox/text", TextConfig());
        if (multiLine) {
            // for now I guess just flat top-left
            tc.Print(textView->rect, text, Vector2::zero);
        } else {
            Vector2 sz = tc.Measure(text);
            Vector2 justification = Vector2(tc.justification.x, 0.5f);
            
            tc.Print(textView->rect.RightEdge(std::max(textView->rect.size.x, sz.x)), text, justification);
        }
        
        GFXManager::PopContext();
    }
}
void TextBox::PreDrawOffscreen() { textView.reset(); } // discard on offscreen

void TextBox::Draw() {
    static auto bg = ThemeManager::GetAsset("controls/textBox");
    static Vector2 margin = ThemeManager::GetMetric("/controls/textBox/margin", Vector2::zero);
    auto rect = (this->rect + GFXManager::GetOffset()).IntSnap();
    
    bg->Draw(rect);
    
    //tc.Print(rect.Expand(-margin), text);
    if (textView) textView->Draw(rect.Expand(-margin));
}

void TextBox::OnResize() {
    textView.reset();
    MarkForRedraw();
}

void TextBox::OnTouchOn() {
    if (InputManager::Pressed(Keys::Touch)) {
        InputManager::GetDragHandle().Grab(this);
        MarkForRedraw();
    }
}

void TextBox::OnTouchOff() {
    auto& drag = InputManager::GetDragHandle();
    if (drag == this) drag.Release();
}

void TextBox::OnDragStart() {
    // do we need to do anything here?
}

void TextBox::OnDragHold() {
    if (InputManager::TouchDragDist().Length() > InputManager::dragThreshold) {
        InputManager::GetDragHandle().PassUp();
    }
}

void TextBox::OnDragRelease() {
    if (InputManager::Released(Keys::Touch)) {
        // pop up osk
        OSK::New(new InputHandlerBuffered(text, multiLine, [this](auto& str){ this->SetText(str); }))->Open();
    }
    MarkForRedraw();
}
