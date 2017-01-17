#include "Image.h"

#include "starlight/GFXManager.h"

using std::string;

using starlight::GFXManager;

using starlight::ui::Image;

Image::Image(VRect rect, const string& imgPath) {
    this->rect = rect;
    SetImage(imgPath, false);
}

Image::Image(Vector2 pos, const string& imgPath) {
    this->rect = VRect(pos, Vector2::zero);
    SetImage(imgPath, true);
}

void Image::SetImage(const string& imgPath, bool resize) {
    image = ThemeManager::GetAsset(imgPath);
    if (resize) this->Resize(image->Size());
    MarkForRedraw();
}

void Image::Draw() {
    image->Draw(rect + GFXManager::GetOffset());
}
