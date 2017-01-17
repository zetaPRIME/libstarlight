#pragma once
#include "starlight/_global.h"

#include "starlight/gfx/DrawContext.h"

namespace starlight {
    namespace gfx {
        class DrawContextTouchscreen : public DrawContext {
        protected:
            void Open() override;
            void Close() override;
            bool Prepare() override;
        public:
            DrawContextTouchscreen() : DrawContext(VRect(0, 0, 320, 240)) { }
            ~DrawContextTouchscreen() { }

        };
    }
}

