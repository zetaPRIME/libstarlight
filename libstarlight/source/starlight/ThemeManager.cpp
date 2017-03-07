#include "ThemeManager.h"
#include "starlight/gfx/ThemeRef.h"

#include <cstdlib>
#include <string>
#include <fstream>
#include <sys/stat.h>

#include <citro3d.h>

#include "starlight/_incLib/lodepng.h"
#include "starlight/_incLib/json.hpp"

#include "starlight/ConfigManager.h"

#include "starlight/Application.h"

#include "starlight/gfx/DrawableImage.h"
#include "starlight/gfx/DrawableNinePatch.h"
#include "starlight/gfx/DrawableTest.h"
#include "starlight/gfx/FontBMF.h"

#include "starlight/gfx/RenderCore.h"
#include "starlight/gfx/BitmapFont.h"

#include "starlight/util/JsonConversions.h"

using std::string;
using std::shared_ptr;
using std::make_shared;

using nlohmann::json;

using starlight::Vector2;
using starlight::VRect;
using starlight::Color;

using starlight::util::Path;

using starlight::ConfigManager;

using starlight::gfx::Drawable;
using starlight::gfx::Font;
using starlight::gfx::ThemeRef;
using starlight::gfx::ThemeRefContainer;

using starlight::gfx::DrawableImage;
using starlight::gfx::DrawableNinePatch;

using starlight::gfx::RenderCore;
using starlight::gfx::CTexture;
using starlight::gfx::BitmapFont;

using starlight::ThemeInfo;
using starlight::ThemeManager;
using starlight::TextConfig;

namespace {
    inline int NextPow2(unsigned int x) {
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return ++x >= 64 ? x : 64; // min size to keep gpu from locking
    }
    
    inline string FindExtension(const string& path) {
        auto idot = path.rfind('.');
        if (idot == string::npos || idot == 0) return string(); // no dot found, or .hiddenfile? return empty
        auto idir = path.rfind('/');
        if (idir != string::npos && idir >= idot-1) return string(); // last dot in containing directory, or dir/.hiddenfile? return empty
        return path.substr(idot+1); // return extension without dot
    }
    
    CTexture* LoadPNG(const std::string& path, bool isPremult = false) {
        unsigned char* imgbuf;
        unsigned width, height;
        lodepng_decode32_file(&imgbuf, &width, &height, path.c_str());
        
        unsigned bw = NextPow2(width), bh = NextPow2(height);
        
        u8* gpubuf = static_cast<u8*>(linearAlloc(bw*bh*4));
        
        u8* src = static_cast<u8*>(imgbuf); u8* dst = static_cast<u8*>(gpubuf);
        
        if (isPremult) {
            // just convert endianness
            for(unsigned iy = 0; iy<height; iy++) {
                for (unsigned ix = 0; ix < height; ix++) {
                    int r = *src++;
                    int g = *src++;
                    int b = *src++;
                    int a = *src++;
                    
                    *dst++ = a;
                    *dst++ = b;
                    *dst++ = g;
                    *dst++ = r;
                }
                dst += (bw - width) * 4; // skip the difference
            }
        } else {
            // convert and premultiply
            for(unsigned iy = 0; iy<height; iy++) {
                for (unsigned ix = 0; ix < height; ix++) {
                    int r = *src++;
                    int g = *src++;
                    int b = *src++;
                    int a = *src++;
                    
                    float aa = (1.0f / 255.0f) * a;
                    
                    *dst++ = a;
                    *dst++ = b*aa;
                    *dst++ = g*aa;
                    *dst++ = r*aa;
                }
                dst += (bw - width) * 4; // skip the difference
            }
        }
        // completely skipping over the difference instead of erasing might eventually lead to garbage outside of frame,
        // but meh; that'll only be visible if you intentionally push the UVs outside the image proper
        
        CTexture* tx = RenderCore::LoadTexture(static_cast<void*>(gpubuf), bw, bh);
        tx->size = Vector2(width, height); // and for now just fix the size after the fact
        
        std::free(imgbuf);
        linearFree(gpubuf);
        
        return tx;
    }
}

ThemeInfo::ThemeInfo(const std::string& name) : ThemeInfo(Path(".starlight/themes").Combine(name), name) {
    //ConfigManager::Get("user").Json()["log"].push_back(basePath);
}

