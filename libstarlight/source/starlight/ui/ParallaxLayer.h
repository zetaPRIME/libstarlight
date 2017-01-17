#pragma once
#include "starlight/_global.h"

#include "starlight/ui/UIContainer.h"

namespace starlight {
    namespace ui {
        class ParallaxLayer : public UIContainer {
        public:
            float depth = 0;
            
            ParallaxLayer() { this->rect = VRect(0,0,400,240); } // screen size
            ~ParallaxLayer() { }
            
            void Draw() override;
        };
    }
}