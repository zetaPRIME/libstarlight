#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <3ds.h>

#include <functional>
#include <memory>

#include "starlight/InputManager.h"
#include "starlight/datatypes/Vector2.h"

#include "starlight/ThemeManager.h"
#include "starlight/gfx/ThemeRef.h"

#include "starlight/ui/TouchScreenCanvas.h"
#include "starlight/ui/ScrollField.h"
#include "starlight/ui/Button.h"

#include "starlight/gfx/DrawableTest.h"
#include "starlight/gfx/DrawContextTouchscreen.h"
#include "starlight/gfx/DrawContextCanvas.h"
#include "starlight/GFXManager.h"

#include "starlight/gfx/RenderCore.h"

#define CONFIG_3D_SLIDERSTATE (*(float *)0x1FF81080)

#include "Core.h"

using starlight::Vector2;
using starlight::VRect;
using starlight::Color;
using starlight::InputManager;
using starlight::GFXManager;
using starlight::ThemeManager;
using starlight::gfx::RenderCore;

/*Handle *signalEvent = NULL;
Handle *resumeEvent = NULL;

Handle *nssHandle = NULL;//*/

//aptMessageCb callback = {NULL, APPID_HOMEMENU, "boop", sizeof("boop")};

/*void __appInit() {
  srvInit();
  hidInit();

  fsInit();
  sdmcInit();

  gfxInitDefault();
  consoleInit(GFX_TOP, NULL);
}//*/

int main()
{
    Core().Run();
    return 0;
}
