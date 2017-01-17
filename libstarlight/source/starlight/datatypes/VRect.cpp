#include <stdlib.h>
#include <math.h>
#include <fastmath.h>
#include <algorithm>

#include "VRect.h"

using starlight::Vector2;
using starlight::VRect;

VRect::VRect() : pos(Vector2()), size(Vector2()) { }

VRect::~VRect() { }

VRect::VRect(float x, float y, float w, float h) : pos(x, y), size(w, h) { }
VRect::VRect(Vector2 pos, Vector2 size) : pos(pos), size(size) { }

Vector2 VRect::Center() const { return pos + (size * 0.5f); }
Vector2 VRect::TopLeft() const { return pos; }
Vector2 VRect::TopRight() const { return Vector2(pos.x + size.x, pos.y); }
Vector2 VRect::BottomLeft() const { return Vector2(pos.x, pos.y + size.y); }
Vector2 VRect::BottomRight() const { return pos + size; }

bool VRect::Overlaps(const VRect & o) const {
    Vector2 c = Center();
    Vector2 oc = o.Center();
    return fabsf(c.x - oc.x) * 2.0f < size.x + o.size.x && fabsf(c.y - oc.y) * 2.0f < size.y + o.size.y;
}
bool VRect::Contains(const Vector2 & vec) const {
    // not sure which implementation is faster
    return (pos.x <= vec.x && vec.x <= pos.x + size.x && pos.y <= vec.y && vec.y <= pos.y + size.y);
    //Vector2 c = Center();
    //return fabsf(c.x - vec.x) < size.x * 0.5f && fabsf(c.y - vec.y) < size.y * 0.5f;
}

VRect VRect::IntSnap() const { return VRect(pos.IntSnap(), size.IntSnap()); }

VRect VRect::Intersect(const VRect & o) const {
    Vector2 tl ( std::max(pos.x, o.pos.x), std::max(pos.y, o.pos.y) );
    Vector2 br ( std::min(pos.x + size.x, o.pos.x + o.size.x), std::min(pos.y + size.y, o.pos.y + o.size.y) );
    VRect res (tl, br - tl);
    if (res.size.x <= 0.0f || res.size.y <= 0.0f) return VRect(); // invalid area represented as a zero-rect
    return res;
}

VRect VRect::Expand(const Vector2& amount, const Vector2& bias) const {
    return VRect(pos + amount * bias * -2, size + amount * 2);
}

VRect VRect::Include(const Vector2& point) const { // could probably be more optimized
    Vector2 tl = TopLeft(), br = BottomRight();
    if (point.x < tl.x) tl.x = point.x;
    else if (point.x > br.x) br.x = point.x;
    if (point.y < tl.y) tl.y = point.y;
    else if (point.y > br.y) br.y = point.y;
    return VRect(tl, br - tl);
}

VRect VRect::TopEdge(float width) const     { return VRect(pos.x, pos.y, size.x, width); }
VRect VRect::BottomEdge(float width) const  { return VRect(pos.x, pos.y + size.y - width, size.x, width); }
VRect VRect::LeftEdge(float width) const    { return VRect(pos.x, pos.y, width, size.y); }
VRect VRect::RightEdge(float width) const   { return VRect(pos.x + size.x - width, pos.y, width, size.y); }

// constants
const VRect VRect::zero = VRect(); // should initialize to 0,0,0,0