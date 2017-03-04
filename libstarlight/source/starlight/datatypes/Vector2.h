#pragma once
#include "starlight/_global.h"

namespace starlight {
    class Vector2 {
    private:
        //
        
    public:
        float x = 0.0f;
        float y = 0.0f;
        
        constexpr Vector2() = default;
        constexpr Vector2(float x, float y) : x(x), y(y) { }
        constexpr Vector2(float mag) : x(mag), y(mag) { }
        ~Vector2() = default;

        float Length() const;
        Vector2 Normalized() const;
        inline float Area() const { return x * y; }
        
        Vector2 Reciprocal() const;
        Vector2 IntSnap() const;
        
        Vector2 CardinalAxis() const;
        Vector2 Cardinal() const;
        
        Vector2 Rotate(float angle) const;
        Vector2 RotateAround(const Vector2& anchor, float angle) const;
        
        inline bool operator == (const Vector2 & o) const { return x == o.x && y == o.y; }
        inline bool operator != (const Vector2 & o) const { return x != o.x || y != o.y; }
        
        inline Vector2 operator - () const { return *this * -1.0f; }
            
        inline Vector2 operator * (const float scalar) const { return Vector2(x * scalar, y * scalar); }
        inline Vector2 operator / (const float scalar) const { return Vector2(x / scalar, y / scalar); }
        
        inline Vector2 operator + (const Vector2 & o) const { return Vector2(x + o.x, y + o.y); }
        inline Vector2 operator - (const Vector2 & o) const { return Vector2(x - o.x, y - o.y); }
        inline Vector2 operator * (const Vector2 & o) const { return Vector2(x * o.x, y * o.y); }
        inline Vector2 operator / (const Vector2 & o) const { return Vector2(x / o.x, y / o.y); }
        
        inline Vector2 & operator *= (const float scalar) { x *= scalar; y *= scalar; return *this; }
        inline Vector2 & operator /= (const float scalar) { x /= scalar; y /= scalar; return *this; } // maybe reciprocate then multiply instead? maybe the compiler does that
        
        inline Vector2 & operator += (const Vector2 & o) { x += o.x; y += o.y; return *this; }
        inline Vector2 & operator -= (const Vector2 & o) { x -= o.x; y -= o.y; return *this; }
        inline Vector2 & operator *= (const Vector2 & o) { x *= o.x; y *= o.y; return *this; }
        
        inline explicit operator bool() const { return x == x && y == y; }
        
        static const Vector2 invalid;
        static const Vector2 zero;
        static const Vector2 one;
        static const Vector2 half;
        static const Vector2 h;
        static const Vector2 v;
    };
}
