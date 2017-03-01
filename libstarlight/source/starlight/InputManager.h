#pragma once
#include "starlight/_global.h"

#include <memory>

#include "starlight/datatypes/Vector2.h"

#include "starlight/ui/UIElement.h"

// borrow this from ctrulib
#ifndef BIT
#define BIT(n) (1U<<(n))
#endif
enum class Keys : unsigned int {
    A       = BIT(0),       ///< A
    B       = BIT(1),       ///< B
    SELECT  = BIT(2),       ///< Select
    START   = BIT(3),       ///< Start
    DRIGHT  = BIT(4),       ///< D-Pad Right
    DLEFT   = BIT(5),       ///< D-Pad Left
    DUP     = BIT(6),       ///< D-Pad Up
    DDOWN   = BIT(7),       ///< D-Pad Down
    R       = BIT(8),       ///< R
    L       = BIT(9),       ///< L
    X       = BIT(10),      ///< X
    Y       = BIT(11),      ///< Y
    ZL      = BIT(14),      ///< ZL (New 3DS only)
    ZR      = BIT(15),      ///< ZR (New 3DS only)
    TOUCH   = BIT(20),      ///< Touch (Not actually provided by HID)
    CSTICK_RIGHT = BIT(24), ///< C-Stick Right (New 3DS only)
    CSTICK_LEFT  = BIT(25), ///< C-Stick Left (New 3DS only)
    CSTICK_UP    = BIT(26), ///< C-Stick Up (New 3DS only)
    CSTICK_DOWN  = BIT(27), ///< C-Stick Down (New 3DS only)
    CPAD_RIGHT = BIT(28),   ///< Circle Pad Right
    CPAD_LEFT  = BIT(29),   ///< Circle Pad Left
    CPAD_UP    = BIT(30),   ///< Circle Pad Up
    CPAD_DOWN  = BIT(31),   ///< Circle Pad Down
    
    // Generic catch-all directions
    UP    = DUP    | CPAD_UP,    ///< D-Pad Up or Circle Pad Up
    DOWN  = DDOWN  | CPAD_DOWN,  ///< D-Pad Down or Circle Pad Down
    LEFT  = DLEFT  | CPAD_LEFT,  ///< D-Pad Left or Circle Pad Left
    RIGHT = DRIGHT | CPAD_RIGHT, ///< D-Pad Right or Circle Pad Right
};

namespace starlight {
    class InputManager;
    class DragHandle {
        friend class starlight::InputManager;
    private:
        
    protected:
        
    public:
        std::weak_ptr<starlight::ui::UIElement> wptr;
        starlight::ui::UIElement* rptr = nullptr;
        
        DragHandle() { }
        DragHandle(const DragHandle&) = delete;
        ~DragHandle() { }
        
        DragHandle& Grab(starlight::ui::UIElement* e);
        DragHandle& PassUp(bool releaseOnFail = false);
        void Release();
        
        inline starlight::ui::UIElement* get() const { if (wptr.expired()) return nullptr; return rptr; }
        inline bool valid() const { return rptr != nullptr && !wptr.expired(); };
        
        inline explicit operator bool() const { return rptr != nullptr && !wptr.expired(); }
        inline bool operator ==(starlight::ui::UIElement* e) const { return rptr == e; }
        //starlight::ui::UIElement& operator *() const { return *rptr; } // as with optref, do *not* call without checking first
    };
    
    class InputManager {
    private:
        static DragHandle drag;
        
    public:
        static constexpr const float dragThreshold = 8.0f;
        static constexpr const float flingThreshold = 5.0f;
        
        static void Update();
        
        static float DepthSlider();
        
        static Vector2 CirclePad();
        static Vector2 CStick();
        
        static bool Held(unsigned int mask);
        static inline bool Held(Keys mask) { return Held(static_cast<unsigned int>(mask)); }
        static bool Pressed(unsigned int mask);
        static inline bool Pressed(Keys mask) { return Pressed(static_cast<unsigned int>(mask)); }
        static bool Released(unsigned int mask);
        static inline bool Released(Keys mask) { return Released(static_cast<unsigned int>(mask)); }
        
        static Vector2 TouchPos();
        static Vector2 TouchDelta();
        static Vector2 TouchStart();
        static Vector2 TouchDragDist();
        static int TouchTime();
        
        static DragHandle& GetDragHandle() { return InputManager::drag; }
        
    private:
        InputManager() {}
    };
}
