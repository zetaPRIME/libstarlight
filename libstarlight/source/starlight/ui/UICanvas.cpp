#include "starlight/GFXManager.h"

#include "starlight/gfx/DrawContextCanvas.h"

#include "UICanvas.h"

using starlight::gfx::Drawable;
using starlight::gfx::DrawContext;
using starlight::gfx::DrawContextCanvas;

using starlight::ui::UIElement;
using starlight::ui::UIContainer;
using starlight::ui::UICanvas;

UICanvas::UICanvas(VRect rect) {
    this->rect = rect;
}

void UICanvas::MarkForRedraw() {
    needsRedraw = true;
    this->UIElement::MarkForRedraw();
}

void UICanvas::PreDrawOffscreen() {
    if (drawContext) {
        drawContext.reset();
        for (auto& it : children) { it->PreDrawOffscreen(); }
    }
}

void UICanvas::PreDraw() {
    if (!drawContext) drawContext = std::make_unique<starlight::gfx::DrawContextCanvas>(rect.size);
    else if (!needsRedraw) return;
    
    drawContext->Clear();
    GFXManager::PushContext(drawContext.get());
    GFXManager::PushOffsetAdd(-scrollOffset);
    
    VRect vr = ViewportRect();
    
    // both passes here so as not to mix things; one rendertarget at a time, please
    for (auto& it : children) { if (it->rect.Overlaps(vr)) it->PreDraw(); else it->PreDrawOffscreen(); }
    for (auto& it : children) { if (it->rect.Overlaps(vr)) it->Draw(); }
    
    GFXManager::PopOffset();
    GFXManager::PopContext();
    
    needsRedraw = false;
}

void UICanvas::Draw() {
    static_cast<DrawContextCanvas*>(drawContext.get())->Draw(rect + GFXManager::GetOffset());
}

