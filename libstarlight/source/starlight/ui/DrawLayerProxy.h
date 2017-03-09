#pragma once
#include "starlight/_global.h"

#include <memory>
#include <functional>

#include "starlight/datatypes/Vector2.h"
#include "starlight/datatypes/VRect.h"

#include "starlight/gfx/DrawContextCanvas.h"

#include "starlight/ui/UIElement.h"

namespace starlight {
    namespace ui {
        class DrawLayerProxy : public UIElement {
        private:
            
        protected:
            
        public:
            bool useCanvas = false;
            
            std::unique_ptr<gfx::DrawContextCanvas> canvas;
            
            std::function<void(DrawLayerProxy&)> eDraw;
            
            DrawLayerProxy(VRect rect, std::function<void(DrawLayerProxy&)> drawFunc, bool useCanvas = false) : useCanvas(useCanvas), eDraw(drawFunc) { this->rect = rect; }
            ~DrawLayerProxy() override { }
            
            void Refresh();
            
            //void Update() override;
            void PreDraw() override;
            void Draw() override;
            
            //
        };
    }
}
