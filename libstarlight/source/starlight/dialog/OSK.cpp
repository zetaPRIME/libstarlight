#include "OSK.h"

#include "starlight/ThemeManager.h"
#include "starlight/InputManager.h"
#include "starlight/GFXManager.h"

#include "starlight/gfx/Font.h"

#include "starlight/ui/Image.h"
#include "starlight/ui/Button.h"
#include "starlight/ui/Label.h"
#include "starlight/ui/ScrollField.h"

#include "starlight/ConfigManager.h"

using std::string;

using starlight::ThemeManager;
using starlight::InputManager;
using starlight::GFXManager;

using starlight::gfx::Font;

using starlight::ui::Image;
using starlight::ui::Button;
using starlight::ui::Label;
using starlight::ui::ScrollField;
using starlight::ui::DrawLayerProxy;

using starlight::ui::Form;

using starlight::dialog::OSK;

namespace {
    inline starlight::TextConfig& PreviewTC() {
        static auto tc = ThemeManager::GetMetric<starlight::TextConfig>("/dialogs/OSK/preview");
        return tc;
    }
    
    inline bool ShiftScroll(Keys k) {
        return InputManager::Pressed(k) || (InputManager::Held(Keys::L | Keys::R) && InputManager::Held(k));
    }
    
    const constexpr float textHang = 4;
}

OSK::OSK(osk::InputHandler* handler) : Form(true), handler(handler) {
    priority = 1000; // probably don't want all that much displaying above the keyboard
    handler->parent = this;
    
    auto cover = touchScreen->AddNew<Image>(touchScreen->rect, "decorations/osk.background");
    cover->blockTouch = true;
    
    // build keyboard
    
    //setContainer = touchScreen->AddNew<ui::UIContainer>(VRect::touchScreen); // kept as a test case
    setContainer = touchScreen->AddNew<ui::UICanvas>(VRect::touchScreen); // but this is much more efficient
    
    auto actSym = [this](Button& key){
        this->handler->InputSymbol(key.label);
        this->OnKey();
    };
    
    Vector2 bs(24, 32);
    Vector2 bpen;
    Vector2 bpstart(160-bs.x*(12.5/2), 68+5);
    int line = -1;
    float linestart [] = {0, .5, .75, 1.25, 2.75-1};
    string chr = "\n1234567890-=\nqwertyuiop[]\nasdfghjkl;\'\nzxcvbnm,./\n` \\";
    string Chr = "\n!@#$%^&*()_+\nQWERTYUIOP{}\nASDFGHJKL:\"\nZXCVBNM<>?\n~ |";
    
    for (unsigned int ic = 0; ic < chr.length(); ic++) {
        char& c = chr[ic];
        char& C = Chr[ic];
        if (c == '\n') {
            line++;
            bpen = bpstart + Vector2(linestart[line] * bs.x, bs.y * line);
        } else {
            // lower
            auto key = setContainer->AddNew<Button>(VRect(bpen, bs));
            if (c == ' ') key->rect.size.x *= 6;
            key->SetText(string(1, c));
            key->eOnTap = actSym;
            
            // upper
            key = setContainer->AddNew<Button>(VRect(bpen + Vector2(0, 1000), bs));
            if (C == ' ') key->rect.size.x *= 6;
            key->SetText(string(1, C));
            key->eOnTap = actSym;
            
            // and after
            bpen.x += key->rect.size.x;
        }
    }
    
    // backspace
    bpen = bpstart + bs * Vector2(linestart[3] + 10, 3);
    auto key = touchScreen->AddNew<Button>(VRect(bpen, bs));
    key->rect.size.x *= 1.25;
    key->style.glyph = ThemeManager::GetAsset("glyphs/backspace.small");
    key->eOnTap = [this](auto& btn){ this->handler->Backspace(); this->OnKey(); };
    
    // enter
    bpen = bpstart + bs * Vector2(linestart[4] + 8, 4);
    key = touchScreen->AddNew<Button>(VRect(bpen, bs));
    key->rect.size.x *= 2.5;
    key->style.glyph = ThemeManager::GetAsset("glyphs/enter.large");
    key->eOnTap = [this](auto& btn){ this->handler->Enter(); this->OnKey(); };
    
    // shift
    bpen = bpstart + bs * Vector2(linestart[0] + .25, 4);
    key = touchScreen->AddNew<Button>(VRect(bpen, bs));
    key->rect.size.x = bs.x * (linestart[4] - linestart[0] - .25);
    key->style.glyph = ThemeManager::GetAsset("glyphs/shift.large");
    key->eOnTap = [this](auto& btn){ this->shiftLock ^= true; };
    shiftKey = key;
    
    previewSc = touchScreen->AddNew<ScrollField>(VRect(VRect::touchScreen.TopEdge(66)));
    
    preview = previewSc->AddNew<DrawLayerProxy>(VRect::touchScreen.TopEdge(66).Expand(-2, 0), [this](auto& layer){ this->DrawPreview(layer); }, true);
    preview->eOnTap = [this](auto& layer){ this->OnPreviewTap(layer); };
    
    RefreshPreview();
}

