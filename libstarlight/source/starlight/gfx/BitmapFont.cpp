#include "BitmapFont.h"

#include <cstdlib>
#include <cmath>
#include <fastmath.h>

#include "starlight/_incLib/json.hpp"

using nlohmann::json;

using starlight::Vector2;
using starlight::VRect;
using starlight::Color;
using starlight::gfx::BitmapFont;
//using starlight::gfx::BitmapFont::CharInfo;

BitmapFont::BitmapFont(json& j) {
    // populate global info
    fontSize = fabsf(j["info"]["size"]);
    lineHeight = j["common"]["lineHeight"];
    baseY = j["common"]["base"];
    padX = j["info"]["padding"][3];
    padY = j["info"]["padding"][0];
    
    // assemble charinfo structs
    for (auto& c : json::iterator_wrapper(j["chars"])) {
        auto& cc = c.value();
        auto& def = chars.insert({static_cast<char>(std::stoi(c.key())), CharInfo()}).first->second; // well that's kind of a pain just to get back the instantiated value
        
        def.imgX = cc["x"];
        def.imgY = cc["y"];
        def.width = cc["width"];
        def.height = cc["height"];
        def.offX = cc["xoffset"];
        def.offY = cc["yoffset"];
        def.advX = cc["xadvance"];
    }
    
    // fetch kerning info
    for (auto& kl : json::iterator_wrapper(j["kernings"])) {
        char cl = std::stoi(kl.key());
        for (auto& kr : json::iterator_wrapper(kl.value())) {
            char cr = std::stoi(kr.key());
            kernings[KerningKey(cl, cr)] = kr.value();
        }
    }
}

float BitmapFont::GetKerning(char cl, char cr) {
    auto f = kernings.find(KerningKey(cl, cr));
    if (f == kernings.end()) return 0;
    return f->second;
}

BitmapFont::CharInfo& BitmapFont::Char(char c) {
    auto f = chars.find(c);
    if (f != chars.end()) return f->second;
    static CharInfo cdefault = []() -> CharInfo {
        CharInfo def;
        def.imgX = def.imgY = def.width = def.height = def.offX = def.offY = def.advX = 0.0f;
        return def;
    }();
    return cdefault;
}

float BitmapFont::DrawText(const Vector2& penStart, std::string& msg, float scale, DisplayList* dl) {
    Vector2 pen = penStart;
    Vector2 pad(padX, 0);//-padY); // compensate for padding around glyphs
    pen -= pad;
    Vector2 uvScale = Vector2::one / txMain->txSize;
    
    auto qn = dl ? dl->GetLastNode<DLNode_Quads>(true) : nullptr;
    
    char cl = 0xFF;//' ';
    for (auto c : msg) {
        pen.x += GetKerning(cl, c) * scale;
        auto& ci = Char(c);
        //printf("%c w %f h %f adv %f sc %f\n", c, ci.width, ci.height, ci.advX, scale);
        VRect crect(ci.imgX, ci.imgY, ci.width, ci.height);
        if (dl) qn->Add(VRect(pen, crect.size * scale), crect * uvScale);
        pen.x += ci.advX * scale;
        
        cl = c;
    }
    pen += pad;
    return pen.x - penStart.x;
}





























