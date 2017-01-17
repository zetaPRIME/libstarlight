#include "starlight/datatypes/Color.h"
#include "starlight/GFXManager.h"

#include "starlight/gfx/RenderCore.h"

#include "DrawableTest.h"

using starlight::Color;
using starlight::GFXManager;
using starlight::gfx::DrawableTest;

using starlight::gfx::RenderCore;

void DrawableTest::Draw(const Vector2& position, const Vector2& origin, OptRef<VRect> sampleRect, OptRef<Color> color, float rotation, const Vector2& scale) {
    if (GFXManager::PrepareForDrawing()) {
        //static u32 col = Color(0, 0.5f, 1);
        //sf2d_draw_rectangle_rotate(position.x, position.y, 16, 16, col, rotation);
    }
}

void DrawableTest::Draw(const VRect& rect, OptRef<VRect> sampleRect, OptRef<Color> color) {
    if (GFXManager::PrepareForDrawing()) {
        RenderCore::BindColor(color ? color.get() : Color(1,1,1,1));
        RenderCore::DrawQuad(rect, VRect());
    }
}

