#include "Application.h"

#include <3ds.h>

#include "starlight/GFXManager.h"
#include "starlight/ConfigManager.h"
#include "starlight/ThemeManager.h"
#include "starlight/InputManager.h"
#include "starlight/gfx/RenderCore.h"

using std::string;

using starlight::GFXManager;
using starlight::ConfigManager;
using starlight::Config;
using starlight::ThemeManager;
using starlight::InputManager;
using starlight::gfx::RenderCore;

using starlight::ui::TouchScreenCanvas;
using starlight::ui::TopScreenCanvas;

using starlight::ui::Form;
using starlight::ui::FormFlags;

using starlight::Application;

  ////////////////////
 // STATIC MEMBERS //
////////////////////

Application* Application::_currentApp = nullptr;

bool Application::Quit() {
    if (_currentApp == nullptr) return false;
    _currentApp->_appQuit = true;
    return _currentApp->_appQuit;
}

Config& Application::GetConfig(const string& path) {
    const string& appId = (_currentApp != nullptr) ? _currentApp->appId : "null";
    string np(path.length() + appId.length() + 4 + 1, ' ');
    np.clear();
    np.append("app/"); np.append(appId); np.append("/"); np.append(path);
    return ConfigManager::Get(np);
}

string Application::AppName() {
    return (_currentApp != nullptr) ? _currentApp->appId : "null";
}

  //////////////////////
 // INSTANCE MEMBERS //
//////////////////////

void Application::Run() {
    if (_currentApp != nullptr) return; // don't run two at once!
    _currentApp = this;
    
    _init();
    while (!_appQuit && aptMainLoop()) _mainLoop();
    _end();
    
    _currentApp = nullptr;
}

void Application::_init() {
    srand(time(NULL));
    romfsInit();
    ConfigManager::Init();
    RenderCore::Open();
    
    touchScreen = std::make_shared<TouchScreenCanvas>();
    topScreen = std::make_shared<TopScreenCanvas>();
    formTouchScreen = touchScreen.get();
    formTopScreen = topScreen.get();
    
    Init();
}

void Application::_end() {
    End();
    
    RenderCore::Close();
    ConfigManager::End();
}

void Application::_mainLoop() {
    if (!forms.empty()) {
        if (_sFormState) {
            _sFormState = false;
            
            // sort open forms
            forms.sort(Form::OrderedCompare);
            
            // reconstruct ui container heirarchy
            bool otouch, otop;
            formTouchScreen->RemoveAll();
            formTopScreen->RemoveAll();
            
            for (auto it = forms.rbegin(); it != forms.rend(); ++it) {
                if ((*it)->IsVisible()) {
                    if (!otouch) formTouchScreen->Add((*it)->touchScreen, true);
                    if (!otop) formTopScreen->Add((*it)->topScreen, true);
                    if ((*it)->GetFlag(FormFlags::canOcclude)) {
                        if ((*it)->GetFlag(FormFlags::occludeTouch)) otouch = true;
                        if ((*it)->GetFlag(FormFlags::occludeTop)) otop = true;
                    }
                }
            }
            //
        }
    }
    
    // update step
    InputManager::Update();
    Update();
    for (auto it : forms) { // update loop for forms
        it->Update(it == forms.back());
    }
    touchScreen->Update();
    topScreen->Update();
    PostUpdate();
    
    // draw step
    RenderCore::BeginFrame();
    RenderCore::targetBottom->Clear(clearColor);
    RenderCore::targetTopLeft->Clear(clearColor);
    RenderCore::targetTopRight->Clear(clearColor);
    
    Draw();
    touchScreen->PreDraw();
    topScreen->PreDraw();
    touchScreen->Draw();
    topScreen->Draw();
    PostDraw();
    RenderCore::EndFrame();
}
