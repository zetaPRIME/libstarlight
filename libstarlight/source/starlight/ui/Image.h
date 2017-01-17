#pragma once
#include "starlight/_global.h"

#include <string>
#include <memory>

#include "starlight/ThemeManager.h"
#include "starlight/gfx/ThemeRef.h"

#include "starlight/gfx/DrawContextCanvas.h"

#include "starlight/ui/UIElement.h"

namespace starlight {
    namespace ui {
        class Image : public UIElement {
        private:
            //
        public:
            gfx::ThemeRef<gfx::Drawable> image;
            Color tint = Color::white;
            
            Image(VRect rect, const std::string& imgPath);
            Image(Vector2 pos, const std::string& imgPath);
            ~Image() override { }
            
            void SetImage(const std::string& imgPath, bool resize = true);
            
            void Draw() override;
        };
    }
}