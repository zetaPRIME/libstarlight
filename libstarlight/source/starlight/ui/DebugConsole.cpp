#include "DebugConsole.h"

#include "starlight/GFXManager.h"
#include "starlight/gfx/RenderCore.h"

#include "sys/iosupport.h"

using starlight::GFXManager;
using starlight::TextConfig;

using starlight::gfx::RenderCore;

using starlight::ui::DebugConsole;

namespace {
    bool csInit = false;
    //std::weak_ptr<DebugConsole> curDC = std::shared_ptr<DebugConsole>(nullptr);
    DebugConsole* cs = nullptr;
    
    ssize_t consoleWrite(struct _reent* r, void* fd, const char* ptr, size_t len) {
        if (!ptr) return -1;
        
        //if (curDC.expired()) return -1;
        
        //auto cs = curDC.lock();
        if (cs == nullptr) return -1; // nullref but not expired???
        cs->text.append(ptr, len);
        cs->dirty = true;
        
        return len;
    }
    
    const devoptab_t devoptab_console = {
        "con",
        0,
        NULL,
        NULL,
        consoleWrite,
        NULL,
        NULL,
        NULL
    };
}

DebugConsole::DebugConsole(VRect rect) {
    this->rect = rect;
}

DebugConsole::~DebugConsole() {
    if (cs == this) cs = nullptr;
}

void DebugConsole::Start() {
    //curDC = std::static_pointer_cast<DebugConsole>(shared_from_this());
    cs = this;
    if (!csInit) {
        csInit = true;
        
        devoptab_list[STD_OUT] = &devoptab_console;
        devoptab_list[STD_ERR] = &devoptab_console;
        
        setvbuf(stdout, NULL , _IONBF, 0);
        setvbuf(stderr, NULL , _IONBF, 0);
        
    }
}

void DebugConsole::PreDrawOffscreen() {
    buffer.reset(); // I guess?
}

void DebugConsole::PreDraw() {
    if (dirty || !buffer) {
        dirty = false;
        
        static TextConfig textConfig = ThemeManager::GetMetric<TextConfig>("/textPresets/normal.12", TextConfig());
        textConfig.font = ThemeManager::GetFont("mono.12");
        textConfig.justification = Vector2(0, 1);
        textConfig.borderColor = Color::black;
        
        // clip text at top left corner
        Vector2 measure = textConfig.Measure(text, rect.size.x);
        if (measure.y > rect.size.y) {
            unsigned int cfp = textConfig.GetCursorFromPoint(rect, text, Vector2(0, measure.y - (rect.size.y + 16)));
            text = text.substr(cfp);
        }
        
        if (!buffer) buffer = std::make_unique<gfx::DrawContextCanvas>(rect.size + Vector2(0, 8));
        buffer->Clear();
        GFXManager::PushContext(buffer.get());
        GFXManager::PrepareForDrawing(); // force clear even if nothing to write
        textConfig.Print(buffer->rect, text);
        GFXManager::PopContext();
    }
}

void DebugConsole::Draw() {
    auto rect = (this->rect + GFXManager::GetOffset()).IntSnap();
    if (buffer) {
        buffer->Draw(VRect(rect.pos, buffer->rect.size));
    }
}
