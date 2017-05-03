#pragma once
#include "starlight/_global.h"

#include <memory>

namespace starlight {
    class Application;
    
    namespace threading {
        enum class ThreadState : unsigned char {
            Unqueued, Init, Idle, Running, Finished
        };
        
        class Thread : public std::enable_shared_from_this<Thread> {
            friend class starlight::Application;
        protected:
            
            volatile ThreadState state = ThreadState::Unqueued;
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
            void Exit();
            
            virtual void Body() = 0;
            virtual bool OnExit();
        };
    }
}
