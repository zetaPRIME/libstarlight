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

Vector2 BitmapFont::MeasureTo(std::string& msg, bool total, unsigned int end, float maxWidth) {
    auto len = msg.length();
    if (end > len) end = len;
    
    Vector2 pen = Vector2::zero;
    Vector2 oPen = pen;
    float longest = 0;
    float wordlen = 0;
    float plen = 0;
    
    float space = Char(' ').advX;
    
    for (unsigned int i = 0; i < len; i++) {
        char& c = msg[i];
        if (c == ' ' || c == '\n') {
            bool alb = false; // already linebreak
            oPen = pen;
            if (pen.x > longest) longest = pen.x;
            if (pen.x + wordlen > maxWidth) {
                pen.x = 0; pen.y += lineHeight;
                alb = true;
            }
            pen.x += wordlen;
            pen.x += space;
            
            if (c == '\n' && !alb) {
                if (pen.x > longest) longest = pen.x;
                pen.x = 0; pen.y += lineHeight;
            }
            
            if (!total && i >= end) {
                //if (c == ' ') pen.x -= space; // I think this needs to be undone too?
                return Vector2(oPen.x + plen, oPen.y); // return cursor position
            }
            wordlen = plen = 0;
        } else {
            float adv = Char(c).advX; 
            wordlen += adv;
            if (i < end) plen += adv;
        }
    }
    
    {   // oh right, this check kind of needs to happen at the end too
        if (pen.x > longest) longest = pen.x;
        if (pen.x + wordlen > maxWidth) {
            pen.x = 0; pen.y += lineHeight;
        }
        pen.x += wordlen;
        
        if (!total) {
            //if (c == ' ') pen.x -= space; // I think this needs to be undone too?
            return Vector2(pen.x - (wordlen - plen), pen.y); // return cursor position
        }
    }
    
    //if (msg.back() != '\n') pen.y += lineHeight; // trim trailing newline (todo: make this recursive...?)
    pen.y += lineHeight;
    return Vector2(longest, pen.y); // total size
}

unsigned int BitmapFont::PointToIndex(std::string& msg, Vector2 pt, float maxWidth) {
    if (pt.y < 0) return 0;
    auto len = msg.length();
    
    unsigned int line = 0;
    unsigned int tLine = std::max(0.0f, std::floor(pt.y / lineHeight));
    
    unsigned int le = 0; // line end
    unsigned int ti = 4294967295; // target index
    
    Vector2 pen = Vector2::zero;
    float wordlen = 0;
    
    float space = Char(' ').advX;
    
    for (unsigned int i = 0; i < len; i++) {
        char& c = msg[i];
        if (c == ' ' || c == '\n') {
            // linebreak on newline or wrap needed
            bool lb = (c == '\n' || pen.x + space + wordlen > maxWidth);
            
            if (lb) {
                if (c == '\n') le = i; // not wrapped
                if (line == tLine) return std::min(le, ti);
                pen.x = (c == ' ') ? wordlen : 0;
                line++;
            } else {
                pen.x += wordlen + space;
                if (line == tLine && ti == 4294967295 && pen.x - space*0.5f >= pt.x) ti = i;
                le = i;
            }
            
            wordlen = 0; // HERP DERP.
        } else {
            float cw = Char(c).advX;
            wordlen += cw;
            if (line == tLine && ti == 4294967295 && pen.x + wordlen - cw*0.5f >= pt.x) ti = i;
        }
    }
    // oh right, process last word
    if (pen.x + space + wordlen > maxWidth) {
        if (line == tLine) return std::min(le, ti);
        pen.x = wordlen;
        line++;
    } else {
        le = len;
    }
    
    if (line == tLine) return std::min(le, ti);
    
    return len;
}





















//
