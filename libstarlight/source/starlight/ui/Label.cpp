#include "Label.h"

#include "starlight/GFXManager.h"

using starlight::GFXManager;

using starlight::ui::Label;

Label::Label(VRect rect) {
    this->rect = rect;
    textConfig = ThemeManager::GetMetric<TextConfig>("/textPresets/normal.12", TextConfig());
}

void Label::AutoSize() {
    if (autoSizeV) {
        float h = textConfig.font->Measure(text, 1, rect.size.x).y;
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
    textConfig.font = ThemeManager::GetFont(fontName);
    AutoSize();
}

void Label::SetPreset(const std::string& name) {
    textConfig = ThemeManager::GetMetric<TextConfig>("/textPresets/" + name, textConfig);
    AutoSize();
}

void Label::Refresh() { AutoSize(); }

void Label::PreDrawOffscreen() {
    buffer.reset(); // I guess?
}

void Label::PreDraw() {
    if (!buffer && text.length() > 64) {
        buffer = std::make_unique<gfx::DrawContextCanvas>(rect.size + Vector2(0, 8));
        buffer->Clear();
        GFXManager::PushContext(buffer.get());
        textConfig.Print(buffer->rect, text, justification);
        GFXManager::PopContext();
    }
}

void Label::Draw() {
    auto rect = (this->rect + GFXManager::GetOffset()).IntSnap();
    if (buffer) {
        buffer->Draw(VRect(rect.pos, buffer->rect.size));
    } else {
        textConfig.Print(rect, text, justification);
    }
}
