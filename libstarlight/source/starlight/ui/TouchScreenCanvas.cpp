#include "starlight/InputManager.h"
#include "starlight/GFXManager.h"
#include "starlight/ThemeManager.h"

#include "starlight/gfx/DrawContextTouchscreen.h"

#include "TouchScreenCanvas.h"

using starlight::Vector2;
using starlight::VRect;

//using starlight::gfx::Drawable;
using starlight::gfx::DrawContext;
using starlight::gfx::DrawContextTouchscreen;

using starlight::ui::UIElement;
using starlight::ui::UIContainer;
using starlight::ui::UICanvas;
using starlight::ui::TouchScreenCanvas;

TouchScreenCanvas::TouchScreenCanvas() : touchedNow(&this->_touched1), touchedLast(&this->_touched2) {
    rect = VRect(0, 0, 320, 240);
    drawContext = std::make_unique<DrawContextTouchscreen>();
    
    //touchedNow = _touched1;
    //touchedLast = _touched2;
}

void TouchScreenCanvas::Update() {
    std::swap(touchedLast, touchedNow);
    touchedNow->clear();
    
    // scan input
    Vector2 tpos = InputManager::TouchPos();
    auto& drag = InputManager::GetDragHandle();
    if (!drag.valid() && InputManager::Held(KEY_TOUCH)) {
        Dive(
            [&tpos](UIElement* e){
                if (e->ScreenRect().Contains(tpos)) {
                    e->OnProcessTouchEvent();
                    return true;
                } return false;
            },
            [&tpos, this](UIElement* e){
                touchedNow->insert({e, std::weak_ptr<UIElement>(e->shared_from_this())});
                return e->InterceptsTouch(tpos - e->ScreenRect().pos);
            }, true, true);
    } else if (drag.valid()) {
        UIElement* e = drag.get();
        if (InputManager::Held(KEY_TOUCH) && e != nullptr) {
            if (e->ScreenRect().Contains(tpos)) {
                touchedNow->insert({e, std::weak_ptr<UIElement>(e->shared_from_this())});
            }
            e->OnDragHold();
        } else {
            drag.Release();
        }
    }
    
    for (auto& it : *touchedLast) {
        if (touchedNow->find(it.first) == touchedNow->end()) { // released
            if (auto e = it.second.lock()) {
                e->OnTouchOff();
            }
        }
    }
    for (auto& it : *touchedNow) {
        if (auto e = it.second.lock()) {
            if (touchedLast->find(it.first) == touchedLast->end()) e->OnTouchOn();
            else e->OnTouchHold();
        }
    }
    
    // and update children
    this->UIContainer::Update();
}

void TouchScreenCanvas::PreDraw() {
    this->UIContainer::PreDraw();
}

void TouchScreenCanvas::Draw() {
    GFXManager::PushContext(drawContext.get());
    this->UIContainer::Draw();
    GFXManager::PrepareForDrawing(); // force clear if not drawn to
    GFXManager::PopContext();
}
