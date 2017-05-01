#include "Thread.h"

#include "3ds.h"

using starlight::threading::Thread;
using SysThread = ::Thread;

namespace {
    void _ThreadEnter(void* arg) {
        // cast to thread and start up
        static_cast<starlight::threading::Thread*>(arg)->_FinishStart();
    }
}

Thread::~Thread() {
    // ...threadjoin? something like that??
    if (event != 0) svcCloseHandle(event);
}

void Thread::Start() {
    svcCreateEvent(&event, RESET_ONESHOT);
    sthread = static_cast<void*>(threadCreate(_ThreadEnter, static_cast<void*>(this), 4*1024, 0x3F, -2, false));
    
}
void Thread::_FinishStart() {
    // lock out once already done
    if (state != ThreadState::Init) return;
    Yield();
    Body();
}

void Thread::Yield() {
    if (state != ThreadState::Running) return; // not executing this right now, this would just futz it up
    svcWaitSynchronization(event, -1 /*U64_MAX*/);
	svcClearEvent(event);
}

void Thread::Resume() {
    if (state != ThreadState::Idle) return; // not applicable
    svcSignalEvent(event);
}

//
