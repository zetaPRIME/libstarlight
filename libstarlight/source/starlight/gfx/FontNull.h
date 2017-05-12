#pragma once
#include "starlight/_global.h"

namespace starlight {
    namespace gfx {
        class FontNull : public Font {
        public:
            FontNull() { }
            ~FontNull() override { }
            
            void Print(Vector2 position, const std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) override {}
            void Print(VRect rect, const std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) override {}
            
            Vector2 Measure(const std::string& text, float scale = 1, float maxWidth = 400) override { return Vector2::zero; }
            
            Vector2 GetCursorPosition(VRect rect, const std::string& text, unsigned int end, float scale = 1) override { return Vector2::zero; }
            unsigned int GetCursorFromPoint(VRect rect, const std::string& text, Vector2 pt, float scale = 1) override { return 0; }
        };
        
    }
}
