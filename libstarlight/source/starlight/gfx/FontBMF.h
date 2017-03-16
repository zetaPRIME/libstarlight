#pragma once
#include "starlight/_global.h"

#include <memory>

#include "starlight/gfx/BitmapFont.h"
#include "starlight/gfx/Font.h"

namespace starlight {
    namespace gfx {
        class FontBMF : public Font {
        private:
            //
            
        public:
            std::shared_ptr<BitmapFont> font;
            
            FontBMF() { }
            ~FontBMF() { }
            
            Vector2 Measure(std::string& text, float scale = 1, float maxWidth = 400) override;
            void Print(Vector2 position, std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) override;
            void Print(VRect rect, std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) override;
            //void PrintDisplayList(DisplayList* dl, Vector2 position, std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) override;
            //void PrintDisplayList(DisplayList* dl, VRect rect, std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) override;
            
            Vector2 GetCursorPosition(VRect rect, std::string& text, unsigned int end, float scale) override;
            unsigned int GetCursorFromPoint(VRect rect, std::string& text, Vector2 pt, float scale) override;
        };
    }
}
