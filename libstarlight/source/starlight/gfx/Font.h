#pragma once
#include "starlight/_global.h"

#include <string>

#include "starlight/datatypes/Vector2.h"
#include "starlight/datatypes/VRect.h"
#include "starlight/datatypes/Color.h"
#include "starlight/datatypes/OptRef.h"

#include "starlight/gfx/DisplayList.h"

namespace starlight {
    namespace gfx {
        class Font {
        public:
            static constexpr const int defaultSize = 16;
            
            Font() { }
            virtual ~Font() { }
            
            virtual Vector2 Measure(std::string& text, float scale = 1, float maxWidth = 400) = 0;
            virtual void Print(Vector2 position, std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) = 0;
            virtual void Print(VRect rect, std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) = 0;
            virtual void PrintDisplayList(DisplayList* dl, Vector2 position, std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) = 0;
            virtual void PrintDisplayList(DisplayList* dl, VRect rect, std::string& text, float scale = 1, Color color = Color::white, Vector2 justification = Vector2::zero, OptRef<Color> borderColor = nullptr) = 0;
        };
        
    }
}

