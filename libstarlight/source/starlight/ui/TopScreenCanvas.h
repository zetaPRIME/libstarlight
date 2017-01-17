#pragma once
#include "starlight/_global.h"

#include "starlight/ui/UICanvas.h"

namespace starlight {
    namespace ui {
        class TopScreenCanvas : public UICanvas {
        public:
            TopScreenCanvas();
            ~TopScreenCanvas() { }
            
            void Update() override;
            void PreDraw() override;
            void Draw() override;
        };
    }
}

