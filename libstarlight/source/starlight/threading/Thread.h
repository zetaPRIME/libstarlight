#pragma once
#include "starlight/_global.h"

namespace starlight {
    namespace threading {
        enum class ThreadState : unsigned char {
            Unqueued, Init, Idle, Running
        };
        
        class Thread {
        protected:
            volatile ThreadState state;
            void* sthread;
            long unsigned int event = 0;
            
            void Start();
        public:
            ~Thread();
            
            inline ThreadState State() { return state; }
            
            void _FinishStart();
            
            void Enqueue();
            void Yield();
            void Resume();
            
            virtual void Body() = 0;
        };
    }
}
