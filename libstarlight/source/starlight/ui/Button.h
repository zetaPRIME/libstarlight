#pragma once
#include "starlight/_global.h"

#include <string>
#include <functional>

#include "starlight/ui/UIElement.h"

namespace starlight {
    namespace ui {
        class Button : public UIElement {
        private:
            //
            
        public:
            std::string label;
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

