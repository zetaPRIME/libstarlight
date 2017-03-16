#pragma once
#include "starlight/_global.h"

#include <string>

#include "starlight/gfx/DrawContextCanvas.h"

#include "starlight/ui/UIElement.h"

namespace starlight {
    namespace ui {
        class TextBox : public UIElement {
        private:
            //
            
        public:
            std::string text = "";
            bool multiLine = false;
            
            std::unique_ptr<gfx::DrawContextCanvas> textView;
            
            TextBox(VRect rect) { this->rect = rect; }
            TextBox(Vector2 pos) { this->rect = VRect(pos, Vector2(128, 24)); }
            ~TextBox() { }
            
            void SetText(const std::string& text);
            
            void PreDraw() override;
            void PreDrawOffscreen() override;
            void Draw() override;
            
            // events
            void OnResize() override;
            
            void OnTouchOn() override;
            void OnTouchOff() override;
            
            void OnDragStart() override;
            void OnDragRelease() override;
            void OnDragHold() override;
            
        };
    }
}