ThemeInfo::ThemeInfo(const Path& path, const std::string& name) {
    this->name = name;
    basePath = path;
    meta = std::make_shared<nlohmann::json>();
    metrics = std::make_shared<nlohmann::json>();
    //if (!basePath.IsDirectory() && std::string(basePath).find("romfs:") != std::string::npos) return; // don't bother trying to load stuff
    { // using...
        std::ifstream load = basePath.Combine("meta.json").OpenI();
        if (load.good()) load >> *meta;
        load = basePath.Combine("metrics.json").OpenI();
        if (load.good()) load >> *metrics;
    }
}

std::list<ThemeInfo> ThemeManager::themeData;
std::unordered_map<std::string, ThemeRefContainer<Drawable>> ThemeManager::drawables;
std::unordered_map<std::string, ThemeRefContainer<Font>> ThemeManager::fonts;
std::list<std::function<void()>> ThemeManager::tq;

void ThemeManager::Init() {
    auto& usercfg = ConfigManager::Get("user");
    
    auto themeName = usercfg.Get<std::string>("/theme", "default", true);
    
    ThemeInfo thm = ThemeInfo(themeName);
    if (!thm.basePath.IsDirectory()) thm = ThemeInfo("default"); // fall back on default if not found
    // ...and if "default" doesn't exist, fall back on a standard location in romfs:
    if (!thm.basePath.IsDirectory()) thm = ThemeInfo(Path("romfs:/.fallback_theme", "FALLBACK"));
    if (!thm.basePath.IsDirectory()) thm = ThemeInfo(Path("romfs:/", "FALLBACK")); // fall back on root of romfs for testbed I guess
    themeData.push_back(thm);
    
    while (!(*thm.meta)["fallback"].is_null()) { // follow fallback chain
        std::string fbn = (*thm.meta)["fallback"];
        thm = ThemeInfo(fbn);
        themeData.push_back(thm);
    }
}

void ThemeManager::End() {
    
}

ThemeRef<Drawable> ThemeManager::GetAsset(const std::string& name) {
    auto const& itr = drawables.find(name);
    if (itr == drawables.end()) {
        return &drawables.insert(std::make_pair(name, ThemeRefContainer<Drawable>(name))).first->second;
    } else return &itr->second;
}

ThemeRef<Font> ThemeManager::GetFont(const std::string& name) {
    auto const& itr = fonts.find(name);
    if (itr == fonts.end()) {
        return &fonts.insert(std::make_pair(name, ThemeRefContainer<Font>(name))).first->second;
    } else return &itr->second;
}

void ThemeManager::Fulfill(ThemeRefContainer<Drawable>& ref) {
    string path = ResolveAssetPath(ref.name);
    ref.ptr = LoadAsset(path);
}

shared_ptr<Drawable> ThemeManager::LoadAsset(string& path) {
    static shared_ptr<Drawable> nulldrw = make_shared<starlight::gfx::DrawableTest>();
    
    string ext = FindExtension(path);
    printf("load: %s (%s)\n", path.c_str(), ext.c_str());
    /**/ if (ext == "png") {
        return make_shared<DrawableImage>(LoadPNG(path));
    }
    else if (ext == "json") {
        json j;
        { // using:
            std::ifstream fs(path);
            fs >> j;
        }
        auto st = j.dump();
        printf("file contents: %s\n", st.c_str());
        
        string type = j["assetType"];
        /**/ if (type == "ninepatch") {
            path.erase(path.end()-5, path.end()); path.append(".png");
            auto d = make_shared<DrawableNinePatch>(LoadPNG(path));
            d->margin = Vector2(j["margin"][0], j["margin"][1]);
            return d;
        }
        // else if (type == "") { }
        else if (type == "link") {
            string npath = ResolveAssetPath(j["path"]);
            //return LoadAsset(npath);
            return GetAsset(npath).GetShared(); // I guess this works; may need to be altered for asynchronity if I do that later
            // (perhaps by--wait no, making it the same ThemeRefContainer would require a full rearchitecture of this part @.@)
        }
        return nulldrw;
    }
    return nulldrw;
}

void ThemeManager::Fulfill(ThemeRefContainer<Font>& ref) {
    string path = ResolveFontPath(ref.name);
    auto font = make_shared<starlight::gfx::FontBMF>();
    { // using:
        json j;
        std::ifstream fs(path);
        fs >> j;
        font->font = std::make_shared<BitmapFont>(j);
    }
    path.erase(path.end()-5, path.end()); path.append(".png");
    font->font->txMain.reset(LoadPNG(path));
    path.erase(path.end()-4, path.end()); path.append(".border.png");
    font->font->txBorder.reset(LoadPNG(path));
    ref.ptr = font;
}

