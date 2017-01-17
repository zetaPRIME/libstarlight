#pragma once
#include "starlight/_global.h"

#include <memory>
#include <unordered_map>
#include <string>

#include "starlight/datatypes/Vector2.h"
#include "starlight/datatypes/VRect.h"
#include "starlight/datatypes/Color.h"

#include "starlight/datatypes/OptRef.h"

#include "starlight/gfx/RenderCore.h"

#include "starlight/gfx/DisplayList.h"

// we don't need the whole thing here
#include "starlight/_incLib/json_fwd.hpp"

namespace starlight {
    namespace gfx {
        class BitmapFont {
        public:
            struct CharInfo {
                float imgX, imgY;
                float width, height;
                float offX, offY;
                float advX;
            };
            
            float fontSize;
            float lineHeight;
            float baseY;
            float padX, padY;
            
            
            std::unordered_map<char, CharInfo> chars;
            std::unordered_map<unsigned int, float> kernings;
            
            std::unique_ptr<CTexture> txMain;
            std::unique_ptr<CTexture> txBorder;
            
            BitmapFont() { }
            BitmapFont(nlohmann::json& j);
            ~BitmapFont() { }
            
            float GetKerning(char cl, char cr);
            CharInfo& Char(char c);
            float DrawText(const Vector2& penStart, std::string& msg, float scale = 1, DisplayList* dl = nullptr);
            // what to put in the bitmapfont class itself?
            
            static inline constexpr unsigned int KerningKey(char cl, char cr) {
                return (static_cast<unsigned int>(cl) | (static_cast<unsigned int>(cr) << 8));
            }
        };
    }
}