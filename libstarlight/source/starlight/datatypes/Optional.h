#pragma once
#include "starlight/_global.h"

#include <memory>

namespace starlight {
    template<typename T>
    class Optional {
    private:
        std::unique_ptr<T> p = nullptr;
        std::function<T&()>* getdef = nullptr;
        
        inline void initp() {
            if (!p) {
                p = std::make_unique<T>();
                if (getdef) *p = (*getdef)();
            }
        }
        
    public:
        Optional<T>() = default;
        Optional<T>(std::function<T&()>* getDefault) : getdef(getDefault) { }
        Optional<T>(nullptr_t) : p(nullptr) { }
        Optional<T>(const Optional<T>& o) { // copy operator *actually copies the inner object*
            if (o.p) {
                p = std::make_unique<T>();
                *p = *o.p;
            }
            getdef = o.getdef;
        }
        
        Optional<T>& operator=(const nullptr_t&) { p.reset(); }
        Optional<T>& operator=(const T& o) { // assign by type's assignment operator if passed a "value"
            if (!p) p = std::make_unique<T>();
            *p = o;
            return *this;
        }
        
        T& operator *() {
            initp();
            return *p;
        }
        
        T* operator ->() {
            initp();
            return &*p;
        }
        
        inline T& Get(T& defaultRef) {
            if (p) return *p;
            return defaultRef;
        }
        
        inline T& ROGet() {
            if (p) return *p;
            if (getdef) return (*getdef)();
            static T fb; return fb; // meh, hackish but you shouldn't do this without a getdef anyway
            // todo: clean this up somehow ^ (throw instead? or maybe have a static unique_ptr instead so we save memory when this is never called for a type)
        }
        
        //
    };
}
