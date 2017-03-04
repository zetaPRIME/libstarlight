#include "JsonConversions.h"

#include "starlight/_incLib/json.hpp"

using nlohmann::json;

namespace starlight {
    
    // Vector2
    void to_json(nlohmann::json& j, const Vector2& v) {
        j = json({v.x, v.y});
    }
    void from_json(const nlohmann::json& j, Vector2& v) {
        if (j.is_array()) {
            switch(j.size()) {
                default:
                case 2: v = Vector2(j[0], j[1]); break;
                case 1: v = Vector2::one * j[0].get<float>(); break;
                case 0: v = Vector2::zero;
            }
            return;
        }
        v = Vector2::zero;
    }
    
    // VRect
    void to_json(nlohmann::json& j, const VRect& r) {
        j = json({r.pos.x, r.pos.y, r.size.x, r.size.y});
    }
    void from_json(const nlohmann::json& j, VRect& r) {
        if (j.is_array() && j.size() >= 4) { // todo: maybe support other formats
            r = VRect(j[0], j[1], j[2], j[3]);
            return;
        }
        r = VRect::zero;
    }
    
    // Color
    void to_json(nlohmann::json& j, const Color& c) {
        j = json({c.r, c.g, c.b, c.a});
    }
    void from_json(const nlohmann::json& j, Color& c) {
        if (j.is_array()) {
            c = Color::black;
            switch(j.size()) {
                default:
                case 4: c.a = j[3];
                case 3: c.b = j[2];
                case 2: c.g = j[1];
                case 1: c.r = j[0];
                case 0: break; // maybe default to Color::white here instead
            }
            return;
        } else if (j.is_string()) {
            static std::map<std::string, Color> byName = {
                {"transparent", Color::transparent},
                {"white", Color::white},
                {"black", Color::black},
                {"lightGray", Color::lightGray},
                {"midGray", Color::midGray},
                {"darkGray", Color::darkGray}
            };
            auto f = byName.find(j);
            if (f != byName.end()) {
                c = f->second;
                return;
            }
        }//*/
        c = Color::white;
    }
    
    
    //
}
