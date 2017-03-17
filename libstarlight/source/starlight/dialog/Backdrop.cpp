#include "Backdrop.h"

#include "starlight/ui/Image.h"

using starlight::ui::Image;

using starlight::ui::Form;

using starlight::dialog::Backdrop;

Backdrop::Backdrop(std::string imgPath) : Form(true) {
    priority = -1000000; // it is, after all, a backdrop
    auto img = std::make_shared<Image>(VRect(0, 0, 400, 480), imgPath);
    topScreen->Add(img);
    touchScreen->Add(img);
    touchScreen->scrollOffset = Vector2(40, 240);
}
