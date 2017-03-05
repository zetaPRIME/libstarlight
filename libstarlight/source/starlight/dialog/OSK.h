#pragma once
#include "starlight/_global.h"

#include <string>
#include <functional>

#include "starlight/ui/Form.h"

namespace starlight {
    namespace dialog {
        class OSK : public ui::Form, public ui::FormCreator<OSK> {
        private:
            std::shared_ptr<ui::UIContainer> setContainer;
        
        public:
            std::string* pText = nullptr;
            
            std::function<void()> eOnModify;
            
            OSK(std::string* textptr, std::function<void()> onModify = {});
            ~OSK() override { };
            
            void Update(bool focused) override;
            
            void OnSymKey(const std::string& chr);
        };
    }
}
