#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>
#include <time.h>
#include <math.h>
#include <3ds.h>

#include "datatypes/Vector2.h"

#include "InputManager.h"

using starlight::Vector2;
using starlight::ui::UIElement;
using starlight::DragHandle;
using starlight::InputManager;

namespace {
    unsigned int heldLast = 0;
    unsigned int heldNow = 0;
    
    Vector2 stickLeftLast;
    Vector2 stickLeftNow;
    Vector2 stickRightLast;
    Vector2 stickRightNow;
    
    Vector2 touchLast;
    Vector2 touchNow;
    Vector2 touchStart;
    int touchTime;
    
    Vector2 stickVec(circlePosition cpos) {
        Vector2 v (cpos.dx, -cpos.dy);
        
        float mag = std::min(v.Length() / 150.0f, 1.0f);
        mag = std::max(mag * 1.1f - 0.1f, 0.0f);
        
        v = v.Normalized() * mag;
        
        return v;
    }
}

void InputManager::Update() {
    hidScanInput();
    circlePosition cp;
    touchPosition tp;
    
    heldLast = heldNow;
    heldNow = hidKeysHeld();
    
    stickLeftLast = stickLeftNow;
    hidCircleRead(&cp);
    stickLeftNow = stickVec(cp);
    stickRightLast = stickRightNow;
    hidCstickRead(&cp);
    stickRightNow = stickVec(cp);
    
    touchLast = touchNow;
    hidTouchRead(&tp);
    if (Held(KEY_TOUCH)) touchNow = Vector2(tp.px, tp.py);
    
    if (Pressed(KEY_TOUCH)) touchStart = touchLast = touchNow;
    
    if (!Held(KEY_TOUCH) && !Released(KEY_TOUCH)) touchTime = 0;
    else touchTime++;
    
}

float InputManager::DepthSlider() { return (*(float*)0x1FF81080); }

Vector2 InputManager::CirclePad() { return stickLeftNow; }
Vector2 InputManager::CStick() { return stickRightNow; }

bool InputManager::Held(unsigned int mask) { return heldNow & mask; }
bool InputManager::Pressed(unsigned int mask) { return (heldNow & ~heldLast) & mask; }
bool InputManager::Released(unsigned int mask) { return (heldLast & ~heldNow) & mask; }

Vector2 InputManager::TouchPos() { return touchNow; }
Vector2 InputManager::TouchDelta() { return touchNow - touchLast; }
Vector2 InputManager::TouchStart() { return touchStart; }
Vector2 InputManager::TouchDragDist() { return touchNow - touchStart; }
int InputManager::TouchTime() { return touchTime; }

// drag stuff!
DragHandle InputManager::drag;
DragHandle& DragHandle::Grab(UIElement* e) {
    if (rptr == e) return *this;
    Release();
    rptr = e;
    wptr = e->shared_from_this();
    e->OnDragStart();
    return *this;
}

DragHandle& DragHandle::PassUp(bool releaseOnFail) {
    if (!valid()) return *this; // invalid
    UIElement* e = rptr;
    while (true) {
        if (auto p = e->parent.lock()) {
            e = p.get();
            if (e->OnDragPassed()) {
                return Grab(e);
            }
            continue;
        } else {
            if (releaseOnFail) Release();
            return *this;
        }
        break;
    }
}

#define err(nth, wat) *((unsigned int*)0x00100000+(nth))=wat;
#define ded(wat) err(0,wat)
void DragHandle::Release() {
    if (!valid()) return; // nothing to release
    UIElement* e = rptr;
    rptr = nullptr;
    wptr = std::shared_ptr<UIElement>(nullptr);
    e->OnDragRelease();
}