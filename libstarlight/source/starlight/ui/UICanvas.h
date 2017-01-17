#pragma once
#include "starlight/_global.h"

#include "starlight/gfx/DrawContext.h"

#include "starlight/ui/UIContainer.h"

namespace starlight {
    namespace ui {
        class UICanvas : public UIContainer {
        private:
            //
        protected:
            std::unique_ptr<gfx::DrawContext> drawContext;
            
            UICanvas() { }
            
        public:
            bool needsRedraw = true;
            
            UICanvas(VRect rect);
            ~UICanvas() { }
            
            void MarkForRedraw() override;
            
            void PreDrawOffscreen() override;
            void PreDraw() override;
            void Draw() override;
        };
    }
}
