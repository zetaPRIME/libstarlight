#include "ThreadTest.h"

#include "starlight/Application.h"
#include "starlight/ConfigManager.h"

using starlight::Application;

void ThreadTest::Body() {
    auto& cc = Application::GetConfig("test");
    int count = 0;
    cc.Json()["pork"] = 0;
    cc.Save();
    cc.autoSave = true;
    
    while(true) {
        
        cc.Json()["pork"] = ++count;
        //cc.Save();
        Yield();
    }
}
