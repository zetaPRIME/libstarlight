#include "ScrollField.h"

#include <algorithm>

#include "starlight/InputManager.h"
#include "starlight/GFXManager.h"
#include "starlight/ThemeManager.h"

using starlight::Vector2;
using starlight::Color;

using starlight::gfx::Font;

using starlight::InputManager;
using starlight::GFXManager;
using starlight::ThemeManager;

using starlight::ui::ScrollField;

void ScrollField::Update() {
    VRect contents(0,0,0,0);
    for (auto& it : children) {
        it->Update();
        contents = contents.Include(it->rect.BottomRight());
    }
    scrollMax = contents.size - rect.size;
    scrollMax.x = std::max(0.0f, scrollMax.x);
    scrollMax.y = std::max(0.0f, scrollMax.y);
    
    scrollOffset += scrollVel;
    scrollVel *= 0.932f;
    if (scrollVel.Length() < 0.1f) {
        scrollVel = Vector2::zero;
        scrollOffset = scrollOffset.IntSnap();
    } else MarkForRedraw();
    
    scrollOffset.x = std::max(0.0f, std::min(scrollOffset.x, scrollMax.x));
    scrollOffset.y = std::max(0.0f, std::min(scrollOffset.y, scrollMax.y));
}

void ScrollField::OnProcessTouchEvent() { // stop when child element touched
    if (InputManager::Pressed(KEY_TOUCH)) scrollVel = Vector2::zero;
}

void ScrollField::OnTouchOn() {
    if (InputManager::Pressed(KEY_TOUCH)) {
        InputManager::GetDragHandle().Grab(this);
    }
}

bool ScrollField::OnDragPassed() {
    scrollOffset -= InputManager::TouchDragDist() - InputManager::TouchDelta();
    return true;
}

void ScrollField::OnDragStart() {
    scrollPreVel = Vector2::zero;
}

void ScrollField::OnDragHold() {
    scrollVel = Vector2::zero;
    scrollPreVel = -InputManager::TouchDelta();
    scrollOffset -= InputManager::TouchDelta();
    
    if (InputManager::TouchDelta().Length() > 0) MarkForRedraw();
    //printf("scroll? (%f, %f); (%f, %f)\n", scrollOffset.x, scrollOffset.y, scrollMax.x, scrollMax.y);
}

void ScrollField::OnDragRelease() {
    if (InputManager::Released(KEY_TOUCH)) {
        if (scrollPreVel.Length() < InputManager::flingThreshold) scrollPreVel = Vector2::zero;
        scrollVel = scrollPreVel;
    }
}

