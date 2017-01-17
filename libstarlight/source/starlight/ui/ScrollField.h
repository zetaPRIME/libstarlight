#pragma once
#include "starlight/_global.h"

#include "starlight/ui/UICanvas.h"

namespace starlight {
    namespace ui {
        class ScrollField : public UICanvas {
        public:
            Vector2 scrollMax = Vector2::zero;
            Vector2 scrollVel = Vector2::zero;
            Vector2 scrollPreVel = Vector2::zero;
            
            ScrollField(VRect rect) : UICanvas(rect) { }
            ~ScrollField() { }
            
            void Update() override;
            
            // events
            void OnTouchOn() override;
            void OnTouchOff() override { }
            void OnTouchHold() override { }
            
            void OnProcessTouchEvent() override;
            
            bool InterceptsTouch(Vector2 where) override { return true; }
            
            bool OnDragPassed() override;
            void OnDragStart() override;
            void OnDragRelease() override;
            void OnDragHold() override;

        };
    }
}

