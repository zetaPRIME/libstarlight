#include "Thread.h"

#include "3ds.h"

#include "starlight/Application.h"

using starlight::threading::Thread;
using SysThread = ::Thread;
using SThread = starlight::threading::Thread;

namespace {
    void _ThreadEnter(void* arg) {
        // cast to thread and start up
        static_cast<SThread*>(arg)->_FinishStart();
    }
}

Thread::~Thread() {
    // ...threadjoin? something like that??
    if (event != 0) svcCloseHandle(event);
}

void Thread::Enqueue() {
    if (state != ThreadState::Unqueued) return; // don't double enqueue, you derp
    Application::Current()->EnqueueThread(shared_from_this());
}

void Thread::Start() {
    state = ThreadState::Init;
    svcCreateEvent(&event, RESET_ONESHOT);
    sthread = static_cast<void*>(threadCreate(_ThreadEnter, static_cast<void*>(this), 4*1024, 0x3F, -2, false));
    
}
void Thread::_FinishStart() {
    // lock out once already done
    if (state != ThreadState::Init) return;
    state = ThreadState::Running;
    Yield();
    Body();
}

void Thread::Yield() {
    if (state != ThreadState::Running) return; // not executing this right now, this would just futz it up
    state = ThreadState::Idle;
    svcWaitSynchronization(event, -1 /*U64_MAX*/);
    //svcWaitSynchronization(event, 65536);
	svcClearEvent(event);
    state = ThreadState::Running;
}

void Thread::Resume() {
    if (state != ThreadState::Idle) return; // not applicable
    svcSignalEvent(event);
}

//
