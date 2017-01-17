#pragma once
#include "starlight/_global.h"

#include <forward_list>

#include "starlight/gfx/DrawContext.h"

namespace starlight {
    class GFXManager {
    private:
        static std::forward_list<gfx::DrawContext*> ctxStack;
        static std::forward_list<Vector2> offsetStack;
    public:
        static float parallax;
        
        GFXManager() = delete;
        
        static void PushContext(gfx::DrawContext* context);
        static gfx::DrawContext* PopContext();
        static gfx::DrawContext* GetContext();
        
        static void PushOffset(Vector2 offset);
        static void PushOffsetAdd(Vector2 add);
        static Vector2 PopOffset();
        static Vector2 GetOffset();
        
        static void Reset();
        
        static bool PrepareForDrawing();
    };
}
