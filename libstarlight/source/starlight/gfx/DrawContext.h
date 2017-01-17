#pragma once
#include "starlight/_global.h"

#include "starlight/datatypes/VRect.h"
#include "starlight/datatypes/Color.h"
#include "starlight/datatypes/OptRef.h"

namespace starlight {
    class GFXManager;
    namespace gfx {
        class DrawContext {
            friend class starlight::GFXManager;
        protected:
            virtual void Open() = 0;
            virtual void Close() = 0;
            virtual bool Prepare() = 0;
            bool drawReady = false;
        public:
            const VRect rect;
            DrawContext() : rect(VRect(0,0,1,1)) { }
            DrawContext(VRect rect) : rect(rect) { }
            virtual ~DrawContext() { }
            
            virtual void Clear(Color color) { }
            virtual void Clear() { Clear(Color(0,0,0,0)); }
            
        };
    }
}

