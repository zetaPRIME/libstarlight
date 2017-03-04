#include "Color.h"

#include "starlight/_incLib/json.hpp"

using starlight::Color;

const Color Color::transparent = Color(0.0f, 0.0f, 0.0f, 0.0f);
const Color Color::white = Color(1.0f, 1.0f, 1.0f);
const Color Color::black = Color(0.0f, 0.0f, 0.0f);
const Color Color::lightGray = Color(0.75f, 0.75f, 0.75f);
const Color Color::midGray = Color(0.5f, 0.5f, 0.5f);
const Color Color::darkGray = Color(0.25f, 0.25f, 0.25f);
