#pragma once
#include "starlight/_global.h"

#include <string>
#include <memory>

#include "starlight/datatypes/Optional.h"

#include "starlight/ThemeManager.h"
#include "starlight/gfx/ThemeRef.h"

#include "starlight/gfx/DrawContextCanvas.h"

#include "starlight/ui/UIElement.h"

namespace starlight {
    namespace ui {
        class DebugConsole : public UIElement {
        private:
            //
            
        public:
            std::string text = "";
            
            std::unique_ptr<gfx::DrawContextCanvas> buffer;
            bool dirty = false;
            
            DebugConsole(VRect rect);
            ~DebugConsole() override;
            
            void Start();
            
            void PreDrawOffscreen() override;
            void PreDraw() override;
            void Draw() override;
        };
    }
}
