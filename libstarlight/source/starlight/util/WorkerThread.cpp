#include "WorkerThread.h"

using starlight::util::WorkerThread;

namespace { // helpers
    void EnterNew(void* arg) {
        static_cast<WorkerThread*>(arg)->ThreadMain();
    }
}

WorkerThread::WorkerThread() {
    svcCreateEvent(&sync, RESET_ONESHOT);
    s32 priority = 0;
    svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
    thread = threadCreate(EnterNew, static_cast<void*>(this), 4*1024, priority-1, -2, false);
}

WorkerThread::~WorkerThread() {
    kill = true;
    threadJoin(thread, U64_MAX);
    threadFree(thread);
}

void WorkerThread::ThreadMain() {
    if (running) return; // wait, how did you get here twice?
    running = true;
    while (!kill) {
        svcWaitSynchronization(sync, U64_MAX);
        while (lock) { }
        lock = true;
        while (!queue.empty()) {
            auto func = queue.front();
            queue.pop_front();
            lock = false;
            
            auto cfunc = (*func)();
            delete func;
            
            if (cfunc) {
                while (c_lock) { }
                c_lock = true;
                cqueue.push_back(cfunc);
                c_lock = false;
            }
            
            while (lock) { }
            lock = true;
        }
        lock = false;
    }
    running = false;
}

void WorkerThread::RunCompleteQueue() {
    while (c_lock) { }
    c_lock = true;
    while (!cqueue.empty()) {
        auto cfunc = cqueue.front();
        cqueue.pop_front();
        c_lock = false;
        
        (*cfunc)();
        delete cfunc;
        
        while (c_lock) { }
        c_lock = true;
    }
    c_lock = false;
}

void WorkerThread::PushTask(std::function<std::function<void()>*()>* func) {
    while (lock) { }
    lock = true;
    queue.push_back(func);
    lock = false;
    svcSignalEvent(sync);
}
