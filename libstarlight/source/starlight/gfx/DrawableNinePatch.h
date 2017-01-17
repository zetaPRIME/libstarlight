#pragma once
#include "starlight/_global.h"

#include <memory>

#include "starlight/gfx/Drawable.h"
#include "starlight/gfx/DrawableImage.h"
#include "starlight/gfx/RenderCore.h"

namespace starlight {
    namespace gfx {
        class DrawableNinePatch : public DrawableImage {
        public:
            Vector2 margin;
            DrawableNinePatch(CTexture* texture) : DrawableImage (texture) { }
            ~DrawableNinePatch() override { }
            
            //void Draw(const Vector2& position, const Vector2& origin = Vector2::zero, OptRef<VRect> sampleRect = nullptr, OptRef<Color> color = nullptr, float rotation = 0, const Vector2& scale = Vector2::one) override;
            void Draw(const VRect& rect, OptRef<VRect> sampleRect = nullptr, OptRef<Color> color = nullptr) override;
        };
    }
}