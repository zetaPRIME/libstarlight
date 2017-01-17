#include <algorithm>

#include "starlight/GFXManager.h"

#include "UIContainer.h"

using starlight::GFXManager;

using starlight::ui::UIElement;
using starlight::ui::UIContainer;

UIContainer::UIContainer() { }
UIContainer::~UIContainer() { }

void UIContainer::Dive(std::function<bool(UIElement*)> check, std::function<bool(UIElement*)> func, bool consumable, bool frontFirst) {
    bool finished = false;
    _Dive(check, func, consumable, frontFirst, finished);
}

void UIContainer::Dive(std::function<bool(UIElement*)> func, bool consumable, bool frontFirst) {
    bool finished = false;
    _Dive(func, consumable, frontFirst, finished);
}

void UIContainer::_Dive(std::function<bool(UIElement*)>& check, std::function<bool(UIElement*)>& func, bool consumable, bool frontFirst, bool& finished) {
    if (!check(this)) return; // early out, don't bother checking further branches
    if (frontFirst) for (auto itr = children.rbegin(); itr != children.rend(); ++itr) {
        (*itr)->_Dive(check, func, consumable, frontFirst, finished);
        if (finished) return;
    } else for (auto itr = children.begin(); itr != children.end(); ++itr) {
        (*itr)->_Dive(check, func, consumable, frontFirst, finished);
        if (finished) return;
    }
    // same thing as plain UIElement's version
    finished = func(this) && consumable;
}

void UIContainer::_Dive(std::function<bool(UIElement*)>& func, bool consumable, bool frontFirst, bool& finished) {
    if (frontFirst) for (auto itr = children.rbegin(); itr != children.rend(); ++itr) {
        (*itr)->_Dive(func, consumable, frontFirst, finished);
        if (finished) return;
    } else for (auto itr = children.begin(); itr != children.end(); ++itr) {
        (*itr)->_Dive(func, consumable, frontFirst, finished);
        if (finished) return;
    }
    // same thing as plain UIElement's version
    finished = func(this) && consumable;
}

void UIContainer::Add(std::shared_ptr<UIElement> elem) {
    children.push_back(elem);
    elem->parent = std::weak_ptr<UIContainer>(std::static_pointer_cast<UIContainer>(this->shared_from_this()));
    MarkForRedraw();
}
//void UIContainer::Add(UIElement* elem) {
    // as it turns out, you *have* to immediately encapsulate because herp derp shared_from_this doesn't even bother to check
    //if (std::shared_ptr<UIElement> ptr = elem->shared_from_this()) this->Add(ptr);
    //else this->Add(std::shared_ptr<UIElement>(elem)); // support initial encapsulation
//}

void UIContainer::Remove(std::shared_ptr<UIElement> elem) {
    if (elem->parent.lock().get() != this) return;
    elem->parent = std::weak_ptr<UIContainer>(); // explicit null expressed in a slightly silly way
    //children.remove_if([&elem](std::shared_ptr<UIElement> ie){return ie == elem;});
    children.remove(elem); // I think this uses operator ==()?
    MarkForRedraw();
}

void UIContainer::Update() {
    for (auto& it : children) { it->Update(); }
}

void UIContainer::PreDraw() {
    VRect vr = ViewportRect();
    for (auto& it : children) { if (it->rect.Overlaps(vr)) it->PreDraw(); else it->PreDrawOffscreen(); }
}

void UIContainer::Draw() {
    GFXManager::PushOffsetAdd(rect.pos - scrollOffset);
    VRect vr = ViewportRect();
    for (auto& it : children) { if (it->rect.Overlaps(vr)) it->Draw(); }
    GFXManager::PopOffset();
}


