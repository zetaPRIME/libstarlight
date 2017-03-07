#include "InputHandler.h"

#include "starlight/ui/Form.h"

using std::string;

using starlight::dialog::osk::InputHandler;
using starlight::dialog::osk::InputHandlerDirectEdit;
using starlight::dialog::osk::InputHandlerBuffered;

// DirectEdit

void InputHandlerDirectEdit::InputSymbol(const string& sym) {
    pText->append(sym);
    if (eOnModify) eOnModify();
}

void InputHandlerDirectEdit::Backspace() {
    if (pText->length() > minIndex) {
        pText->pop_back();
        if (eOnModify) eOnModify();
    }
}

void InputHandlerDirectEdit::Enter() {
    if (multiLine) InputSymbol("\n");
    else Done();
}

void InputHandlerDirectEdit::Done() {
    if (eOnFinalize) eOnFinalize();
    parent->Close();
}

// Buffered

void InputHandlerBuffered::InputSymbol(const string& sym) {
    buffer.append(sym);
}

void InputHandlerBuffered::Backspace() {
    buffer.pop_back();
}

void InputHandlerBuffered::Enter() {
    if (multiLine) InputSymbol("\n");
    else Done();
}

void InputHandlerBuffered::Done() {
    if (eOnFinalize) eOnFinalize(buffer);
    parent->Close();
}
