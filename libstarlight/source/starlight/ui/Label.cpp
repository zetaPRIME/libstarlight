#include "Label.h"

#include "starlight/GFXManager.h"

using starlight::GFXManager;

using starlight::ui::Label;

Label::Label(VRect rect) {
    this->rect = rect;
    font = ThemeManager::GetFont("default.12");
}

void Label::AutoSize() {
    if (autoSizeV) {
        float h = font->Measure(text, 1, rect.size.x).y;
        Resize(rect.size.x, h);
    }
    
    buffer.reset();
    
    MarkForRedraw();
}

void Label::SetText(const std::string& text) {
    this->text = text;
    AutoSize();
}

void Label::SetFont(const std::string& fontName) {
    font = ThemeManager::GetFont(fontName);
    AutoSize();
}

void Label::PreDrawOffscreen() {
    buffer.reset(); // I guess?
}

void Label::PreDraw() {
    if (!buffer && text.length() > 64) {
        buffer = std::make_unique<gfx::DrawContextCanvas>(rect.size + Vector2(0, 8));
        buffer->Clear();
        GFXManager::PushContext(buffer.get());
        font->Print(buffer->rect, text, 1, color, justification, borderColor);
        GFXManager::PopContext();
    }
}

void Label::Draw() {
    auto rect = (this->rect + GFXManager::GetOffset()).IntSnap();
    if (buffer) {
        buffer->Draw(VRect(rect.pos, buffer->rect.size));
    } else {
        font->Print(rect, text, 1, color, justification, borderColor);
    }
}

