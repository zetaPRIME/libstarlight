#pragma once
#include "starlight/_global.h"

namespace starlight {
    namespace util {
        class FrameTimer {
        private:
            struct TickCount {
                unsigned long long int elapsed;
                unsigned long long int ref;
            };
            TickCount tc;
        public:
            FrameTimer();
            
            void FrameStart();
            double GetSubframe();
        };
    }
}
