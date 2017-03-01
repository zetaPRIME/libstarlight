#include "MessageBox.h"

#include "starlight/InputManager.h"

#include "starlight/ui/Image.h"
#include "starlight/ui/Button.h"
#include "starlight/ui/Label.h"
#include "starlight/ui/ScrollField.h"

using starlight::InputManager;

using starlight::ui::Image;
using starlight::ui::Button;
using starlight::ui::Label;
using starlight::ui::ScrollField;

using starlight::ui::Form;

using starlight::dialog::MessageBox;

MessageBox::MessageBox(Mode m, const std::string& msg, std::function<void(int)> onSelect) : Form(true) {
    priority = 10;
    eOnSelect = onSelect;
    
    VRect boxArea = VRect(160, 120, 0, 0).Expand(Vector2(300, 200)*.5);
    
    auto bg = std::make_shared<Image>(boxArea, "decorations/panel.bg");
    touchScreen->Add(bg);
    auto scroll = std::make_shared<ScrollField>(boxArea.Expand(-8, -8).TopEdge(200-16-8-32));
    touchScreen->Add(scroll);
    auto label = std::make_shared<Label>(VRect(0, 0, 300-16, 0));
    label->autoSizeV = true;
    label->SetFont("default.16");
    label->SetText(msg);
    scroll->Add(label);
    
    VRect buttonArea = boxArea.Expand(-8, -8).BottomEdge(32);
    
    switch (m) {
        case Ok: { // one button
            numButtons = 1;
            
            auto b = std::make_shared<Button>(buttonArea);
            b->SetText("OK");
            b->eOnTap = [this](auto& b){ this->OnSelect(0); };
            touchScreen->Add(b);
        } break;
        
        case OkCancel:
        case YesNo: { // two buttons
            numButtons = 2;
            
            auto b1 = std::make_shared<Button>(buttonArea.LeftEdge(buttonArea.size.x / 2 - 4));
            b1->eOnTap = [this](auto& b){ this->OnSelect(0); };
            touchScreen->Add(b1);
            
            auto b2 = std::make_shared<Button>(buttonArea.RightEdge(buttonArea.size.x / 2 - 4));
            b2->eOnTap = [this](auto& b){ this->OnSelect(1); };
            touchScreen->Add(b2);
            
            switch(m) { // labeling
                case OkCancel: {
                    b1->SetText("OK");
                    b2->SetText("Cancel");
                } break;
                case YesNo: {
                    b1->SetText("Yes");
                    b2->SetText("No");
                } break;
                
                default: {
                    b1->SetText("You forgot to");
                    b2->SetText("implement this");
                } break;
            }
        }
        
        default: break;
    }
}

void MessageBox::Update(bool focused) {
    if (focused) {
        if (InputManager::Pressed(Keys::A)) OnSelect(0);
        else if (InputManager::Pressed(Keys::B) && numButtons > 1) OnSelect(numButtons - 1);
    }
}

void MessageBox::OnSelect(int buttonId) {
    if (eOnSelect) eOnSelect(buttonId);
    Close();
}
