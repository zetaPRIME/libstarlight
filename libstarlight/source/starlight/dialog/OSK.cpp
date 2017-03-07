#include "OSK.h"

#include "starlight/ThemeManager.h"
#include "starlight/InputManager.h"

#include "starlight/ui/Image.h"
#include "starlight/ui/Button.h"
#include "starlight/ui/Label.h"
#include "starlight/ui/ScrollField.h"

#include "starlight/ConfigManager.h"

using std::string;

using starlight::InputManager;

using starlight::ui::Image;
using starlight::ui::Button;
using starlight::ui::Label;
using starlight::ui::ScrollField;

using starlight::ui::Form;

using starlight::dialog::OSK;

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
    key->eOnTap = [this](auto& btn){ this->handler->Backspace(); };
    touchScreen->Add(key);
    
    bpen = bpstart + bs * Vector2(linestart[4] + 8, 4);
    key = std::make_shared<Button>(VRect(bpen, bs));
    key->rect.size.x *= 2.5;
    key->SetText("Enter");
    key->eOnTap = [this](auto& btn){ this->handler->Enter(); };
    touchScreen->Add(key);
    
}

void OSK::Update(bool focused) {
    if (focused) {
        if (InputManager::Pressed(Keys::B)) handler->Done();
        
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

void OSK::OnSymKey(const string& chr) {
    //pText->append(chr);
    //pText->append("sackboy ");
    //auto& tx = *pText;
    //ConfigManager::Get("user").Json()["log"].push_back(*pText);
    //tx.assign("shickaxe");
    //if (eOnModify) eOnModify();
}
