#include "InputHandler.h"

#include "starlight/ui/Form.h"

using std::string;

using starlight::dialog::osk::InputHandler;
using starlight::dialog::osk::InputHandlerDirectEdit;
using starlight::dialog::osk::InputHandlerBuffered;

// DirectEdit

std::string& InputHandlerDirectEdit::GetPreviewText() { return *pText; }

unsigned int InputHandlerDirectEdit::GetCursor() { return pText->length(); }
void InputHandlerDirectEdit::SetCursor(unsigned int index) { }

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

std::string& InputHandlerBuffered::GetPreviewText() { return buffer; }

unsigned int InputHandlerBuffered::GetCursor() { return buffer.length(); }
void InputHandlerBuffered::SetCursor(unsigned int index) { }

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