void OSK::Update(bool focused) {
    if (handler->done) {
        Close();
        return;
    }
    if (focused) {
        if (InputManager::Pressed(Keys::B | Keys::Start)) handler->Done();
        if (handler->showPreview) {
            auto& tc = PreviewTC();
            bool refresh = false;
            
            if (ShiftScroll(Keys::DPadLeft)) {
                auto c = handler->GetCursor();
                if (c > 0) handler->SetCursor(c - 1);
                refresh = true;
            }
            if (ShiftScroll(Keys::DPadRight)) {
                handler->SetCursor(handler->GetCursor() + 1);
                refresh = true;
            }
            if (ShiftScroll(Keys::DPadUp)) {
                Vector2 pt = tc.GetCursorPosition(preview->rect, handler->GetPreviewText(), handler->GetCursor());
                pt.y -= tc.Measure("|").y * 0.5f;
                handler->SetCursor(tc.GetCursorFromPoint(preview->rect, handler->GetPreviewText(), pt));
                refresh = true;
            }
            if (ShiftScroll(Keys::DPadDown)) {
                Vector2 pt = tc.GetCursorPosition(preview->rect, handler->GetPreviewText(), handler->GetCursor());
                pt.y += tc.Measure("|").y * 1.5f;
                handler->SetCursor(tc.GetCursorFromPoint(preview->rect, handler->GetPreviewText(), pt));
                refresh = true;
            }
            if (refresh) RefreshPreview();
        }
        
        float& s = setContainer->scrollOffset.y;
        float ts = 0;
        if (InputManager::Held(Keys::L) || InputManager::Held(Keys::R)) {
            ts = 1000;
            shiftLock = false;
        } else if (shiftLock) ts = 1000;
        if (s != ts) {
            s = ts;
            setContainer->MarkForRedraw();
            
            if (ts > 0) {
                static TextConfig stc = ThemeManager::GetMetric<starlight::TextConfig>("/dialogs/OSK/keyHighlight");
                shiftKey->style.textConfig = stc;
            } else {
                shiftKey->style.textConfig = nullptr;
            }
        }
    }
}

void OSK::OnKey() {
    shiftLock = false;
    RefreshPreview();
}

void OSK::RefreshPreview() {
    auto& tc = PreviewTC();
    if (handler->showPreview) {
        Vector2 sz = tc.Measure(handler->GetPreviewText(), preview->rect.size.x);
        preview->Resize(Vector2(preview->rect.size.x, std::max(sz.y + textHang, previewSc->rect.size.y))); // I guess a magic four will do
        
        Vector2 cp = tc.GetCursorPosition(preview->rect, handler->GetPreviewText(), handler->GetCursor());
        Vector2 cs = tc.Measure("|") + Vector2(0, textHang);
        previewSc->ScrollIntoView(VRect(cp, cs));//*/
        
    } else {
        preview->Resize(Vector2(preview->rect.size.x, 66));
    }
    preview->Refresh();
}

void OSK::DrawPreview(DrawLayerProxy& layer) {
    if (handler->showPreview) {
        auto& tc = PreviewTC();
        tc.Print(layer.rect, handler->GetPreviewText(), Vector2::zero);
        
        Vector2 cp = tc.GetCursorPosition(layer.rect, handler->GetPreviewText(), handler->GetCursor());
        string cc = "|";
        tc.Print(cp, cc);
        
        //Vector2 cs = tc.Measure("|") + Vector2(0, textHang);
        //previewSc->ScrollIntoView(VRect(cp, cs));
    } else {
        static auto tc = ThemeManager::GetMetric<starlight::TextConfig>("/dialogs/OSK/noPreview");
        tc.Print(layer.rect, "(no preview available)");
    }
}

void OSK::OnPreviewTap(DrawLayerProxy& layer) {
    if (handler->showPreview) {
        Vector2 tpos = InputManager::TouchPos() - layer.ScreenRect().pos;
        auto& tc = PreviewTC();
        handler->SetCursor(tc.GetCursorFromPoint(layer.rect, handler->GetPreviewText(), tpos));
        RefreshPreview();
    }
}
