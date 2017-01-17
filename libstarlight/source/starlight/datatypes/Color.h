#pragma once
#include "starlight/_global.h"

namespace starlight {
    class Color {
    private:
        //
        
    public:
        float r = 1.0f;
        float g = 1.0f;
        float b = 1.0f;
        float a = 1.0f;
        
        constexpr Color() : r(0), g(0), b(0), a(0) { }
        constexpr Color(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }
        constexpr Color(float r, float g, float b) : r(r), g(g), b(b), a(1.0f) { }
        ~Color() = default;
        
        inline Color Alpha(float alpha) const { return Color(r,g,b,alpha); }
        
        inline Color Premultiplied() const { return Color(r*a,g*a,b*a,a); }
        
        inline bool operator == (const Color& o) const { return r == o.r && g == o.g && b == o.b && a == o.a; }
        inline bool operator != (const Color& o) const { return r != o.r || g != o.g || b != o.b || a != o.a; }
        
        inline Color operator * (const Color& o) const { return Color(r * o.r, g * o.g, b * o.b, a * o.a); }
        
        // hmm. I guess this will do ¯\_(ツ)_/¯ don't really want to force cstdint
        inline operator unsigned int() const { return (((((int)(a*255))&0xFF)<<24) | ((((int)(b*255))&0xFF)<<16) | ((((int)(g*255))&0xFF)<<8) | ((((int)(r*255))&0xFF)<<0)); }
        // premult: inline operator unsigned int() const { return (((((int)(a*255))&0xFF)<<24) | ((((int)(a*b*255))&0xFF)<<16) | ((((int)(a*g*255))&0xFF)<<8) | ((((int)(a*r*255))&0xFF)<<0)); }
        
        static const Color transparent;
        static const Color white;
        static const Color black;
        static const Color lightGray;
        static const Color midGray;
        static const Color darkGray;
        
        //
    };
}
