#pragma once
#include "starlight/_global.h"

namespace starlight {
    namespace gfx {
        enum class BlendMode {
            Blend,
            Mask,
            Replace,
            
            Normal = Blend
        };
    }
}
