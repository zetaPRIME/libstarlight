#include "ConfigManager.h"

#include <fstream>
#include <cstring>

#include <unistd.h>
#include <sys/stat.h>

#include "starlight/_incLib/json.hpp"

#include "starlight/Application.h"

#include "starlight/util/FSHelper.h"

using std::string;
using std::ifstream;
using std::ofstream;

using starlight::Application;

using starlight::util::FSHelper;

using starlight::Config;
using starlight::ConfigManager;

// helper stuff
namespace {
    void ensureDirectory(const char* path, bool hidden = false) {
        struct stat st;
        if (stat(path, &st) != 0) {
            mkdir(path, 0);
            if (hidden) {
                // NYI; no idea how to do this, actually :(
            }
        }
    }
    void ensureDirectory(const string& path, bool hidden = false) { ensureDirectory(path.c_str(), hidden); }
    
}

  ////////////
 // Config //
////////////

Config::Config(const string& path) : path(path) {
    static const constexpr char* cpfx = "sdmc:/.starlight/config/";
    constexpr std::size_t cpfx_s = std::strlen(cpfx);
    // reserve string size
    fsPath = string(path.length() + cpfx_s + 5, ' '); fsPath.clear();
    // and build
    fsPath.append(cpfx); fsPath.append(path); fsPath.append(".json");
    
    // init json
    this->json = std::make_shared<nlohmann::json>();
    
    Reload();
}
Config::~Config() { }

void Config::Reload() {
    ifstream load(fsPath);
    if (load.good()) load >> *json;
}

void Config::Save() {
    ofstream save(fsPath);
    if (save.good()) save << *json;
}

  ///////////////////
 // ConfigManager //
///////////////////

std::unordered_map<std::string, std::shared_ptr<Config>> ConfigManager::cfg;

void ConfigManager::Init() {
    FSHelper::AssertDirPath("sdmc:/.starlight/config/app/" + Application::AppName());
}

void ConfigManager::End() {
    for (auto it : cfg) {
        auto& c = *it.second.get();
        if (c.autoSave) c.Save();
    }
    // actualy, don't clear in case something uses a static get
    // in the future, perhaps discard contents
}

Config& ConfigManager::Get(const string& path) {
    auto const& itr = cfg.find(path);
    if (itr == cfg.end()) {
        return *cfg.insert(std::make_pair(path, std::make_shared<Config>(path))).first->second;
    } else return *itr->second;
}
