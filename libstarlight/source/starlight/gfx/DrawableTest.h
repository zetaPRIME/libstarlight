#pragma once
#include "starlight/_global.h"

#include "starlight/gfx/Drawable.h"

namespace starlight {
    namespace gfx {
        class DrawableTest : public Drawable {
        public:
            DrawableTest() { }
            ~DrawableTest() override { }
            
            void Draw(const Vector2& position, const Vector2& origin = Vector2::zero, OptRef<VRect> sampleRect = nullptr, OptRef<Color> color = nullptr, float rotation = 0, const Vector2& scale = Vector2::one) override;
            void Draw(const VRect& rect, OptRef<VRect> sampleRect = nullptr, OptRef<Color> color = nullptr) override;
            
            Vector2 Size() override { return Vector2::zero; }
        };
    }
}

