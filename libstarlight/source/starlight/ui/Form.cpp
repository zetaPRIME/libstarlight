#include "Form.h"

#include <3ds.h>

#include "starlight/GFXManager.h"
#include "starlight/ConfigManager.h"
#include "starlight/ThemeManager.h"
#include "starlight/InputManager.h"
#include "starlight/gfx/RenderCore.h"

#include "starlight/Application.h"

using std::string;

using starlight::GFXManager;
using starlight::ConfigManager;
using starlight::Config;
using starlight::ThemeManager;
using starlight::InputManager;
using starlight::gfx::RenderCore;

using starlight::ui::TouchScreenCanvas;
using starlight::ui::TopScreenCanvas;

using starlight::Application;

using starlight::ui::Form;

  ////////////////////
 // STATIC MEMBERS //
////////////////////

unsigned int Form::nextShowCounter = 0;

bool Form::OrderedCompare(std::shared_ptr<Form>& f1, std::shared_ptr<Form>& f2) { // acts as < operator
    if (!f1->IsVisible()) return true;
    if (f1->priority < f2->priority) return true;
    if (f1->priority > f2->priority) return false;
    return f1->showCounter < f2->showCounter;
}

  //////////////////////
 // INSTANCE MEMBERS //
//////////////////////

Form::Form(bool useDefaults) {
    if (useDefaults) {
        touchScreen = std::make_shared<UIContainer>(VRect(0, 0, 320, 240));
        topScreen = std::make_shared<UIContainer>(VRect(0, 0, 400, 240));
        SetFlag(FormFlags::canOcclude, true);
    }
}

void Form::Open(bool showImmediately) {
    auto app = Application::Current();
    if (app == nullptr) return;
    if (GetFlag(FormFlags::open)) return;
    app->forms.push_back(shared_from_this());
    SetFlag(FormFlags::open, true);
    app->SignalFormState();
    if (showImmediately) Show();
}

void Form::Close() {
    volatile auto keepalive = shared_from_this(); // don't allow delete until Close() goes out of scope
    auto app = Application::Current();
    if (app == nullptr) return;
    app->forms.remove(shared_from_this());
    SetFlag(FormFlags::open, false);
    app->SignalFormState();
}

void Form::Show() {
    auto app = Application::Current();
    if (app == nullptr) return;
    showCounter = nextShowCounter++;
    SetFlag(FormFlags::visible, true);
    app->SignalFormState();
    OnShow();
}

void Form::Hide() {
    auto app = Application::Current();
    if (app == nullptr) return;
    if (!GetFlag(FormFlags::visible)) return; // don't signal if already hidden
    SetFlag(FormFlags::visible, false);
    app->SignalFormState();
    OnHide(); // todo: make this interceptable
}
