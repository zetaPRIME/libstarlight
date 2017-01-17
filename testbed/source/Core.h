#pragma once

#include "starlight/Application.h"

using starlight::Application;

class Core : public Application {
public:
    Core() : Application("starlauncher") { }
    ~Core() override = default;
    
    void Init() override;
    void End() override;
    void Update() override;

};

