#include "ParallaxLayer.h"

#include "starlight/GFXManager.h"

using starlight::Vector2;

using starlight::GFXManager;

using starlight::ui::UIContainer;
using starlight::ui::ParallaxLayer;

void ParallaxLayer::Draw() {
    scrollOffset = Vector2(GFXManager::parallax * -depth, 0);
    this->UIContainer::Draw();
}