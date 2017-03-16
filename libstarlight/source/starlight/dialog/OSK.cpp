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
}

OSK::OSK(osk::InputHandler* handler) : Form(true), handler(handler) {
    priority = 1000; // probably don't want all that much displaying above the keyboard
    handler->parent = this;
    
    auto cover = std::make_shared<Image>(touchScreen->rect.Expand(4), "decorations/dialog.modal-cover");
    cover->blockTouch = true;
    touchScreen->Add(cover);
    
    // wip
    
    setContainer = std::make_shared<ui::UIContainer>(VRect::touchScreen);
    touchScreen->Add(setContainer);
    
    auto actSym = [this](Button& key){
        this->handler->InputSymbol(key.label);
        this->OnKey();
    };
    
    Vector2 bs(24, 32);
    Vector2 bpen;
    Vector2 bpstart(160-bs.x*(12.5/2), 68);
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
            auto key = std::make_shared<Button>(VRect(bpen, bs));
            if (c == ' ') key->rect.size.x *= 6;
            key->SetText(string(1, c));
            key->eOnTap = actSym;
            setContainer->Add(key);
            
            // upper
            key = std::make_shared<Button>(VRect(bpen + Vector2(0, 1000), bs));
            if (C == ' ') key->rect.size.x *= 6;
            key->SetText(string(1, C));
            key->eOnTap = actSym;
            setContainer->Add(key);
            
            // and after
            bpen.x += key->rect.size.x;
        }
    }
    
    // backspace
    bpen = bpstart + bs * Vector2(linestart[3] + 10, 3);
    auto key = std::make_shared<Button>(VRect(bpen, bs));
    key->rect.size.x *= 1.25;
    key->SetText("< <");
    key->eOnTap = [this](auto& btn){ this->handler->Backspace(); this->OnKey(); };
    touchScreen->Add(key);
    
    // enter
    bpen = bpstart + bs * Vector2(linestart[4] + 8, 4);
    key = std::make_shared<Button>(VRect(bpen, bs));
    key->rect.size.x *= 2.5;
    key->SetText("Enter");
    key->eOnTap = [this](auto& btn){ this->handler->Enter(); this->OnKey(); };
    touchScreen->Add(key);
    
    preview = std::make_shared<DrawLayerProxy>(VRect::touchScreen.TopEdge(68).Expand(-2), [this](auto& layer){ this->DrawPreview(layer); }, true);
    preview->eOnTap = [this](auto& layer){ this->OnPreviewTap(layer); };
    touchScreen->Add(preview);
}

void OSK::Update(bool focused) {
    if (handler->done) {
        Close();
    }
    if (focused) {
        if (InputManager::Pressed(Keys::B)) handler->Done();
        if (true || handler->showPreview) {
            if (InputManager::Pressed(Keys::DPadLeft)) {
                handler->SetCursor(handler->GetCursor() - 1);
                preview->Refresh();
            }
            if (InputManager::Pressed(Keys::DPadRight)) {
                handler->SetCursor(handler->GetCursor() + 1);
                preview->Refresh();
            }
            
            auto& tc = PreviewTC();
            if (InputManager::Pressed(Keys::DPadUp)) {
                Vector2 pt = tc.GetCursorPosition(preview->rect, handler->GetPreviewText(), handler->GetCursor());
                string msr = "|";
                pt.y -= tc.Measure(msr).y * 0.5f;
                handler->SetCursor(tc.GetCursorFromPoint(preview->rect, handler->GetPreviewText(), pt));
                preview->Refresh();
            }
            if (InputManager::Pressed(Keys::DPadDown)) {
                Vector2 pt = tc.GetCursorPosition(preview->rect, handler->GetPreviewText(), handler->GetCursor());
                string msr = "|";
                pt.y += tc.Measure(msr).y * 1.5f;
                handler->SetCursor(tc.GetCursorFromPoint(preview->rect, handler->GetPreviewText(), pt));
                preview->Refresh();
            }
        }
        
        float& s = setContainer->scrollOffset.y;
        float ts = 0;
        if (InputManager::Held(Keys::L) || InputManager::Held(Keys::R)) {
            ts = 1000;
        }
        if (s != ts) {
            s = ts;
            setContainer->MarkForRedraw();
        }
    }
}

void OSK::OnKey() {
    preview->Refresh();
}

void OSK::DrawPreview(DrawLayerProxy& layer) {
    if (true || handler->showPreview) {
        auto& tc = PreviewTC();
        tc.Print(layer.rect, handler->GetPreviewText(), Vector2::zero);
        
        Vector2 cp = tc.GetCursorPosition(layer.rect, handler->GetPreviewText(), handler->GetCursor());
        string cc = "|";
        tc.Print(cp, cc, Vector2::zero);
    }
}

void OSK::OnPreviewTap(DrawLayerProxy& layer) {
    Vector2 tpos = InputManager::TouchPos() - layer.ScreenRect().pos;
    auto& tc = PreviewTC();
    handler->SetCursor(tc.GetCursorFromPoint(layer.rect, handler->GetPreviewText(), tpos));
    preview->Refresh();
}
