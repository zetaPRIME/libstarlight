#pragma once
#include "starlight/_global.h"

#include <memory>
#include <unordered_map>

//#include "starlight/_incLib/json_fwd.hpp"
#include "starlight/_incLib/json.hpp"

#include "starlight/util/Path.h"

//#include "starlight/gfx/DrawContext.h"

namespace starlight {
    class Config {
    private:
        util::Path fsPath;
        std::shared_ptr<nlohmann::json> json;
    public:
        const std::string path;
        bool autoSave = false;
        
        Config(const std::string& path);
        ~Config();
        
        void Reload();
        void Save();
        
        nlohmann::json& Json() { return *json; }
        
        template <typename T>
        T Get(const std::string& path, T defVal, bool writeDefault = false) {
            auto jp = nlohmann::json::json_pointer(path);
            auto& js = (*json)[jp];
            if (js.is_null()) {
                if (writeDefault) js = defVal;
                return defVal;
            }
            return js;
        }
        
        template <typename T>
        void Set(const std::string& path, T val) {
            (*json)[nlohmann::json::json_pointer(path)] = val;
        }
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