void ThemeManager::LoadProc() {
    while (!tq.empty()) {
        tq.front()();
        tq.pop_front();
    }
}

string ThemeManager::ResolveAssetPath(const string& id) {
    //struct stat buf;
    //string path(id.length() + 64, ' '); // preallocate buffer space
    
    static const string pfxLocal = "app:/";
    if (id.compare(0, pfxLocal.length(), pfxLocal) == 0) {
        // app-local asset
        // check if present in theme/app/[appname]/, else check in romfs
        for (auto thm : themeData) {
            Path bp = thm.basePath.Combine("app").Combine(Application::AppName());
            Path p = bp.Combine(id+".json");
            if (p.IsFile()) return p;
            p = bp.Combine(id+".png");
            if (p.IsFile()) return p;
        }
        // TBD - directly in romfs, or in an assets folder?
        Path bp = Path("romfs:");
        Path p = bp.Combine(id+".json");
        if (p.IsFile()) return p;
        p = bp.Combine(id+".png");
        if (p.IsFile()) return p;
    }
    else {
        // theme asset; check in each theme from selected to most-fallback
        for (auto thm : themeData) {
            Path p = thm.basePath.Combine(id+".json");
            if (p.IsFile()) return p;
            p = thm.basePath.Combine(id+".png");
            if (p.IsFile()) return p;
        }
    }
    
    /*path.clear(); path.append("romfs:/"); path.append(id); path.append(".json");
    printf("attempt: %s\n", path.c_str());
    if (stat(path.c_str(), &buf) == 0) return path;
    path.erase(path.end()-5, path.end()); path.append(".png");
    printf("attempt: %s\n", path.c_str());
    if (stat(path.c_str(), &buf) == 0) return path;//*/
    
    return string();
}

string ThemeManager::ResolveFontPath(const string& id) { // there we go, nice and simple
    for (auto thm : themeData) {
        Path p = thm.basePath.Combine("fonts").Combine(id+".json");
        if (p.IsFile()) return p;
    }
    
    return string();
}

json& ThemeManager::GetMetric(const string& path) {
    json::json_pointer jp(path);
    for (auto& t : themeData) {
        json& j = (*t.metrics)[jp];
        if (!j.is_null()) {
            if (j.is_object()) {
                auto& jr = j["_redir"];
                if (!jr.is_null()) return GetMetric(jr);
            }
            return j;
        }
    }
    static json jx({});
    return jx; // well, here's a null json
}

template <typename T>
T ThemeManager::GetMetric(const std::string& path, const T& defaultValue) {
    //try {
        json& j = GetMetric(path);
        if (j.is_null()) return defaultValue;
        return j;
    /*} catch (std::exception& e) {
        return defaultValue;
    }//*/
}

template <typename T>
T ThemeManager::GetMetric(const std::string& path) {
    json& j = GetMetric(path);
    if (j.is_null()) return T();
    return j;
}

TextConfig::TextConfig(const std::string& fontName, Color text, Color border) {
    font = ThemeManager::GetFont(fontName);
    textColor = text; borderColor = border;
}

void TextConfig::Print(Vector2 position, std::string& text, Vector2 justification)
    { font->Print(position, text, 1, textColor, justification, borderColor); }
void TextConfig::Print(VRect rect, std::string& text, Vector2 justification)
    { font->Print(rect, text, 1, textColor, justification, borderColor); }

namespace starlight { // todo: expose these in the header
    void to_json(nlohmann::json& j, const TextConfig& tc) {
        // todo: implement this
    }
    void from_json(const nlohmann::json& j, TextConfig& tc) {
        if (j.is_object()) {
            tc.font = ThemeManager::GetFont(j.value("font", "default.12"));
            tc.textColor = j.value("textColor", Color::white);
            tc.borderColor = j.value("borderColor", Color::transparent);
        }
        //
    }
}

// explicit type conversions for metrics
#define typeconv(t) \
    template t ThemeManager::GetMetric(const std::string&, const t &); \
    template t ThemeManager::GetMetric(const std::string&);

typeconv(Vector2);
typeconv(VRect);
typeconv(Color);

typeconv(TextConfig);
