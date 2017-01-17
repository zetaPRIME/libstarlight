#pragma once
#include "starlight/_global.h"

#include <memory>

#include "starlight/gfx/RenderCore.h"

#include "starlight/gfx/Drawable.h"
#include "starlight/gfx/DrawContext.h"

namespace starlight {
    namespace gfx {
        class DrawContextCanvas : public DrawContext, public Drawable {
        private:
            std::unique_ptr<CRenderTarget> target;
            
        protected:
            void Open() override;
            void Close() override;
            bool Prepare() override;
        public:
            DrawContextCanvas(VRect rect);
            DrawContextCanvas(Vector2 size);
            ~DrawContextCanvas();
            
            void Draw(const Vector2& position, const Vector2& origin = Vector2::zero, OptRef<VRect> sampleRect = nullptr, OptRef<Color> color = nullptr, float rotation = 0, const Vector2& scale = Vector2::one) override;
            void Draw(const VRect& rect, OptRef<VRect> sampleRect = nullptr, OptRef<Color> color = nullptr) override;
            
            Vector2 Size() override;
            
            void Clear(Color color) override;
            void Clear() override;
        };
    }
}