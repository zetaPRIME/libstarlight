#include <cmath>
#include <citro3d.h>

#include "starlight/datatypes/Color.h"
#include "starlight/GFXManager.h"

#include "DrawContextCanvas.h"

using starlight::Vector2;
using starlight::VRect;
using starlight::Color;
using starlight::GFXManager;
using starlight::gfx::DrawContextCanvas;

using starlight::gfx::RenderCore;
using starlight::gfx::CRenderTarget;

DrawContextCanvas::DrawContextCanvas(VRect rect) : DrawContext(rect) {
    target = std::make_unique<CRenderTarget>(rect.size.x, rect.size.y);
}

DrawContextCanvas::DrawContextCanvas(Vector2 size) : DrawContextCanvas(VRect(Vector2::zero, size)) { }

DrawContextCanvas::~DrawContextCanvas() {
    // hmm. don't think we need to do anything here anymore
}

// drawcontext stuff
void DrawContextCanvas::Open() { }

void DrawContextCanvas::Close() {
    drawReady = false;
}

bool DrawContextCanvas::Prepare() {
    if (drawReady) return true;
    drawReady = true;
    target->BindTarget();
    return true;
}

void DrawContextCanvas::Clear(Color color) {
    target->Clear(color);
}
void DrawContextCanvas::Clear() { Clear(Color(0,0,0,0)); }

// drawable stuff
void DrawContextCanvas::Draw(const Vector2& position, const Vector2& origin, OptRef<VRect> sampleRect, OptRef<Color> color, float rotation, const Vector2& scale) {
    if (GFXManager::PrepareForDrawing()) {
        target->Bind(color ? color.get() : Color(1,1,1,1));
        const VRect& sr = sampleRect ? sampleRect.get() : this->rect;
        VRect rect(position - origin * scale, sr.size * scale);
        RenderCore::DrawQuad(rect, position, rotation, sr / target->txSize);
    }
}

void DrawContextCanvas::Draw(const VRect& rect, OptRef<VRect> sampleRect, OptRef<Color> color) {
    if (GFXManager::PrepareForDrawing()) {
        target->Bind(color ? color.get() : Color(1,1,1,1));
        const VRect& sr = sampleRect ? sampleRect.get() : this->rect;
        RenderCore::DrawQuad(rect, sr / target->txSize);
    }
}

Vector2 DrawContextCanvas::Size() {
    return target->size;
}
