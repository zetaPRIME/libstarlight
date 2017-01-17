#include "Color.h"

using starlight::Color;

//
//#define RGBA8(r, g, b, a) ((((a)&0xFF)<<24) | (((b)&0xFF)<<16) | (((g)&0xFF)<<8) | (((r)&0xFF)<<0))

//#define RGBA8_GET_R(c) (((c) >>  0) & 0xFF)
//#define RGBA8_GET_G(c) (((c) >>  8) & 0xFF)
//#define RGBA8_GET_B(c) (((c) >> 16) & 0xFF)
//#define RGBA8_GET_A(c) (((c) >> 24) & 0xFF)

const Color Color::transparent = Color(0.0f, 0.0f, 0.0f, 0.0f);
const Color Color::white = Color(1.0f, 1.0f, 1.0f);
const Color Color::black = Color(0.0f, 0.0f, 0.0f);
const Color Color::lightGray = Color(0.75f, 0.75f, 0.75f);
const Color Color::midGray = Color(0.5f, 0.5f, 0.5f);
const Color Color::darkGray = Color(0.25f, 0.25f, 0.25f);
