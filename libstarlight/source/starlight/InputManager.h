#pragma once
#include "starlight/_global.h"

#include <memory>

#include "starlight/datatypes/Vector2.h"

#include "starlight/ui/UIElement.h"

// based on ctrulib's enum
#ifndef BIT
#define BIT(n) (1U<<(n))
#endif
enum class Keys : unsigned int {
    A           = BIT(0),               ///< A
    B           = BIT(1),               ///< B
    Select      = BIT(2),               ///< Select
    Start       = BIT(3),               ///< Start
    DPadRight   = BIT(4),               ///< D-Pad Right
    DPadLeft    = BIT(5),               ///< D-Pad Left
    DPadUp      = BIT(6),               ///< D-Pad Up
    DPadDown    = BIT(7),               ///< D-Pad Down
    R           = BIT(8),               ///< R
    L           = BIT(9),               ///< L
    X           = BIT(10),              ///< X
    Y           = BIT(11),              ///< Y
    ZL          = BIT(14),              ///< ZL (New 3DS only)
    ZR          = BIT(15),              ///< ZR (New 3DS only)
    Touch       = BIT(20),              ///< Touch (Not actually provided by HID)
    CStickRight = BIT(24),              ///< C-Stick Right (New 3DS only)
    CStickLeft  = BIT(25),              ///< C-Stick Left (New 3DS only)
    CStickUp    = BIT(26),              ///< C-Stick Up (New 3DS only)
    CStickDown  = BIT(27),              ///< C-Stick Down (New 3DS only)
    CPadRight   = BIT(28),              ///< Circle Pad Right
    CPadLeft    = BIT(29),              ///< Circle Pad Left
    CPadUp      = BIT(30),              ///< Circle Pad Up
    CPadDown    = BIT(31),              ///< Circle Pad Down
    
    // Generic catch-all directions
    Up    = DPadUp      | CPadUp,       ///< D-Pad Up or Circle Pad Up
    Down  = DPadDown    | CPadDown,     ///< D-Pad Down or Circle Pad Down
    Left  = DPadLeft    | CPadLeft,     ///< D-Pad Left or Circle Pad Left
    Right = DPadRight   | CPadRight,    ///< D-Pad Right or Circle Pad Right
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
