#include "GFXManager.h"

using starlight::Vector2;
using starlight::GFXManager;
using starlight::gfx::DrawContext;

std::forward_list<DrawContext*> GFXManager::ctxStack;
std::forward_list<Vector2> GFXManager::offsetStack;

float GFXManager::parallax = 0;

void GFXManager::PushContext(DrawContext* context) {
    if (!ctxStack.empty()) ctxStack.front()->Close();
    ctxStack.push_front(context);
    context->Open();
    PushOffset(Vector2::zero);
}

DrawContext* GFXManager::PopContext() {
    //if (ctxStack.empty()) return nullptr;
    DrawContext* context = ctxStack.front();
    PopOffset();
    context->Close();
    ctxStack.pop_front();
    if (!ctxStack.empty()) ctxStack.front()->Open();
    return context;
}

DrawContext* GFXManager::GetContext() { return !ctxStack.empty() ? ctxStack.front() : nullptr; }

void GFXManager::PushOffset(Vector2 offset) { offsetStack.push_front(offset); }
void GFXManager::PushOffsetAdd(Vector2 add) { offsetStack.push_front(GetOffset() + add); }
Vector2 GFXManager::PopOffset() {
    Vector2 r = GetOffset();
    offsetStack.pop_front();
    return r;
}
Vector2 GFXManager::GetOffset() { return !offsetStack.empty() ? offsetStack.front() : Vector2::zero; }

void GFXManager::Reset() {
    ctxStack.clear();
    offsetStack.clear();
}

bool GFXManager::PrepareForDrawing() {
    if (ctxStack.empty()) return false;
    auto context = ctxStack.front();
    if (context->drawReady) return true;
    return context->Prepare();
}

