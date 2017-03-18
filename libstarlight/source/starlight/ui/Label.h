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
        class Label : public UIElement {
        private:
            static std::function<TextConfig&()> defCfg;
            
            void AutoSize();
            
        public:
            std::string text = "";
            Optional<TextConfig> textConfig = &defCfg;
            
            std::unique_ptr<gfx::DrawContextCanvas> buffer;
            
            bool autoSizeV = false;
            
            Label(VRect rect);
            ~Label() override { }
            
            void SetText(const std::string& text);
            void SetFont(const std::string& fontName);
            void SetPreset(const std::string& name);
            
            void Refresh();
            
            void PreDrawOffscreen() override;
            void PreDraw() override;
            void Draw() override;
        };
    }
}
