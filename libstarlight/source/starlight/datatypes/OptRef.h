#pragma once
#include "starlight/_global.h"

namespace starlight {
    template <class T>
    class OptRef {
    private:
        T* ref;
    public:
        OptRef() : ref(nullptr) { }
        OptRef(decltype(nullptr) nul) : ref(nullptr) { }
        OptRef(const T& ref) : ref(const_cast<T*>(&ref)) { } // whatever, const cast, I know what I'm doing
        ~OptRef() { }
        
        const T& get() { return *ref; } // ... I guess :(
        
        inline explicit operator bool() const { return ref != nullptr; }
        inline const T& operator ->() { return *ref; } // meh, technically unsafe but it's not meant to be used without explicit checking
        inline operator const T&() { return *ref; } // same here
    };
}

