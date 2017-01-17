#include <string>
#include <sstream>
#include <vector>
#include <list>
#include <cmath>

#include "starlight/GFXManager.h"

#include "FontBMF.h"

using starlight::Vector2;
using starlight::VRect;
using starlight::GFXManager;
using starlight::gfx::Font;
using starlight::gfx::FontBMF;

#define err(nth, wat) *((unsigned int*)0x00100000+(nth))=wat;
#define ded(wat) err(0,wat)
Vector2 FontBMF::Measure(std::string& text, float scale, float maxWidth) {
    Vector2 v;
    PrintOp(Vector2(), text, scale, Color(), Vector2(), nullptr, maxWidth, &v, static_cast<DisplayList*>(nullptr));
    return v;
}

void FontBMF::Print(Vector2 position, std::string& text, float scale, Color color, Vector2 justification, OptRef<Color> borderColor) {
    if (GFXManager::PrepareForDrawing()) {
        DisplayList dl = DisplayList();
        PrintOp(position, text, scale, color, justification, borderColor, 2147483647, static_cast<Vector2*>(nullptr), &dl);
        if (borderColor && borderColor.get() != Color::transparent && font->txBorder) {
            font->txBorder->Bind(borderColor.get());
            dl.Run(position);
        }
        font->txMain->Bind(color);
        dl.Run(position);
    }
}

void FontBMF::Print(VRect rect, std::string& text, float scale, Color color, Vector2 justification, OptRef<Color> borderColor) {
    if (GFXManager::PrepareForDrawing()) {
        if (borderColor && borderColor.get() != Color::transparent) rect = rect.Expand(-1, -1);
        Vector2 pos = rect.pos + rect.size * justification;
        DisplayList dl = DisplayList();
        PrintOp(pos, text, scale, color, justification, borderColor, rect.size.x, static_cast<Vector2*>(nullptr), &dl);
        if (borderColor && borderColor.get() != Color::transparent && font->txBorder) {
            font->txBorder->Bind(borderColor.get());
            dl.Run(pos);
        }
        font->txMain->Bind(color);
        dl.Run(pos);
    }
}

void FontBMF::PrintDisplayList(DisplayList* dl, Vector2 position, std::string& text, float scale, Color color, Vector2 justification, OptRef<Color> borderColor) {
    
}

void FontBMF::PrintDisplayList(DisplayList* dl, VRect rect, std::string& text, float scale, Color color, Vector2 justification, OptRef<Color> borderColor) {
    // this bind is super kludge ;.;
    bool drawBorder = (borderColor && borderColor.get() != Color::transparent);
    if (drawBorder) rect = rect.Expand(-1, -1);
    Vector2 pos = rect.pos + rect.size * justification;
    dl->offset = pos - rect.pos;
    if (drawBorder) dl->AddFunc([fnt = std::weak_ptr<BitmapFont>(font), c = borderColor.get()](Vector2 offset) mutable {
        if (auto f = fnt.lock()) f->txBorder->Bind(c);
    });
    else dl->AddFunc([fnt = std::weak_ptr<BitmapFont>(font), c = color](Vector2 offset) mutable {
        if (auto f = fnt.lock()) f->txMain->Bind(c);
    });
    PrintOp(pos, text, scale, color, justification, borderColor, rect.size.x, static_cast<Vector2*>(nullptr), dl);
    if (drawBorder) {
        auto vlist = dl->GetLastNode<DLNode_Quads>();
        dl->AddFunc([fnt = std::weak_ptr<BitmapFont>(font), c = color](Vector2 offset) mutable {
            if (auto f = fnt.lock()) f->txMain->Bind(c);
        });
        dl->AddNode(vlist);
    }
}

void FontBMF::PrintOp(Vector2 position, std::string& text, float scale, const Color& color, Vector2 justification, OptRef<Color> borderColor, float maxWidth, Vector2* measure, DisplayList* dl) {
    float lineHeight = font->lineHeight * scale;
    float spaceWidth = font->Char(' ').advX * scale;
    
    float longest = 0;
    std::vector<std::list<std::string>> ptree;
    std::vector<float> lwidth;
    {
        std::stringstream ss(text);
        std::string line, word;
        
        // prepare line buffer
        ptree.push_back(std::list<std::string>());
        auto* cline = &ptree.back();
        float cLineLen = -spaceWidth;
        
        // split into words and lines
        while(getline(ss, line, '\n')) {
            std::stringstream ls(line);
            while (getline(ls, word, ' ')) {
                float ww = spaceWidth + font->DrawText(Vector2(), word, scale);
                if (cLineLen + ww > maxWidth) {
                    lwidth.push_back(cLineLen);
                    ptree.push_back(std::list<std::string>());
                    cline = &ptree.back();
                    if (cLineLen > longest) longest = cLineLen;
                    cLineLen = -spaceWidth;
                }
                cLineLen += ww;
                cline->push_back(word);
                //
            }
            lwidth.push_back(cLineLen);
            ptree.push_back(std::list<std::string>());
            cline = &ptree.back();
            if (cLineLen > longest) longest = cLineLen;
            cLineLen = -spaceWidth;
        }
    }
    
    if (measure) { // measurement operation
        *measure = Vector2(longest, lwidth.size() * lineHeight);
    }
    if (!dl) return;
    
    //Vector2 lp = position - Vector2(0, lwidth.size() * lineHeight) * justification;
    Vector2 lp = -Vector2(0, lwidth.size() * lineHeight) * justification;
    lp.y -= lineHeight - font->baseY * scale;
    lp.x = floor(lp.x); lp.y = floor(lp.y); // normalize across zero-crossings
    int li = 0;
    
    for (auto& line : ptree) {
        Vector2 wp = lp - Vector2(lwidth[li], 0) * justification;
        for (auto& word : line) {
            wp.x += spaceWidth + font->DrawText(wp, word, scale, dl);
        }
        lp += Vector2(0, lineHeight);
        li++;
    }
}
