#pragma once
#include "starlight/_global.h"

#include <string>
#include <functional>
#include <memory>

#include "starlight/ui/Form.h"

#include "starlight/dialog/osk/InputHandler.h"

namespace starlight {
    namespace dialog {
        class OSK : public ui::Form, public ui::FormCreator<OSK> {
        private:
            std::shared_ptr<ui::UIContainer> setContainer;
        
        public:
            std::unique_ptr<osk::InputHandler> handler;
            
            OSK(osk::InputHandler* handler);
            ~OSK() override { };
            
            void Update(bool focused) override;
            
            void OnSymKey(const std::string& chr);
        };
    }
}
