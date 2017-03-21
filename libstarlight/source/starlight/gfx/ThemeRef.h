#pragma once
#include "starlight/_global.h"

#include <string>
#include <memory>

#include "starlight/ThemeManager.h"

namespace starlight {
    namespace gfx {
        template <class T> class ThemeRef;
        template <class T>
        class ThemeRefContainer {
            friend class starlight::ThemeManager;
            friend class ThemeRef<T>;
        protected:
            const std::string name;
            std::shared_ptr<T> ptr = nullptr;
            ThemeRefContainer* redir = nullptr;
            unsigned int lastAccess = 0; // how many gc sweeps since last use
            void Unload(bool full = false) {
                ptr.reset();
                if (full) redir = nullptr;
            }
            
            ThemeRefContainer(std::string name, std::shared_ptr<T> ptr) : name(name), ptr(ptr) { }
            ThemeRefContainer(std::string name, T* ptr) : name(name), ptr(ptr) { }
            ThemeRefContainer(std::string name) : name(name) { }
            
            inline std::shared_ptr<T>& _getptr() {
                lastAccess = 0;
                if (!redir && !ptr)  ThemeManager::Fulfill(*this); // call thememanager to grab things
                if (redir) return redir->_getptr();
                return ptr;
            }
            inline std::shared_ptr<T>& getptr() const { return const_cast<ThemeRefContainer<T>&>(*this)._getptr(); }
        public:
            ~ThemeRefContainer() { }
            
            T* operator ->() const {
                return &*(getptr());
            }
            
            /*T& operator *() const {
                if (ptr == nullptr) {
                    ThemeManager::Fulfill(const_cast<ThemeRef<T>&>(*this)); // call thememanager to grab things
                }
                return *ptr;
            }*/
            
        };
        
        template <class T>
        class ThemeRef {
            friend class starlight::ThemeManager;
        protected:
            const ThemeRefContainer<T>* cptr;
        public:
            ThemeRef() : cptr(nullptr) { }
            ThemeRef(ThemeRefContainer<T>* c) : cptr(c) { }
            ~ThemeRef() { }
            inline const ThemeRefContainer<T>& operator ->() const { return *cptr; }
            inline explicit operator bool() const { return cptr != nullptr; }
            inline std::shared_ptr<T> GetShared() const { return (*cptr).getptr(); }
            inline const std::string& GetName() const { return (*cptr).name; }
        };
    }
}
