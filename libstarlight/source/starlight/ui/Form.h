#pragma once
#include "starlight/_global.h"

#include <memory>
#include <list>
#include <functional>
#include <type_traits>

#include "starlight/datatypes/Vector2.h"
#include "starlight/datatypes/VRect.h"

#include "starlight/ui/UIContainer.h"

// Form subclasses should also derive from FormCreator, with their own typename as the template argument
// class WhateverForm : public Form, public FormCreator<WhateverForm>

namespace starlight {
    namespace ui {
        enum class FormFlags : unsigned int {
            visible             = 1 << 0,
            canOcclude          = 1 << 1,
            occludeTouch        = 1 << 2,
            occludeTop          = 1 << 3,
            
            open                = 1 << 30
        };
        //typedef std::underlying_type<FormFlags>::type FormFlags_t;
        
        class Form;
        template<class F>
        class FormCreator {
        public:
            template<typename ... Args>
            static std::shared_ptr<F> New(Args... args) {
                auto f = std::make_shared<F>(args...);
                // ... hmm. do I "make live" here, or have a distinct Open call to insert it into the Application's list?
                return f;
            }
        };
        
        class Form : public std::enable_shared_from_this<Form> {
              ////////////////////
             // STATIC MEMBERS //
            ////////////////////
        private:
            static unsigned int nextShowCounter; // you're not going to end up showing forms 4.3 billion times in a single session
            
        public:
            static bool OrderedCompare(std::shared_ptr<Form>& f1, std::shared_ptr<Form>& f2);
            
              //////////////////////
             // INSTANCE MEMBERS //
            //////////////////////
        private:
            unsigned int showCounter = 0;
            
        protected:
            //
            
        public:
            float priority = 0;
            unsigned int flags = 0;
            
            std::shared_ptr<UIContainer> touchScreen = nullptr;
            std::shared_ptr<UIContainer> topScreen = nullptr;
            
            Form() { }
            Form(bool useDefaults);
            virtual ~Form() { }
            
            void Open(bool showImmediately = true);
            void Close();
            void Show();
            void Hide();
            
            //virtual void Init() { }
            virtual void Update(bool focused) { }
            //virtual void PostUpdate(bool focused) { }
            //virtual void Draw() { }
            //virtual void PostDraw() { }
            //virtual void End() { }
            virtual void OnShow() { }
            virtual void OnHide() { }
            
            // convenience
            inline bool GetFlag(FormFlags f) { return flags & static_cast<unsigned int>(f); }
            inline void SetFlag(FormFlags f, bool b) {
                auto ff = static_cast<unsigned int>(f);
                flags &= ~ff; if (b) flags |= ff;
            }
            
            inline bool IsVisible() { return GetFlag(FormFlags::visible); }
            
        };
    }
}
