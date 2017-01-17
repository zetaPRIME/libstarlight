#pragma once
#include "starlight/_global.h"

#include <memory>
#include <list>
#include <functional>

#include "starlight/datatypes/Vector2.h"
#include "starlight/datatypes/VRect.h"

#include "starlight/ui/UIElement.h"

// no forward declaration in this header

namespace starlight {
    namespace ui {
        class UIContainer : public UIElement {
        private:
            
        protected:
            std::list<std::shared_ptr<UIElement>> children;
            void _Dive(std::function<bool(UIElement*)>& check, std::function<bool(UIElement*)>& func, bool consumable, bool frontFirst, bool& finished);
            void _Dive(std::function<bool(UIElement*)>& func, bool consumable, bool frontFirst, bool& finished);
            
        public:
            Vector2 scrollOffset = Vector2::zero;
            
            VRect ViewportRect() { return VRect(scrollOffset, rect.size); }
            
            UIContainer();
            UIContainer(VRect rect) { this->rect = rect; }
            ~UIContainer();
            
            void Dive(std::function<bool(UIElement*)> check, std::function<bool(UIElement*)> func, bool consumable = true, bool frontFirst = true);
            void Dive(std::function<bool(UIElement*)> func, bool consumable = true, bool frontFirst = true);
            
            void Add(std::shared_ptr<UIElement> elem);
            //void Add(UIElement* elem);
            void Remove(std::shared_ptr<UIElement> elem);
            
            void Update() override;
            void PreDraw() override;
            void Draw() override;
            
            virtual void OnChildResize(UIElement& child) { }
        };
    }
}
