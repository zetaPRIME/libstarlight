#pragma once
#include "starlight/_global.h"

#include <3ds.h>

#include <functional>
#include <list>

namespace starlight {
    namespace util {
        class WorkerThread {
        private:
            Thread thread;
            Handle sync;
            volatile bool lock = false;
            volatile bool c_lock = false;
            volatile bool kill = false;
            volatile bool running = false;
            std::list<std::function<std::function<void()>*()>*> queue;
            std::list<std::function<void()>*> cqueue;
        public:
            
            WorkerThread();
            ~WorkerThread();
            
            void ThreadMain();
            
            void PushTask(std::function<std::function<void()>*()>* func);
            
            void RunCompleteQueue();
        };
    }
}

