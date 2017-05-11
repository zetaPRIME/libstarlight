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
#include "starlight/gfx/FontNull.h"

#include "starlight/gfx/RenderCore.h"
#include "starlight/gfx/BitmapFont.h"

#include "starlight/util/JsonConversions.h"

#include "starlight/util/Profiler.h"
using starlight::util::Profiler;
#include <sstream>

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
    inline unsigned int NextPow2(unsigned int x) {
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
        //Profiler::TaskStart();
        unsigned char* imgbuf;
        unsigned width, height;
        lodepng::State state;
        lodepng_decode32_file(&imgbuf, &width, &height, path.c_str());
        if (state.info_png.color.colortype != 6) isPremult = true; // expect no alpha if not rgba
        /*{
            std::stringstream ss; ss << "loaded png, ";
            ss << width; ss << "x"; ss << height;
            Profiler::TaskFinish(ss.str());
        }*/
        
        unsigned bw = NextPow2(width), bh = NextPow2(height);
        
        u8* gpubuf = static_cast<u8*>(linearAlloc(bw*bh*4));
        //Profiler::TaskStart();
        //memset(gpubuf, 0, bw*bh*4);
        //Profiler::TaskFinish("cleared canvas");
        
        
        //Profiler::TaskStart();
        if (isPremult) {
            u32* src = reinterpret_cast<u32*>(imgbuf); u32* dst = reinterpret_cast<u32*>(gpubuf);
            // just convert endianness
            for(unsigned iy = 0; iy < height; iy++) {
                for (unsigned ix = 0; ix < width; ix++) {
                    u32 clr = *src;
                    *dst = __builtin_bswap32(clr);
                    
                    src+=4; dst+=4;
                }
                dst += (bw - width) * 4; // skip the difference
            }
        } else {
            u8* src = static_cast<u8*>(imgbuf); u8* dst = static_cast<u8*>(gpubuf);
            // convert and premultiply
            for(unsigned iy = 0; iy < height; iy++) {
                for (unsigned ix = 0; ix < width; ix++) {
                    u8 r = *src++;
                    u8 g = *src++;
                    u8 b = *src++;
                    u8 a = *src++;
                    
                    float aa = (1.0f / 255.0f) * a;
                    
                    *dst++ = a;
                    *dst++ = b*aa;
                    *dst++ = g*aa;
                    *dst++ = r*aa;
                }
                dst += (bw - width) * 4; // skip the difference
            }
        }
        //Profiler::TaskFinish("made into canvas, flipped and premult");
        // completely skipping over the difference instead of erasing might eventually lead to garbage outside of frame,
        // but meh; that'll only be visible if you intentionally push the UVs outside the image proper
        
        //Profiler::TaskStart();
        CTexture* tx = RenderCore::LoadTexture(static_cast<void*>(gpubuf), bw, bh);
        tx->size = Vector2(width, height); // and for now just fix the size after the fact
        //Profiler::TaskFinish("copied into linear");
        
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

void ThemeManager::GC() {
    constexpr const int keepCycles = 5; // how many gc sweeps a drawable gets to stay loaded without being used
    std::vector<string> rem;
    // WIP
    for (auto& d : drawables) {
        if (++d.second.lastAccess > keepCycles) {
            d.second.Unload();
            if (d.second.refCount <= 0) rem.push_back(d.first); // mark for full removal when no references exist
        }
    }
    
    for (auto& s : rem) drawables.erase(s); // and remove everything queued
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
    ref.ptr = LoadAsset(path, ref);
}

shared_ptr<Drawable> ThemeManager::LoadAsset(string& path, ThemeRefContainer<Drawable>& ref) {
    static shared_ptr<Drawable> nulldrw = make_shared<starlight::gfx::DrawableTest>();
    
    string ext = FindExtension(path);
    //printf("load: %s (%s)\n", path.c_str(), ext.c_str());
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
        //printf("file contents: %s\n", st.c_str());
        
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
            ref.redir = const_cast<ThemeRefContainer<Drawable>*>(GetAsset(npath).cptr); // link containers directly
            return nulldrw; // doesn't really matter what's inside, it'll never get used
        }
        return nulldrw;
    }
    return nulldrw;
}

