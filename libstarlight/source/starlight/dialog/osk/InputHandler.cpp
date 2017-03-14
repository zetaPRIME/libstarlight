#include "InputHandler.h"

#include "starlight/ui/Form.h"

using std::string;

using starlight::dialog::osk::InputHandler;
using starlight::dialog::osk::InputHandlerDirectEdit;
using starlight::dialog::osk::InputHandlerBuffered;

// DirectEdit

std::string& InputHandlerDirectEdit::GetPreviewText() { return *pText; }

unsigned int InputHandlerDirectEdit::GetCursor() { return cursor; }
void InputHandlerDirectEdit::SetCursor(unsigned int index) { cursor = index; if (cursor < minIndex) cursor = minIndex; auto len = pText->length(); if (cursor > len) cursor = len; }

void InputHandlerDirectEdit::InputSymbol(const string& sym) {
    //pText->append(sym);
    pText->insert(cursor, sym);
    cursor += sym.length();
    if (eOnModify) eOnModify();
}

void InputHandlerDirectEdit::Backspace() {
    if (cursor > minIndex) {
        //pText->pop_back();
        pText->erase(cursor-1, 1);
        cursor -= 1;
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
