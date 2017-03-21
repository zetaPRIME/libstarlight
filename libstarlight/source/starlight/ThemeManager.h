#pragma once
#include "starlight/_global.h"

#include <string>
#include <unordered_map>
#include <list>
#include <memory>
#include <functional>

#include "starlight/_incLib/json_fwd.hpp"

#include "starlight/util/Path.h"

#include "starlight/gfx/Drawable.h"
#include "starlight/gfx/Font.h"

namespace starlight {
    // forward declare
    namespace gfx {
        template <class T> class ThemeRefContainer;
        template <class T> class ThemeRef;
    }
    
    struct ThemeInfo {
    public:
        std::string name;
        util::Path basePath;
        
        std::shared_ptr<nlohmann::json> meta;
        std::shared_ptr<nlohmann::json> metrics;
        
        ThemeInfo() = default;
        ThemeInfo(const std::string& name);
        ThemeInfo(const util::Path& path, const std::string& name = "");
        ~ThemeInfo() = default;
    };

    class ThemeManager {
        template <class T>
        friend class starlight::gfx::ThemeRefContainer;
    private:
        static std::unordered_map<std::string, gfx::ThemeRefContainer<gfx::Drawable>> drawables;
        static std::unordered_map<std::string, gfx::ThemeRefContainer<gfx::Font>> fonts;
        static std::list<std::function<void()>> tq;
    protected:
        static std::list<ThemeInfo> themeData;
        
        static void Fulfill(gfx::ThemeRefContainer<gfx::Drawable>& ref);
        static void Fulfill(gfx::ThemeRefContainer<gfx::Font>& ref);
        
        static std::shared_ptr<gfx::Drawable> LoadAsset(std::string& path, gfx::ThemeRefContainer<gfx::Drawable>& ref);
    public:
        ThemeManager() = delete; // "static" class
        
        static void Init();
        static void End();
        
        static void GC();
        
        static gfx::ThemeRef<gfx::Drawable> GetAsset(const std::string& name);
        static gfx::ThemeRef<gfx::Font> GetFont(const std::string& name);
        
        static void LoadProc();
        
        static std::string ResolveAssetPath(const std::string& id);
        static std::string ResolveFontPath(const std::string& id);
        
        static inline std::string GetThemeName() { return themeData.front().name; }
        static nlohmann::json& GetMetric(const std::string& path);
        template <typename T> static T GetMetric(const std::string& path, const T& defaultValue);
        template <typename T> static T GetMetric(const std::string& path);
    };
}

// post-include dependency
#include "starlight/gfx/ThemeRef.h"

// and some metrics types depending on ThemeRef
namespace starlight {
    struct TextConfig {
        gfx::ThemeRef<gfx::Font> font = ThemeManager::GetFont("default.12");
        Color textColor = Color::white;
        Color borderColor = Color::transparent;
        Vector2 justification = Vector2::zero;
        
        TextConfig() = default;
        TextConfig(const std::string& fontName, Color text, Color border = Color::transparent);
        ~TextConfig() = default;
        
        void Print(Vector2 position, const std::string& text, Vector2 justification = Vector2::invalid);
        void Print(VRect rect, const std::string& text, Vector2 justification = Vector2::invalid);
        
        Vector2 Measure(const std::string& text, float maxWidth = 65536*64);
        
        Vector2 GetCursorPosition(VRect rect, const std::string& text, unsigned int end);
        unsigned int GetCursorFromPoint(VRect rect, const std::string& text, Vector2 pt);
    };
}
