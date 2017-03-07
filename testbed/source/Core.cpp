#include "Core.h"

#include <3ds.h>

#include "starlight/_incLib/json.hpp"

#include "starlight/InputManager.h"
#include "starlight/ConfigManager.h"
#include "starlight/ThemeManager.h"
#include "starlight/GFXManager.h"
#include "starlight/gfx/RenderCore.h"

#include "starlight/util/Path.h"

#include "starlight/ui/ParallaxLayer.h"
#include "starlight/ui/ScrollField.h"
#include "starlight/ui/Button.h"
#include "starlight/ui/Label.h"

#include "starlight/dialog/MessageBox.h"
#include "starlight/dialog/OSK.h"

using starlight::Vector2;
using starlight::VRect;
using starlight::Color;
using starlight::InputManager;
using starlight::ConfigManager;
using starlight::ThemeManager;
using starlight::GFXManager;
using starlight::gfx::RenderCore;

using starlight::util::Path;

using starlight::Application;

void Core::Init() {
    //consoleInit(GFX_TOP, consoleGetDefault());
    
    auto container = std::make_shared<sl::ui::ScrollField>(VRect(0,0,320-0,240-0));
    touchScreen->Add(container);
    
    auto label = std::make_shared<sl::ui::Label>(VRect(0,0,320,0));
    label->autoSizeV = true;
    //label->justification = Vector2::zero;
    //label->SetText("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
    label->SetText("~libstardust UI test~\n\nHello. I'm a label.\nI have multiple lines and can resize to fit my content. Did you know that miles per gallon is actually a measure of volume?");
    //label->SetFont("default.16");
    //label->SetFont("default.12");
    container->Add(label);
    
    auto button = std::make_shared<sl::ui::Button>(VRect(64,80,128,32));
    button->SetText("I'm a button.");
    button->eOnTap = [](auto& btn){
        auto form = std::make_shared<sl::ui::Form>(true);
        auto label = std::make_shared<sl::ui::Label>(VRect(0,0,320,0));
        label->autoSizeV = true;
        label->SetText("This is a form, coming in and nuking the non-form UI elements. Whoops.");
        form->touchScreen->Add(label);
        auto xbtn = std::make_shared<sl::ui::Button>(VRect(4,24,32,32));
        //xbtn->SetText(" ");
        xbtn->eOnTap = [](auto& btn){
            Application::Quit();
        };
        form->touchScreen->Add(xbtn);
        form->Open();
        //auto msg = sl::dialog::MessageBox::New(sl::dialog::MessageBox::OkCancel, "This is a modal dialog!\n\n\n\nScrolly\n\n\n\nscrolly\n\n\n\nscrolly\n\n\n\nscrolly\n\n\n\nscroll!");
        //msg->Open();
        auto tlbl = std::make_shared<sl::ui::Label>(VRect(2, 2, 396, 0));
        tlbl->autoSizeV = true;
        tlbl->SetPreset("normal.16");
        tlbl->justification = Vector2::zero;
        tlbl->textConfig.borderColor = Color::black;
        tlbl->SetText("3DS:~# ");
        form->topScreen->Add(tlbl);
        auto kb = sl::dialog::OSK::New(new sl::dialog::osk::InputHandlerDirectEdit(&(tlbl->text), true, 7, [tlbl](){tlbl->Refresh();}));
            //&(tlbl->text), [tlbl](){tlbl->Refresh();});
        kb->Open();
        
        /*label->SetFont("default.16");
        btn.SetText("I was pressed!");
        btn.eOnTap = [label](auto& btn){
            label->borderColor = Color::black;
            btn.SetText("Event swap!");
            btn.eOnTap = [label](auto& btn){
                label->SetFont("default.12");
                btn.SetText("Clicked again!\nBunch of lines!\nNow testing scrollarea fling with some extra size!\n\n\nPotato.\nCalamari sandwich on rye with a side of octagonal pimento; a jar of butter?");
                btn.rect.size.y = 573;
                
                auto form = std::make_shared<sl::ui::Form>(true);
                auto label = std::make_shared<sl::ui::Label>(VRect(0,0,320,0));
                label->autoSizeV = true;
                label->SetText("This is a form, coming in and nuking the non-form UI elements. Whoops.");
                form->touchScreen->Add(label);
                form->Open();
                
                form = std::make_shared<sl::ui::Form>(true);
                auto xbtn = std::make_shared<sl::ui::Button>(VRect(32,32,128,32));
                form->touchScreen->Add(xbtn);
                form->Open();
                xbtn->eOnTap = [form](auto& btn){
                    //form->Close();
                    auto msg = sl::dialog::MessageBox::New(sl::dialog::MessageBox::Ok, "This is a modal dialog!\n\n\n\nScrolly\n\n\n\nscrolly\n\n\n\nscrolly\n\n\n\nscrolly\n\n\n\nscroll!");
                    msg->Open();
                };
            };
        };*/
    };
    container->Add(button);
    
    //
    
    auto parallax = std::make_shared<sl::ui::ParallaxLayer>();
    parallax->depth = 5;
    topScreen->Add(parallax);
    
    auto pipf = std::make_shared<sl::ui::Label>(VRect(0,0,400,240));
    pipf->SetPreset("normal.16"); pipf->textConfig.borderColor = Color::black;
    //pipf->SetText("I am the very model of something on the top screen. :D\nLorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
    std::stringstream st;
    //st << "dir: " << Path("sdmc:/.starlight").IsDirectory() << "\nfile: " << Path("sdmc:/arm9loaderhax.bin").IsFile();
    st << "dir: " << Path("sdmc:/arm9loaderhax.bin").IsDirectory() << "\nfile: " << Path("sdmc:/arm9loaderhax.bin").IsFile() << "\nroot: " << Path("sdmc:/").IsDirectory();
    pipf->SetText(st.str());
    parallax->Add(pipf);
    
    Path("sdmc:/all/your/base").CreateDirectory();
    
    clearColor = Color(0.0f, 0.5f, 0.5f);
    
    // test config
    auto& cc = Application::GetConfig("test");
    cc.Json()["panini"] = "yes please!";
    cc.Save();
    
    //
}

void Core::End() {
    
}

void Core::Update() {
    if (InputManager::Held(Keys::Y) || InputManager::Pressed(Keys::Start)) Application::Quit();
}