void ThemeManager::Fulfill(ThemeRefContainer<Font>& ref) {
    string path = ResolveFontPath(ref.name);
    if (path == "") { // no fonts found, emergency fallback
        ref.ptr = make_shared<starlight::gfx::FontNull>();
        return;
    }
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
    string pfx = "";
    
    size_t cpos = id.find(":/");
    if (cpos != string::npos) {
        pfx = id.substr(0, cpos);
        cpos += 2;
    } else cpos = 0;
    
    if (pfx == "app") {
        string sid = id.substr(cpos); // strip off the "app:/"
        // app-local asset
        // check if present in theme/app/[appname]/, else check in romfs
        for (auto thm : themeData) {
            Path bp = thm.basePath.Combine("app").Combine(Application::AppName());
            Path p = bp.Combine(sid);
            if (p.IsFile()) return p;
            p = bp.Combine(sid+".json");
            if (p.IsFile()) return p;
            p = bp.Combine(sid+".png");
            if (p.IsFile()) return p;
        }
        // TBD - directly in romfs, or in an assets folder?
        Path bp = Path("romfs:/");
        Path p = bp.Combine(sid);
        if (p.IsFile()) return p;
        p = bp.Combine(sid+".json");
        if (p.IsFile()) return p;
        p = bp.Combine(sid+".png");
        if (p.IsFile()) return p;
    }
    else if (pfx == "sdmc" || pfx == "romfs") {
        Path p = Path(id);
        if (p.IsFile()) return p;
        p = Path(id + ".json");
        if (p.IsFile()) return p;
        p = Path(id + ".png");
        if (p.IsFile()) return p;
    }
    else {
        // theme asset; check in each theme from selected to most-fallback
        for (auto thm : themeData) {
            Path p = thm.basePath.Combine(id);
            if (p.IsFile()) return p;
            p = thm.basePath.Combine(id+".json");
            if (p.IsFile()) return p;
            p = thm.basePath.Combine(id+".png");
            if (p.IsFile()) return p;
        }
    }
    
    return string();
}

string ThemeManager::ResolveFontPath(const string& id) { // there we go, nice and simple
    string pfx = "";
    
    size_t cpos = id.find(":/");
    if (cpos != string::npos) {
        pfx = id.substr(0, cpos);
        cpos += 2;
    } else cpos = 0;
    
    if (pfx == "app") {
        string sid = id.substr(cpos); // strip off the "app:/"
        // app-local asset
        // check if present in theme/app/[appname]/fonts/, else check in romfs
        for (auto thm : themeData) {
            Path bp = thm.basePath.Combine("app").Combine(Application::AppName()).Combine("fonts");
            Path p = bp.Combine(sid+".json");
            if (p.IsFile()) return p;
        }
        // TBD - directly in romfs, or in an assets folder?
        Path bp = Path("romfs:/fonts/");
        Path p = bp.Combine(sid+".json");
        if (p.IsFile()) return p;
    }
    else if (pfx == "sdmc" || pfx == "romfs") {
        // no forced "fonts" here, of course; this is an actual path
        Path p = Path(id + ".json");
        if (p.IsFile()) return p;
    }
    else {
        // theme asset; check in each theme from selected to most-fallback
        
        for (auto thm : themeData) {
            Path p = thm.basePath.Combine("fonts").Combine(id+".json");
            if (p.IsFile()) return p;
        }
    }
    
    // I guess fall back to 12px monospace if it's just nowhere to be found
    const string fallback = "mono.12";
    if (id != fallback) return ResolveFontPath(fallback);
    return string(); // fallback not found; no themes on sdmc or romfs, probably
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

void TextConfig::Print(Vector2 position, const std::string& text, Vector2 justification) { 
    if (!justification) justification = this->justification;
    font->Print(position, text, 1, textColor, justification, borderColor);
}
void TextConfig::Print(VRect rect, const std::string& text, Vector2 justification) {
    if (!justification) justification = this->justification;
    font->Print(rect, text, 1, textColor, justification, borderColor);
}

Vector2 TextConfig::Measure(const std::string& text, float maxWidth) {
    return font->Measure(text, 1, maxWidth);
}

Vector2 TextConfig::GetCursorPosition(VRect rect, const std::string& text, unsigned int end) {
    if (borderColor != Color::transparent) rect = rect.Expand(-1); // sync with drawing where bordered
    return font->GetCursorPosition(rect, text, end);
}
unsigned int TextConfig::GetCursorFromPoint(VRect rect, const std::string& text, Vector2 pt) {
    if (borderColor != Color::transparent) rect = rect.Expand(-1); // sync with drawing where bordered
    return font->GetCursorFromPoint(rect, text, pt);
}

namespace starlight { // todo: expose these in the header
    void to_json(nlohmann::json& j, const TextConfig& tc) {
        // todo: implement this
    }
    void from_json(const nlohmann::json& j, TextConfig& tc) {
        if (j.is_object()) {
            std::string inh = j.value("_inherit", "");
            if (inh != "") {
                tc = ThemeManager::GetMetric<TextConfig>(inh);
            }
            tc.font = ThemeManager::GetFont(j.value("font", tc.font.GetName()));
            tc.textColor = j.value("textColor", tc.textColor);
            tc.borderColor = j.value("borderColor", tc.borderColor);
            tc.justification = j.value("justification", tc.justification);
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
