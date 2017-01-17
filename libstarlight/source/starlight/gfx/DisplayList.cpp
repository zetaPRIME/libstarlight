#include "DisplayList.h"

#include "starlight/gfx/RenderCore.h"

#include "starlight/GFXManager.h"

using starlight::gfx::RenderCore;

using starlight::gfx::DLNodeType;

using starlight::gfx::DLNode;
using starlight::gfx::DLNode_Quads;
using starlight::gfx::DLNode_Func;
using starlight::gfx::DisplayList;

void DisplayList::Run(Vector2 offset) {
    if (GFXManager::PrepareForDrawing()) {
        offset = (offset + this->offset).IntSnap();
        for (auto n : nodes) n->Apply(offset);
    }
}

bool DisplayList::Valid() {
    return !nodes.empty();
}

void DLNode_Quads::Apply(Vector2 offset) {
    for (auto r : rects) RenderCore::DrawQuad(r.first + offset, r.second, true);
}
void DisplayList::AddQuad(const VRect& quad, const VRect& uv) {
    auto node = (!nodes.empty() && nodes.back()->Type() == DLNodeType::Quads) ? std::static_pointer_cast<DLNode_Quads>(nodes.back()) : CreateNode<DLNode_Quads>();
    node->Add(quad, uv);
}

void DLNode_Func::Apply(Vector2 offset) { func(offset); }
void DisplayList::AddFunc(std::function<void(Vector2)> func) { CreateNode<DLNode_Func>(func); }

