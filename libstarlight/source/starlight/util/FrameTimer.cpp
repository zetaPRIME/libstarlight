#include "FrameTimer.h"

#include "3ds.h"

using starlight::util::FrameTimer;

FrameTimer::FrameTimer() {
    osTickCounterStart(reinterpret_cast<TickCounter*>(&tc));
}

void FrameTimer::FrameStart() {
    osTickCounterUpdate(reinterpret_cast<TickCounter*>(&tc));
}

double FrameTimer::GetSubframe() {
    TickCounter tmp = *(reinterpret_cast<TickCounter*>(&tc));
    osTickCounterUpdate(&tmp);
    return osTickCounterRead(&tmp) * (60.0/1000.0);
}
