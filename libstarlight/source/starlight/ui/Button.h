#pragma once
#include "starlight/_global.h"

#include <string>
#include <functional>

#include "starlight/datatypes/Optional.h"

#include "starlight/ThemeManager.h"
#include "starlight/gfx/ThemeRef.h"

#include "starlight/ui/UIElement.h"

namespace starlight {
    namespace ui {
        class Button : public UIElement {
        public:
            struct Style {
                gfx::ThemeRef<gfx::Drawable>
                    idle = nullptr,
                    press = nullptr,
                    glyph = nullptr;
                    Optional<TextConfig> textConfig = &Button::defCfg;
            };
        private:
            static std::function<TextConfig&()> defCfg;
            
        public:
            Style style;
            
            std::string label = "";
            std::function<void(Button&)> eOnTap;
            
            Button(VRect rect) { this->rect = rect; }
            Button(Vector2 pos) { this->rect = VRect(pos, Vector2(128, 32)); }
            ~Button() { }
            
            void SetText(const std::string& text);
            
            void Draw() override;
            
            // events
            void OnTouchOn() override;
            void OnTouchOff() override;
            
            void OnDragStart() override;
            void OnDragRelease() override;
            void OnDragHold() override;
            
        };
    }
}
