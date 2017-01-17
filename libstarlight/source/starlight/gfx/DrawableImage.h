#pragma once
#include "starlight/_global.h"

#include <memory>

#include "starlight/gfx/Drawable.h"
#include "starlight/gfx/RenderCore.h"

namespace starlight {
    namespace gfx {
        class DrawableImage : public Drawable {
        public:
            std::unique_ptr<CTexture> texture;
            
            DrawableImage(CTexture* texture) : texture(texture) { }
            ~DrawableImage() override { }
            
            void Draw(const Vector2& position, const Vector2& origin = Vector2::zero, OptRef<VRect> sampleRect = nullptr, OptRef<Color> color = nullptr, float rotation = 0, const Vector2& scale = Vector2::one) override;
            void Draw(const VRect& rect, OptRef<VRect> sampleRect = nullptr, OptRef<Color> color = nullptr) override;
            
            Vector2 Size() override;
        };
    }
}

