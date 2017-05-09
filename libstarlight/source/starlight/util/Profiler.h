#pragma once
#include "starlight/_global.h"

#include <string>

namespace starlight {
    namespace util {
        class Profiler {
        private:
            struct TickCount {
                unsigned long long int elapsed;
                unsigned long long int ref;
            };
            static TickCount tc;
            Profiler() {};
            
        public:
            static void TaskStart();
            static void TaskFinish(const std::string& msg);
        };
    }
}
