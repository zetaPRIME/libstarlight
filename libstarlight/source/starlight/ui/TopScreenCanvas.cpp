#include "starlight/InputManager.h"
#include "starlight/GFXManager.h"
#include "starlight/ThemeManager.h"

#include "starlight/gfx/DrawContextTopScreen.h"

#include "TopScreenCanvas.h"

using starlight::Vector2;
using starlight::VRect;

//using starlight::gfx::Drawable;
using starlight::gfx::DrawContext;
using starlight::gfx::DrawContextTopScreen;

using starlight::ui::UIElement;
using starlight::ui::UIContainer;
using starlight::ui::UICanvas;
using starlight::ui::TopScreenCanvas;

TopScreenCanvas::TopScreenCanvas() {
    rect = VRect(0, 0, 320, 240);
    drawContext = std::make_unique<DrawContextTopScreen>();
}

void TopScreenCanvas::Update() {
    // for now just update children
    this->UIContainer::Update();
}

void TopScreenCanvas::PreDraw() {
    this->UIContainer::PreDraw();
}

void TopScreenCanvas::Draw() {
    float depth = InputManager::DepthSlider();
    
    // left frame
    GFXManager::parallax = depth;
    GFXManager::PushContext(drawContext.get());
    this->UIContainer::Draw();
    GFXManager::PrepareForDrawing(); // force clear if not drawn to
    GFXManager::PopContext();
    
    if (depth > 0) { // right frame
        GFXManager::parallax = -depth;
        GFXManager::PushContext(drawContext.get());
        this->UIContainer::Draw();
        GFXManager::PrepareForDrawing(); // force clear if not drawn to
        GFXManager::PopContext();
    }
    
    // reset parallax
    GFXManager::parallax = 0;
}
