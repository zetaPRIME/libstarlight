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

//#define err(nth, wat) *((unsigned int*)0x00100000+(nth))=wat;
//#define ded(wat) err(0,wat)
Vector2 FontBMF::Measure(std::string& text, float scale, float maxWidth) {
    if (text == "") return Vector2::zero;
    return font->MeasureTo(text, true, text.length(), maxWidth) * scale;
}

void FontBMF::Print(Vector2 position, std::string& text, float scale, Color color, Vector2 justification, OptRef<Color> borderColor) {
    if (text == "") return;
    if (GFXManager::PrepareForDrawing()) {
        DisplayList dl = DisplayList();
        {
            auto qn = dl.GetLastNode<DLNode_Quads>(true);
            Vector2 uvScale = Vector2::one / font->txMain->txSize;
            Vector2 ppen = Vector2(-font->padX, -font->padY /*- (font->lineHeight - font->baseY)*/);
            font->ForChar(text, [&, this, ppen, justification, qn, scale, uvScale](auto& s){
                if (s.c == ' ' || s.c == '\n') return false; // skip spaces/newlines
                Vector2 pen = (ppen + Vector2(s.lineAcc - s.lineWidth * justification.x, font->lineHeight * ((float)s.lineNum - (float)s.numLines * justification.y))) * scale;
                auto& ci = *s.cc;
                VRect crect(ci.imgX, ci.imgY, ci.width, ci.height);
                qn->Add(VRect(pen, crect.size * scale), crect * uvScale);
                
                return false;
            });
        }
        if (borderColor && borderColor.get() != Color::transparent && font->txBorder) {
            font->txBorder->Bind(borderColor.get());
            dl.Run(position);
        }
        font->txMain->Bind(color);
        dl.Run(position);
    }
}

void FontBMF::Print(VRect rect, std::string& text, float scale, Color color, Vector2 justification, OptRef<Color> borderColor) {
    if (text == "") return;
    if (GFXManager::PrepareForDrawing()) {
        if (borderColor && borderColor.get() != Color::transparent) rect = rect.Expand(-1, -1);
        Vector2 position = rect.pos + rect.size * justification;
        DisplayList dl = DisplayList();
        {
            auto qn = dl.GetLastNode<DLNode_Quads>(true);
            Vector2 uvScale = Vector2::one / font->txMain->txSize;
            Vector2 ppen = Vector2(-font->padX, -font->padY /*- (font->lineHeight - font->baseY)*/);
            font->ForChar(text, [&, this, ppen, justification, qn, scale, uvScale](auto& s){
                if (s.c == ' ' || s.c == '\n') return false; // skip spaces/newlines
                Vector2 pen = (ppen + Vector2(s.lineAcc - s.lineWidth * justification.x, font->lineHeight * ((float)s.lineNum - (float)s.numLines * justification.y))) * scale;
                auto& ci = *s.cc;
                VRect crect(ci.imgX, ci.imgY, ci.width, ci.height);
                qn->Add(VRect(pen, crect.size * scale), crect * uvScale);
                
                return false;
            }, rect.size.x);
        }
        
        if (borderColor && borderColor.get() != Color::transparent && font->txBorder) {
            font->txBorder->Bind(borderColor.get());
            dl.Run(position);
        }
        font->txMain->Bind(color);
        dl.Run(position);
    }
}

Vector2 FontBMF::GetCursorPosition(VRect rect, std::string& text, unsigned int end, float scale) {
    return rect.pos + (font->MeasureTo(text, false, end, rect.size.x / scale)* scale);
}

unsigned int FontBMF::GetCursorFromPoint(VRect rect, std::string& text, Vector2 pt, float scale) {
    return font->PointToIndex(text, pt*scale - rect.pos, rect.size.x / scale);
}
