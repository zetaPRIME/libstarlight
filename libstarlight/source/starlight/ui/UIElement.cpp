#include "UIElement.h"

using starlight::Vector2;
using starlight::VRect;
using starlight::ui::UIElement;
using starlight::ui::UIContainer;

UIElement::UIElement() { }
UIElement::~UIElement() { }

void UIElement::_Dive(std::function<bool(UIElement*)>& check, std::function<bool(UIElement*)>& func, bool consumable, bool frontFirst, bool& finished) {
    if (!check(this)) return;
    finished = func(this) && consumable;
}

void UIElement::_Dive(std::function<bool(UIElement*)>& func, bool consumable, bool frontFirst, bool& finished) {
    finished = func(this) && consumable;
}

VRect UIElement::ViewRect() {
    if (auto p = parent.lock()) {
        return rect - p->scrollOffset;
    }
    return rect;
}

VRect UIElement::ScreenRect() {
    if (auto p = parent.lock()) {
        return ViewRect() + p->ScreenRect().pos;
    }
    return rect;
}

VRect& UIElement::Resize(Vector2 size) {
    if (size == rect.size) return rect; // don't trigger events on false alarm
    rect.size = size;
    OnResize();
    if (auto p = parent.lock()) {
        p->OnChildResize(*this);
    }
    return rect;
}

void UIElement::MarkForRedraw() {
    if (auto p = parent.lock()) p->MarkForRedraw();
}