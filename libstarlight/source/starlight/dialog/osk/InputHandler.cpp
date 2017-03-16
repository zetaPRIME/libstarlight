#include "InputHandler.h"

#include <cmath>

#include "starlight/ui/Form.h"

using std::string;

using starlight::dialog::osk::InputHandler;
using starlight::dialog::osk::InputHandlerDirectEdit;
using starlight::dialog::osk::InputHandlerBuffered;

// DirectEdit

std::string& InputHandlerDirectEdit::GetPreviewText() { return *pText; }

unsigned int InputHandlerDirectEdit::GetCursor() { return cursor; }
void InputHandlerDirectEdit::SetCursor(unsigned int index) { cursor = std::max(minIndex, std::min(index, pText->length())); }

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
    done = true;
}

// Buffered

std::string& InputHandlerBuffered::GetPreviewText() { return buffer; }

unsigned int InputHandlerBuffered::GetCursor() { return cursor; }
void InputHandlerBuffered::SetCursor(unsigned int index) { cursor = std::max(0U, std::min(index, buffer.length())); }

void InputHandlerBuffered::InputSymbol(const string& sym) {
    buffer.insert(cursor, sym);
    cursor += sym.length();
}

void InputHandlerBuffered::Backspace() {
    if (cursor > 0) buffer.erase(--cursor, 1);
}

void InputHandlerBuffered::Enter() {
    if (multiLine) InputSymbol("\n");
    else Done();
}

void InputHandlerBuffered::Done() {
    if (eOnFinalize) eOnFinalize(buffer);
    done = true;
}
