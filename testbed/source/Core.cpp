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
#include "starlight/ui/TextBox.h"
#include "starlight/ui/Label.h"
#include "starlight/ui/Image.h"
#include "starlight/ui/DebugConsole.h"

#include "starlight/dialog/Backdrop.h"
#include "starlight/dialog/MessageBox.h"
#include "starlight/dialog/OSK.h"

#include "ThreadTest.h"

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
    /*clearColor = Color(0,0,0.5);
    auto img = touchScreen->AddNew<sl::ui::Image>(Vector2(32, 32), "sdmc:/snes9x_3ds_top.png");
    auto lbl = touchScreen->AddNew<sl::ui::Label>(VRect(0,0,320,240));
    lbl->SetText("text go here\ntest test test\nnickelpickle");
    return;*/
    
    auto container = std::make_shared<sl::ui::ScrollField>(VRect(0,0,320-0,240-0));
    touchScreen->Add(container);
    
    auto label = std::make_shared<sl::ui::Label>(VRect(0,0,320,0));
    label->textConfig->justification = Vector2::half;
    label->autoSizeV = true;
    label->SetText("~libstarlight UI test~\n\nHello. I'm a label.\nI have multiple lines and can resize to fit my content. Did you know that miles per gallon is actually a measure of volume? " + std::to_string(sizeof(unsigned long long)));
    container->Add(label);
    
    auto button = std::make_shared<sl::ui::Button>(VRect(64,80,128,32));
    button->SetText("I'm a button.");
    button->eOnTap = [](auto& btn){
        // assemble and open a basic form
        auto form = std::make_shared<sl::ui::Form>(true);
        
        auto tbtn = form->touchScreen->AddNew<sl::ui::Button>(VRect(4, 28, 80, 24));
        tbtn->SetText("print something");
        tbtn->eOnTap = [](auto& btn){
            printf("pickles!\n");
        };
        
        /*auto label = std::make_shared<sl::ui::Label>(VRect(0,0,320,0));
        label->textConfig->justification = Vector2::half;
        label->autoSizeV = true;
        label->SetText("This is a form, coming in and nuking the non-form UI elements. Whoops.");
        form->touchScreen->Add(label);*/
        
        auto console = form->topScreen->AddNew<sl::ui::DebugConsole>(VRect::topScreen);
        console->Start();
        
        auto xbtn = std::make_shared<sl::ui::Button>(VRect(320-96,28,32,24));
        xbtn->eOnTap = [](auto& btn){
            Application::Quit();
        };
        xbtn->SetText("(exit)");
        form->touchScreen->Add(xbtn);
        
        /*auto tlbl = std::make_shared<sl::ui::Label>(VRect(2, 2, 396, 0));
        tlbl->autoSizeV = true;
        tlbl->SetPreset("normal.16");
        tlbl->textConfig->justification = Vector2::zero;
        tlbl->textConfig->borderColor = Color::black;
        tlbl->SetText("3DS:~# sudo make me a sandwich_");
        form->topScreen->Add(tlbl);*/
        
        auto tb = std::make_shared<sl::ui::TextBox>(VRect(0, 64, 320, 24).Expand(-16, 0));
        tb->text = "Single-line TextBox widget example. Tap me!";
        form->touchScreen->Add(tb);
        
        tb = std::make_shared<sl::ui::TextBox>(VRect(0, 64+24+4, 320, 24*6).Expand(-16, 0));
        tb->text = "Multi-line TextBox widget example.\nSee? Multiple lines!\nTry it for yourself;\njust tap right here...\n\n(Drag the OSK preview to scroll, tap to place cursor, and use the + Control Pad to move the cursor manually; L/R is shift, B to exit)";
        tb->multiLine = true;
        form->touchScreen->Add(tb);
        
        form->Open();
        
        // open a backdrop with the default asset
        sl::dialog::Backdrop::New()->Open();
        
        // make a test thread
        std::make_shared<ThreadTest>()->Enqueue();
        
        //
    };
    container->Add(button);
    
    //
    
    auto parallax = std::make_shared<sl::ui::ParallaxLayer>();
    parallax->depth = 5;
    topScreen->Add(parallax);
    
    auto pipf = std::make_shared<sl::ui::Label>(VRect(0,0,400,240));
    pipf->SetPreset("normal.16");
    pipf->textConfig->borderColor = Color::black;
    pipf->textConfig->justification = Vector2::half;
    //pipf->SetText("This label is on a parallax layer. Try moving the 3D slider.\n\nLorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
    pipf->SetText(ThemeManager::ResolveAssetPath("app:/decorations/osk.background"));
    parallax->Add(pipf);
    
    clearColor = Color(0.0f, 0.5f, 0.5f);
    
    // basic config example
    auto& cc = Application::GetConfig("test");
    cc.Json()["panini"] = "yes please!";
    cc.Save();
    
    //*/
}

void Core::End() {
    
}

void Core::Update() {
    //if (InputManager::Held(Keys::Y) || InputManager::Pressed(Keys::Start)) Application::Quit();
}
