#pragma once
#include "starlight/_global.h"

#include <memory>

#include "starlight/gfx/BitmapFont.h"
#include "starlight/gfx/Font.h"

namespace starlight {
    namespace gfx {
        class FontBMF : public Font {
        private:
            void PrintOp(Vector2 position, std::string& text, float scale, const Color& color, Vector2 justification, OptRef<Color> borderColor, float maxWidth, Vector2* measure, DisplayList* dl);
            
        public:
            std::shared_ptr<BitmapFont> font;
            
            FontBMF() { }
            ~FontBMF() { }
            
            Vector2 Measure(std::string& text, float scale = 1, float maxWidth = 400) override;
            void Print(Vector2 position, std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) override;
            void Print(VRect rect, std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) override;
            void PrintDisplayList(DisplayList* dl, Vector2 position, std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) override;
            void PrintDisplayList(DisplayList* dl, VRect rect, std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) override;
        };
    }
}