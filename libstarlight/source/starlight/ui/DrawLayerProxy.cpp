#include "DrawLayerProxy.h"

#include "starlight/GFXManager.h"
#include "starlight/InputManager.h"

using starlight::GFXManager;
using starlight::gfx::DrawContextCanvas;

using starlight::InputManager;

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

void DrawLayerProxy::OnTouchOn() {
    if (!eOnTap) return;
    if (InputManager::Pressed(Keys::Touch)) {
        InputManager::GetDragHandle().Grab(this);
        MarkForRedraw();
    }
}

void DrawLayerProxy::OnTouchOff() {
    auto& drag = InputManager::GetDragHandle();
    if (drag == this) drag.Release();
}

void DrawLayerProxy::OnDragHold() {
    if (InputManager::TouchDragDist().Length() > InputManager::dragThreshold) {
        InputManager::GetDragHandle().PassUp();
    }
}

void DrawLayerProxy::OnDragRelease() {
    if (InputManager::Released(Keys::Touch)) {
        if (eOnTap) eOnTap(*this);
    }
    MarkForRedraw();
}
