#include <cstdlib>
#include <string>
#include <fstream>
#include <sys/stat.h>

#include <citro3d.h>

#include "starlight/_incLib/lodepng.h"
#include "starlight/_incLib/json.hpp"

#include "ThemeManager.h"
#include "starlight/gfx/ThemeRef.h"

#include "starlight/gfx/DrawableImage.h"
#include "starlight/gfx/DrawableNinePatch.h"
#include "starlight/gfx/DrawableTest.h"
#include "starlight/gfx/FontBMF.h"

#include "starlight/gfx/RenderCore.h"
#include "starlight/gfx/BitmapFont.h"

using std::string;
using std::shared_ptr;
using std::make_shared;

using nlohmann::json;

using starlight::Vector2;

using starlight::ThemeManager;
using starlight::gfx::Drawable;
using starlight::gfx::Font;
using starlight::gfx::ThemeRef;
using starlight::gfx::ThemeRefContainer;

using starlight::gfx::DrawableImage;
using starlight::gfx::DrawableNinePatch;

using starlight::gfx::RenderCore;
using starlight::gfx::CTexture;
using starlight::gfx::BitmapFont;

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

std::unordered_map<std::string, ThemeRefContainer<Drawable>> ThemeManager::drawables;
std::unordered_map<std::string, ThemeRefContainer<Font>> ThemeManager::fonts;
std::list<std::function<void()>> ThemeManager::tq;

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
    struct stat buf;
    string path(id.length() + 64, ' '); // preallocate buffer space
    path.clear(); path.append("romfs:/"); path.append(id); path.append(".json");
    printf("attempt: %s\n", path.c_str());
    if (stat(path.c_str(), &buf) == 0) return path;
    path.erase(path.end()-5, path.end()); path.append(".png");
    printf("attempt: %s\n", path.c_str());
    if (stat(path.c_str(), &buf) == 0) return path;
    
    return string();
}

string ThemeManager::ResolveFontPath(const string& id) { // this needs redone, but whatever
    struct stat buf;
    string path(id.length() + 64, ' '); // preallocate buffer space
    path.clear(); path.append("romfs:/fonts/"); path.append(id); path.append(".json");
    printf("attempt: %s\n", path.c_str());
    if (stat(path.c_str(), &buf) == 0) return path;
    path.erase(path.end()-5, path.end()); path.append(".png");
    printf("attempt: %s\n", path.c_str());
    if (stat(path.c_str(), &buf) == 0) return path;
    
    return string();
}

