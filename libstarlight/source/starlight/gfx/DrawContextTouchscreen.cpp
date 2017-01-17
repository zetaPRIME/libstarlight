#include "starlight/gfx/RenderCore.h"

#include "DrawContextTouchscreen.h"

using starlight::gfx::DrawContextTouchscreen;

void DrawContextTouchscreen::Open() {
    // meh
}

void DrawContextTouchscreen::Close() {
    drawReady = false;
}

bool DrawContextTouchscreen::Prepare() {
    if (drawReady) return true;
    drawReady = true;
    RenderCore::targetBottom->BindTarget();
    return true;
}


