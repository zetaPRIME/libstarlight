#pragma once
#include "starlight/_global.h"

#include "Vector2.h"

namespace starlight {
    class VRect {
    private:
        //
        
    public:
        Vector2 pos;
        Vector2 size;
        
        VRect();
        VRect(float x, float y, float w, float h);
        VRect(Vector2 pos, Vector2 size);
        ~VRect();
        
        Vector2 Center() const;
        Vector2 TopLeft() const;
        Vector2 TopRight() const;
        Vector2 BottomLeft() const;
        Vector2 BottomRight() const;
        
        bool Overlaps(const VRect & o) const;
        bool Contains(const Vector2 & vec) const;
        
        inline float Area() const { return size.Area(); }
        VRect IntSnap() const;
        
        VRect Intersect(const VRect & o) const;
        VRect Expand(const Vector2& amount, const Vector2& bias) const;
        VRect Expand(const Vector2& amount) const { return Expand(amount, Vector2::half); }
        VRect Expand(float x, float y) const { return Expand(Vector2(x, y )); }
        
        VRect Include(const Vector2& point) const;
        
        VRect TopEdge(float width) const;
        VRect BottomEdge(float width) const;
        VRect LeftEdge(float width) const;
        VRect RightEdge(float width) const;
        
        inline bool operator == (const VRect & o) const { return pos == o.pos && size == o.size; }
        inline bool operator != (const VRect & o) const { return pos != o.pos || size != o.size; }
        
        // add/sub to move...
        inline VRect operator + (const Vector2 & vec) const { return VRect(pos + vec, size); }
        inline VRect operator - (const Vector2 & vec) const { return VRect(pos - vec, size); }
        // mul/div to scale to a different mapping
        inline VRect operator * (const Vector2 & vec) const { return VRect(pos * vec, size * vec); }
        inline VRect operator / (const Vector2 & vec) const { return VRect(pos / vec, size / vec); }
        
        inline VRect & operator += (const Vector2 & vec) { pos += vec; return *this; }
        inline VRect & operator -= (const Vector2 & vec) { pos -= vec; return *this; }
        
        static const VRect zero;
    };
}
