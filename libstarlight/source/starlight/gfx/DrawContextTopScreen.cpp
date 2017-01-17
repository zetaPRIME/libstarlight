#include "DrawContextTopScreen.h"

#include "starlight/gfx/RenderCore.h"
#include "starlight/GFXManager.h"

using starlight::GFXManager;
using starlight::gfx::DrawContextTopScreen;

void DrawContextTopScreen::Open() {
    // meh
}

void DrawContextTopScreen::Close() {
    drawReady = false;
}

bool DrawContextTopScreen::Prepare() {
    if (drawReady) return true;
    drawReady = true;
    if (GFXManager::parallax > 0) // I think this is right?
        RenderCore::targetTopRight->BindTarget();
    else
        RenderCore::targetTopLeft->BindTarget();
    return true;
}


