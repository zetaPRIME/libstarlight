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

float fRand() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

bool quit = false;
int main()
{
    Core().Run();
    return 0;
    /*printf("does this draw\n");
    APT_Initialize(APPID_HOMEMENU, aptMakeAppletAttr(APTPOS_SYS, true, true), signalEvent, resumeEvent);
    while (aptMainLoop()) {
        hidScanInput();
        u32 k = hidKeysDown();
        if (k & KEY_A) printf("this is a test\n");
        if (k & KEY_START) return 0;
        
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }*/
    
    
    // Set the random seed based on the time
    srand(time(NULL));
    
    romfsInit();
    
    RenderCore::Open();
    
    float offset3d = 0.0f;
    float rad = 0.0f;
    u16 touch_x = 320/2;
    u16 touch_y = 240/2;
    touchPosition touch;
    circlePosition circle;
    u32 held;
    
    Vector2 circpos (16.0f, 16.0f);
    
    char sbuf[100];
    
    // stuff!
    auto touchscreen = std::make_shared<starlight::ui::TouchScreenCanvas>();
    //auto container = std::make_shared<starlight::ui::UIContainer>(VRect(32,32,320-32,240-32));
    //auto container = std::make_shared<starlight::ui::UICanvas>(VRect(32,32,320-32,240-32));
    auto container = std::make_shared<starlight::ui::ScrollField>(VRect(0,0,320-0,240-0));
    auto button = std::make_shared<starlight::ui::Button>(VRect(4,80,128,32));
    //auto button = std::make_shared<starlight::ui::Button>(VRect(0,0,320-32,240-32));
    touchscreen->Add(container);
    container->Add(button);
    button->label = "I'm a button.";
    button->eOnTap = [](auto& btn){
        btn.label = "I was pressed!";
        btn.eOnTap = [](auto& btn){
            btn.label = "Event swap!";
            btn.eOnTap = [](auto& btn){
                //quit = true;
                btn.label = "Clicked again!\nBunch of lines!\nNow testing scrollarea fling with some extra size!\n\n\nPotato.";
                btn.rect.size.y = 573;
            };
        };
    };
    
    Color bgColor = Color(0,0,0);
    
    consoleInit(GFX_TOP, consoleGetDefault());
    
    while (aptMainLoop()) {
        if (quit) break;

        InputManager::Update();
        held = hidKeysHeld();
        
        Vector2 cpad = InputManager::CirclePad();
        
        if (held & (KEY_Y | KEY_START)) {
            break;
        } else if (held & KEY_TOUCH) {
            hidTouchRead(&touch);
            touch_x = touch.px;
            touch_y = touch.py;
        } else if (held & (KEY_L | KEY_R)) {
            //sf2d_set_clear_color(RGBA8(rand()%255, rand()%255, rand()%255, 255));
            bgColor = Color(fRand(), fRand(), fRand());
        } else if (held & KEY_Y) rad = 0;
        
        //if (InputManager::Held(KEY_TOUCH))
        circpos = circpos + InputManager::TouchDelta();
        
        offset3d = CONFIG_3D_SLIDERSTATE * 30.0f;
        
        /*sf2d_start_frame(GFX_TOP, GFX_LEFT);
            sf2d_draw_fill_circle(offset3d + 60, 100, 35, RGBA8(0x00, 0xFF, 0x00, 0xFF));
            sf2d_draw_fill_circle(circpos.x + cpad.x * 20.0f, circpos.y + cpad.y * 20.0f, 55, RGBA8(0xFF, 0xFF, 0x00, (int)(0xFF * (0.75f * cpad.Length() + 0.25f))));
        
            sf2d_draw_rectangle_rotate(offset3d + 260, 20, 40, 40, RGBA8(0xFF, 0xFF, 0x00, 0xFF), -2.0f*rad);
            sf2d_draw_rectangle(offset3d + 20, 60, 40, 40, RGBA8(0xFF, 0x00, 0x00, 0xFF));
            sf2d_draw_rectangle(offset3d + 5, 5, 30, 30, RGBA8(0x00, 0xFF, 0xFF, 0xFF));
            //sf2d_draw_texture_rotate(tex1, offset3d + 400/2 + circle.dx, 240/2 - circle.dy, rad);
        sf2d_end_frame();//*/
        
        /*static int ct = 0;
        static u32 c1 = starlight::Color(1,0,0);
        static u32 c2 = starlight::Color(0,0,1);
        ct = ++ct % 3;
        if (ct == 0) {
            //std::swap(c1, c2);
            c1 = RGBA8(rand()%255, rand()%255, rand()%255, 255);
            c2 = RGBA8(rand()%255, rand()%255, rand()%255, 255);
            if (offset3d == 0) c1 = 0;
        }
        sf2d_start_frame(GFX_TOP, GFX_LEFT);
            sf2d_draw_rectangle(0,0,400,240,c1);
        sf2d_end_frame();
        
        sf2d_start_frame(GFX_TOP, GFX_RIGHT);
            sf2d_draw_rectangle(0,0,400,240,c2);
            //sftd_draw_text(font, 3, 1, RGBA8(255,255,255,255), 16, "THEY'RE MOULDY YOU PILLOCK\nWelcome to the secret text~");
            
        sf2d_end_frame();*/
        
        /*sf2d_start_frame(GFX_BOTTOM, GFX_LEFT); {
            
            const int tileSize = 48;
            const int tileSpace = 3;
            for (int iy = 0; iy < 4; iy++) {
                for (int ix = 0; ix < 5; ix++) {
                    sf2d_draw_rectangle(34 + ix * (tileSize+tileSpace), 3 + iy * (tileSize+tileSpace), tileSize, tileSize, RGBA8(0x7F, 0xBF, 0xFF, 0xFF));
                }
            }
            
            Vector2 test (circle.dx, circle.dy);
            test = test * .5f;
            
            Vector2 tdrag = InputManager::TouchDelta();
            
            sprintf(sbuf, "Circle pad: %f, %f\nTouch: %f, %f\nVec: %f", cpad.x, cpad.y, tdrag.x, tdrag.y, test.x);
            sftd_draw_text(font, 3, 3, RGBA8(0xff, 0xff, 0xff, 0xff), 16, sbuf);
            
            
        } sf2d_end_frame();*/
        
        /*using starlight::GFXManager;
        static auto drw = starlight::ThemeManager::GetAsset("whatever");
        static auto tgt = std::make_shared<starlight::gfx::DrawContextCanvas>(starlight::VRect(0,0,32*3,32*3));
        
        GFXManager::PushContext(con.get());
        tgt->Clear(starlight::Color(1,0,0,0.5f));
        GFXManager::PushContext(static_cast<starlight::gfx::DrawContext*>(tgt.get()));
        
        drw->Draw(Vector2(0, 0));
        drw->Draw(Vector2(24, 8));
        
        drw->Draw(Vector2(0, 63));
        drw->Draw(Vector2(48, 63));
        drw->Draw(Vector2(63, 48));
        
        GFXManager::PopContext();
        tgt->Draw(Vector2(200, 50), nullptr, nullptr, nullptr, rad*0.1f);
        
        drw->Draw(Vector2(0, 0));
        drw->Draw(Vector2(24, 8));
        
        auto rect = starlight::VRect(32, 32, 64, 32);
        for (int q = 0; q < 5; q++) {
            drw->Draw(rect, nullptr, starlight::Color(1,1,1));
            drw->Draw(rect.Expand(Vector2::one * -0.5f, Vector2::one), nullptr, starlight::Color(0.5f,0.5f,0.5f));
            drw->Draw(rect.Expand(Vector2::one * -1), nullptr, starlight::Color(0.75f,0.75f,0.75f));
            rect.pos.y += 34;
        }
        GFXManager::PopContext();*/
        
        RenderCore::BeginFrame();
        //RenderCore::targetTopLeft->Clear(Color(0,0,0));
        //RenderCore::targetTopLeft->BindTarget();
        RenderCore::targetBottom->Clear(bgColor);
        //container->rect += InputManager::TouchDelta();
        button->rect += InputManager::CirclePad() * 5.0f;
        
        touchscreen->Update();
        touchscreen->PreDraw();
        touchscreen->Draw();
        
        rad += 0.2f;
        
        RenderCore::EndFrame();
        ThemeManager::LoadProc();
    }
    
    RenderCore::Close();//sf2d_fini();
    return 0;
}
