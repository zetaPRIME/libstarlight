#pragma once
#include "starlight/_global.h"

#include "starlight/datatypes/Vector2.h"
#include "starlight/datatypes/VRect.h"
#include "starlight/datatypes/Color.h"

#include "starlight/datatypes/OptRef.h"

namespace starlight {
    namespace gfx {
        class Drawable {
        public:
            Drawable() { }
            virtual ~Drawable() { }
            
            // pattern after:
            // public abstract void Draw(DrawContext context, PxRect rect, PxRect? sampleRect = null, DrawColor? color = null);
            // public abstract void Draw(DrawContext context, FxVector position, FxVector? align = null, PxRect? sampleRect = null, DrawColor? color = null, float rotation = 0, FxVector? scale = null);
            virtual void Draw(const Vector2& position, const Vector2& origin = Vector2::zero, OptRef<VRect> sampleRect = nullptr, OptRef<Color> color = nullptr, float rotation = 0, const Vector2& scale = Vector2::one) = 0;
            void Draw(const Vector2& position, OptRef<Vector2> hotspot, OptRef<VRect> sampleRect, OptRef<Color> color, float rotation, float scale) {
                Draw(position, hotspot, sampleRect, color, rotation, Vector2(scale, scale));
            }
            
            virtual void Draw(const VRect& rect, OptRef<VRect> sampleRect = nullptr, OptRef<Color> color = nullptr) = 0;
            
            virtual Vector2 Size() = 0;
        };
    }
}


