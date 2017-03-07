#pragma once
#include "starlight/_global.h"

#include <string>
#include <functional>

namespace starlight {
    // forward declare...
    namespace ui {
        class Form;
    }
    
    namespace dialog {
        namespace osk {
            
            class InputHandler {
            public:
                bool showPreview = false;
                ui::Form* parent;
                
                InputHandler() = default;
                virtual ~InputHandler() = default;
                
                virtual void InputSymbol(const std::string& sym) { }
                virtual void Backspace() { }
                virtual void Enter() { }
                
                virtual void Done() { }
            };
            
            class InputHandlerDirectEdit : public InputHandler {
            public:
                bool multiLine = false;
                
                std::string* pText;
                unsigned int minIndex = 0;
                
                std::function<void()> eOnModify = {};
                std::function<void()> eOnFinalize = {};
                
                InputHandlerDirectEdit(std::string* textptr, bool multiLine = false, unsigned int minIndex = 0, std::function<void()> onModify = {}, std::function<void()> onFinalize = {})
                    : multiLine(multiLine), pText(textptr), minIndex(minIndex), eOnModify(onModify), eOnFinalize(onFinalize) { }
                ~InputHandlerDirectEdit() override { }
                
                void InputSymbol(const std::string& sym) override;
                void Backspace() override;
                void Enter() override;
                
                void Done() override;
            };
            
            class InputHandlerBuffered : public InputHandler {
                std::string buffer = "";
            public:
                bool multiLine = false;
                
                std::function<void(const std::string&)> eOnFinalize = { };
                
                InputHandlerBuffered(const std::string& text = "", bool multiLine = false, std::function<void(const std::string&)> onFinalize = {})
                    : showPreview(true), buffer(text), multiLine(multiLine), eOnFinalize(onFinalize) { }
                ~InputHandlerBuffered() override { }
                
                void InputSymbol(const std::string& sym) override;
                void Backspace() override;
                void Enter() override;
                
                void Done() override;
            };
            
        }
    }
}
