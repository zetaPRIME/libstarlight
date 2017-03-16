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

Vector2 BitmapFont::MeasureTo(std::string& msg, bool total, unsigned int end, float maxWidth) {
    if (total) {
        Vector2 measure = Vector2::zero;
        
        ForChar(msg, [&, this](auto& s){
            if (s.i > end) return true;
            if (s.iline > 0) return false;
            measure.x = std::max(measure.x, s.lineWidth);
            measure.y += lineHeight;
            return false;
        }, maxWidth);
        
        return measure;
    }
    
    Vector2 measure;
    bool found = false;
    
    ForChar(msg, [&, this, total, end](auto& s){
        measure = Vector2(s.lineAcc + s.cc->advX, lineHeight * s.lineNum);
        if (s.i == end-1) { 
            found = true;
            if (s.i == s.lineEnd) measure = Vector2(0, lineHeight * (s.lineNum + 1)); // linebreak == next line
            return true;
        }
        
        return false;
    }, maxWidth);
    
    if (!found) { // catch newline-at-end
        measure.x = 0;
        measure.y += lineHeight;
    }
    
    return measure;
}

unsigned int BitmapFont::PointToIndex(std::string& msg, Vector2 pt, float maxWidth) {
    //pt -= Vector2(padX, 0*padY);
    if (pt.y < 0) return 0;
    unsigned int tl = std::floor(pt.y / lineHeight);
    
    unsigned int idx;
    
    ForChar(msg, [&, this, pt, tl](auto& s){
        if (s.lineNum < tl) return false; // skip
        if (s.lineNum > tl) { // huh.
            return true;
        }
        if (s.i == s.lineEnd) {
            if (s.i == s.lineStart) { // if blank line...
                idx = s.i;
                return true;
            }
            return false; // skip newlines on non-blank lines
        }
        
        if ((s.iline == 0 || pt.x >= s.lineAcc) && pt.x < s.lineAcc + s.cc->advX) {
            idx = s.i;
            if (pt.x > s.lineAcc + s.cc->advX * 0.5f) idx++;
            return true;
        }
        
        idx = s.i+1;
        return false;
    }, maxWidth);
    
    return idx;
}

void BitmapFont::ForChar(const std::string& msg, std::function<bool(CharLoopState&)> func, float maxWidth) {
    struct LineStat {
        unsigned int start;
        unsigned int end;
        float width;
    };
    
    unsigned int len = msg.length();
    float space = Char(' ').advX;
    
    std::vector<LineStat> lines;
    
    {
        LineStat cl = {0, 0, 0};
        float ww = 0;
        unsigned int ws = 0;
        for (unsigned int i = 0; i <= len; i++) {
            char c = (i == len) ? '\n' : msg[i];
            char pc = (i == 0) ? '\n' : msg[i-1];
            if (c == ' ' || c == '\n') {
                // handle previously-accumulated word
                if (cl.start != ws && cl.width + ww > maxWidth) { // don't bother wrapping the first word on a line...
                    // omit breaking space from the end of the line
                    if (ws > 0 && msg[ws-1] == ' ') cl.width -= space;
                    // wrap
                    lines.push_back(cl);
                    cl = {ws, i, ww}; // start at start of word, end at < this char, have width of word
                    ww = 0;
                    ws = i+1;
                } else {
                    // add to length
                    cl.width += ww;
                    cl.end = i; // < this char
                    ww = 0;
                    ws = i+1;
                }
                
                if (c == ' ') {
                    // space should be present in spacing when it *is not* the line-terminator (that is, the space before a wrapped word)
                    // how the fuck do I do that!?
                    // probably a retroactive decrement on wrap event...
                    if (true) { // if not... this stuff ^
                        cl.width += space;
                    }
                } else { // newline
                    // I guess there's no circumstance where a newline *shouldn't* be honored?
                    cl.end = i;
                    lines.push_back(cl);
                    cl = {i+1, i+1, 0};
                }
            } else { // non-word-ender! add to accumulated word width
                ww += Char(c).advX + GetKerning(pc, c);
            }
        }
        // don't need to manually do anything after due to the extra step
        //lines.push_back(cl);
    }
    
    CharLoopState state;
    state.numLines = lines.size();
    state.lineNum = 0;
    for (auto& cl : lines) {
        state.lineStart = cl.start;
        state.lineEnd = cl.end;
        state.lineWidth = cl.width;
        state.lineAcc = 0;
        state.c = '\n';
        
        for (unsigned int i = cl.start; i <= cl.end; i++) {
            char pc = state.c;
            state.c = (i >= len) ? '\n' : msg[i];
            state.cc = &(Char(state.c));
            
            state.i = i;
            state.iline = i - cl.start;
            
            state.lineAcc += GetKerning(pc, state.c); // I think this goes before?
            
            if (func(state)) return; // return true to break
            
            state.lineAcc += state.cc->advX;
        }
        
        state.lineNum++;
    }
    
}
