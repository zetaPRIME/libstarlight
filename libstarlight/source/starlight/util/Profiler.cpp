#include "Profiler.h"

#include <cstdio>

#include "3ds.h"

using starlight::util::Profiler;

Profiler::TickCount Profiler::tc = Profiler::TickCount();

void Profiler::TaskStart() {
    osTickCounterUpdate(reinterpret_cast<TickCounter*>(&tc));
}

void Profiler::TaskFinish(const std::string& msg) {
    osTickCounterUpdate(reinterpret_cast<TickCounter*>(&tc));
    double tm = osTickCounterRead(reinterpret_cast<TickCounter*>(&tc));
    printf("T:%f - %s\n", tm, msg.c_str());
}
