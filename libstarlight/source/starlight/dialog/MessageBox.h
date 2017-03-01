#pragma once
#include "starlight/_global.h"

#include <string>
#include <functional>

#include "starlight/ui/Form.h"

namespace starlight {
    namespace dialog {
        class MessageBox : public ui::Form, public ui::FormCreator<MessageBox> {
        private:
            //
        
        public:
            enum Mode {
                Ok, OkCancel, YesNo
            };
            
            std::function<void(int)> eOnSelect;
            int numButtons = 0;
            
            MessageBox(Mode m, const std::string& msg, std::function<void(int)> onSelect = {});
            ~MessageBox() override { };
            
            void Update(bool focused) override;
            
            void OnSelect(int buttonId);
        };
    }
}
