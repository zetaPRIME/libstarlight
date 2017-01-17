#pragma once
#include "starlight/_global.h"

#include <string>
#include <memory>

#include "starlight/datatypes/Vector2.h"
#include "starlight/datatypes/VRect.h"
#include "starlight/datatypes/Color.h"

#include "starlight/ui/TouchScreenCanvas.h"
#include "starlight/ui/TopScreenCanvas.h"

namespace starlight {
    class Application {
          ////////////////////
         // STATIC MEMBERS //
        ////////////////////
    private:
        static Application* _currentApp;
        
    public:
        static bool Quit();
        
          //////////////////////
         // INSTANCE MEMBERS //
        //////////////////////
    private:
        bool _appQuit = false;
        void _init();
        void _mainLoop();
        void _end();
        
    public:
        const std::string appId;
        
        Color clearColor = Color::black;
        
        std::shared_ptr<ui::TouchScreenCanvas> touchScreen = nullptr;
        std::shared_ptr<ui::TopScreenCanvas> topScreen = nullptr;
        
        Application() = delete;
        Application(std::string id) : appId(id) { }
        virtual ~Application() = default;
        
        void Run();
        
        virtual void Init() { }
        virtual void Update() { }
        virtual void PostUpdate() { }
        virtual void Draw() { }
        virtual void PostDraw() { }
        virtual void End() { }
    };
}

