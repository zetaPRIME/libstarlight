#include <cstdio>
#include <cmath>

#include "starlight/datatypes/Color.h"
#include "starlight/GFXManager.h"

#include "DrawableNinePatch.h"

using starlight::Vector2;
using starlight::VRect;
using starlight::Color;
using starlight::GFXManager;
using starlight::gfx::DrawableNinePatch;

using starlight::gfx::RenderCore;
using starlight::gfx::CRenderTarget;

void DrawableNinePatch::Draw(const VRect& rect, OptRef<VRect> sampleRect, OptRef<Color> color) {
    if (GFXManager::PrepareForDrawing()) {
        texture->Bind(color ? color.get() : Color(1,1,1,1));
        VRect rr = rect.IntSnap();
        const VRect& sr = (sampleRect ? sampleRect.get() : VRect(Vector2::zero, texture->size)).IntSnap();
        
        // here comes the fun, doo doot doo doo~
        Vector2 m = Vector2::one / texture->txSize;
        // top left
        RenderCore::DrawQuad(rr.TopEdge(margin.y).LeftEdge(margin.x), sr.TopEdge(margin.y).LeftEdge(margin.x) * m);
        // top
        RenderCore::DrawQuad(rr.TopEdge(margin.y).Expand(-margin.x, 0), sr.TopEdge(margin.y).Expand(-margin.x, 0) * m);
        // top right
        RenderCore::DrawQuad(rr.TopEdge(margin.y).RightEdge(margin.x), sr.TopEdge(margin.y).RightEdge(margin.x) * m);
        // left
        RenderCore::DrawQuad(rr.Expand(0, -margin.y).LeftEdge(margin.x), sr.Expand(0, -margin.y).LeftEdge(margin.x) * m);
        // center
        RenderCore::DrawQuad(rr.Expand(-margin.x, -margin.y), sr.Expand(-margin.x, -margin.y) * m);
        // right
        RenderCore::DrawQuad(rr.Expand(0, -margin.y).RightEdge(margin.x), sr.Expand(0, -margin.y).RightEdge(margin.x) * m);
        // bottom left
        RenderCore::DrawQuad(rr.BottomEdge(margin.y).LeftEdge(margin.x), sr.BottomEdge(margin.y).LeftEdge(margin.x) * m);
        // bottom
        RenderCore::DrawQuad(rr.BottomEdge(margin.y).Expand(-margin.x, 0), sr.BottomEdge(margin.y).Expand(-margin.x, 0) * m);
        // bottom right
        RenderCore::DrawQuad(rr.BottomEdge(margin.y).RightEdge(margin.x), sr.BottomEdge(margin.y).RightEdge(margin.x) * m);
        
        // probably could be done with a bit less math, but preoptimization etc. etc.
    }
}//*/
