#pragma once

#include "starlight/threading/Thread.h"

class ThreadTest : public sl::threading::Thread {
    void Body() override;
};
