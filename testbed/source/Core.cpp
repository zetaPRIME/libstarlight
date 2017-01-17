#include "Core.h"

#include <3ds.h>

#include "starlight/InputManager.h"
#include "starlight/ThemeManager.h"
#include "starlight/GFXManager.h"
#include "starlight/gfx/RenderCore.h"

#include "starlight/ui/ParallaxLayer.h"
#include "starlight/ui/ScrollField.h"
#include "starlight/ui/Button.h"
#include "starlight/ui/Label.h"

using starlight::Vector2;
using starlight::VRect;
using starlight::Color;
using starlight::InputManager;
using starlight::GFXManager;
using starlight::ThemeManager;
using starlight::gfx::RenderCore;

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
    button->eOnTap = [label](auto& btn){
        label->SetFont("default.16");
        btn.SetText("I was pressed!");
        btn.eOnTap = [label](auto& btn){
            label->borderColor = Color::black;
            btn.SetText("Event swap!");
            btn.eOnTap = [label](auto& btn){
                label->SetFont("default.12");
                btn.SetText("Clicked again!\nBunch of lines!\nNow testing scrollarea fling with some extra size!\n\n\nPotato.\nCalamari sandwich on rye with a side of octagonal pimento; a jar of butter?");
                btn.rect.size.y = 573;
            };
        };
    };
    container->Add(button);
    
    //
    
    auto parallax = std::make_shared<sl::ui::ParallaxLayer>();
    parallax->depth = 5;
    topScreen->Add(parallax);
    
    auto pipf = std::make_shared<sl::ui::Label>(VRect(0,0,400,240));
    pipf->SetFont("default.16"); pipf->borderColor = Color::black;
    pipf->SetText("I am the very model of something on the top screen. :D\nLorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
    parallax->Add(pipf);
    
    clearColor = Color(0.0f, 0.5f, 0.5f);
    
    //
}

void Core::End() {
    
}

void Core::Update() {
    if (InputManager::Held(KEY_Y) || InputManager::Pressed(KEY_START)) Application::Quit();
}

