#pragma once
#include "starlight/_global.h"

#include <memory>
#include <unordered_map>

#include "starlight/_incLib/json_fwd.hpp"

//#include "starlight/gfx/DrawContext.h"

namespace starlight {
    class Config {
    private:
        std::string fsPath;
        std::shared_ptr<nlohmann::json> json;
    public:
        const std::string path;
        bool autoSave = false;
        
        Config(const std::string& path);
        ~Config();
        
        void Reload();
        void Save();
        
        nlohmann::json& Json() { return *json; }
    };
    
    class ConfigManager {
    private:
        static std::unordered_map<std::string, std::shared_ptr<Config>> cfg;
        
    public:
        ConfigManager() = delete;
        
        static void Init();
        static void End();
        
        static Config& Get(const std::string& path);
    };
}
