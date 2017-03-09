#include "DrawLayerProxy.h"

#include "starlight/GFXManager.h"

using starlight::GFXManager;
using starlight::gfx::DrawContextCanvas;

using starlight::ui::DrawLayerProxy;

void DrawLayerProxy::Refresh() {
    canvas.reset();
    MarkForRedraw();
}

void DrawLayerProxy::PreDraw() {
    if (useCanvas) {
        if (!canvas) {
            canvas = std::make_unique<DrawContextCanvas>(rect.size);
            canvas->Clear();
            GFXManager::PushContext(canvas.get());
            if (eDraw) eDraw(*this);
            GFXManager::PopContext();
        }
    } else canvas.reset();
}

void DrawLayerProxy::Draw() {
    if (canvas) {
        canvas->Draw(VRect(rect.pos, canvas->rect.size));
    } else {
        if (eDraw) eDraw(*this);
    }
}
