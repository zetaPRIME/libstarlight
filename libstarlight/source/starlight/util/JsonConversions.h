#pragma once
#include "starlight/_global.h"

#include "starlight/_incLib/json_fwd.hpp"

#include "starlight/datatypes/Vector2.h"
#include "starlight/datatypes/VRect.h"
#include "starlight/datatypes/Color.h"

namespace starlight {
    
    void to_json(nlohmann::json& j, const Vector2& v);
    void from_json(const nlohmann::json& j, Vector2& v);
    
    void to_json(nlohmann::json& j, const VRect& r);
    void from_json(const nlohmann::json& j, VRect& r);
    
    void to_json(nlohmann::json& j, const Color& c);
    void from_json(const nlohmann::json& j, Color& c);
    
    //
}
