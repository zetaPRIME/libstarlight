#pragma once
#include "starlight/_global.h"

#include <unordered_map>
#include <memory>

#include "starlight/ui/UICanvas.h"

namespace starlight {
    namespace ui {
        class TouchScreenCanvas : public UICanvas {
        private:
            std::unordered_map<UIElement*, std::weak_ptr<UIElement>> _touched1 = std::unordered_map<UIElement*, std::weak_ptr<UIElement>>();
            std::unordered_map<UIElement*, std::weak_ptr<UIElement>> _touched2 = std::unordered_map<UIElement*, std::weak_ptr<UIElement>>();
            std::unordered_map<UIElement*, std::weak_ptr<UIElement>>* touchedNow = &this->_touched1;
            std::unordered_map<UIElement*, std::weak_ptr<UIElement>>* touchedLast = &this->_touched2;
            
        public:
            TouchScreenCanvas();
            ~TouchScreenCanvas() { }
            
            void Update() override;
            void PreDraw() override;
            void Draw() override;
        };
    }
}

