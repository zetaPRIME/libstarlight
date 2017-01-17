#include <cstdio>
#include <cmath>

#include <3ds.h>
#include <citro3d.h>

#include "starlight/datatypes/Vector2.h"
#include "starlight/datatypes/VRect.h"
#include "starlight/datatypes/Color.h"

#include "basic_shader_shbin.h"

#include "RenderCore.h"

using starlight::Vector2;
using starlight::VRect;
using starlight::Color;
using starlight::util::WorkerThread;
using starlight::gfx::CTexture;
using starlight::gfx::CRenderTarget;
using starlight::gfx::RenderCore;

namespace { // internals
    typedef struct {
        float x, y, z, u, v;
    } vbo_xyzuv;
    void setXYZUV(vbo_xyzuv& vbo, float x, float y, float z, float u, float v) {
        vbo.x = x;
        vbo.y = y;
        vbo.z = z;
        vbo.u = u;
        vbo.v = v;
    }
    void setXYZUV(vbo_xyzuv& vbo, Vector2 xy, Vector2 uv) { setXYZUV(vbo, xy.x, xy.y, 0, uv.x, uv.y); }
    
    void* bufferStart = nullptr;
    size_t bufferInd = 0;
    size_t bufferSize = 0;
    
    DVLB_s* dvlb = nullptr;
    shaderProgram_s shader;
    int sLocProjection = -1;
    
    void ResetBuffer() { bufferInd = 0; }
    
    void* AllocBuffer(size_t size, size_t align = 1) {
        bufferInd += align - (bufferInd % align); // prealign
        void* b = reinterpret_cast<void*>(reinterpret_cast<size_t>(bufferStart) + bufferInd);
        bufferInd += size;
        if (bufferInd > bufferSize) return nullptr;
        return b;
    }
    
    inline int NextPow2(unsigned int x) {
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return ++x >= 64 ? x : 64; // min size to keep gpu from locking
    }
    
    class CRawTexture : public CTexture {
        friend class starlight::gfx::RenderCore;
    protected:
        C3D_Tex* texture;
    public:
        CRawTexture(int width, int height) {
            size = Vector2(width, height);
            auto w = NextPow2(width), h = NextPow2(height);
            txSize = Vector2(w, h);
            texture = new C3D_Tex();
            C3D_TexInit(texture, w, h, GPU_RGBA8);
        }
        ~CRawTexture() override {
            C3D_TexDelete(texture);
            delete texture;
        }
        void Bind(Color color = Color::white) override {
            RenderCore::BindTexture(texture, color);
        }
    };
}

//WorkerThread RenderCore::loadingThread;

std::unique_ptr<CRenderTarget> RenderCore::targetTopLeft = nullptr;
std::unique_ptr<CRenderTarget> RenderCore::targetTopRight = nullptr;
std::unique_ptr<CRenderTarget> RenderCore::targetBottom = nullptr;

void RenderCore::Open() {
    gfxInitDefault();
    gfxSet3D(true);
    C3D_Init(0x80000*8);
    
    bufferSize = 0x80000;
    bufferStart = linearAlloc(bufferSize);
    bufferInd = 0;
    
    // set up screen targets
    targetTopLeft = std::make_unique<CRenderTarget>(240, 400, true);
    targetTopRight = std::make_unique<CRenderTarget>(240, 400, true);
    targetBottom = std::make_unique<CRenderTarget>(240, 320, true);
    C3D_RenderTargetSetOutput(targetTopLeft->tgt, GFX_TOP, GFX_LEFT,  0x1000);
    C3D_RenderTargetSetOutput(targetTopRight->tgt, GFX_TOP, GFX_RIGHT, 0x1000);
    C3D_RenderTargetSetOutput(targetBottom->tgt, GFX_BOTTOM, GFX_LEFT,  0x1000);
    Mtx_OrthoTilt(&targetTopLeft->projection, 0.0f, 400, 240, 0.0f, 0.0f, 1.0f, true);
    Mtx_OrthoTilt(&targetTopRight->projection, 0.0f, 400, 240, 0.0f, 0.0f, 1.0f, true);
    Mtx_OrthoTilt(&targetBottom->projection, 0.0f, 320, 240, 0.0f, 0.0f, 1.0f, true);
    
    // shader and company
    dvlb = DVLB_ParseFile((u32*)basic_shader_shbin, basic_shader_shbin_size);
    shaderProgramInit(&shader);
    shaderProgramSetVsh(&shader, &dvlb->DVLE[0]);
    C3D_BindProgram(&shader);
    
    sLocProjection = shaderInstanceGetUniformLocation(shader.vertexShader, "projection");
    
    // set up mode defaults
    C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA); // premult
    C3D_DepthTest(true, GPU_GEQUAL, GPU_WRITE_ALL); // hmm.
    C3D_CullFace(GPU_CULL_NONE);
    
}

void RenderCore::Close() {
    targetTopLeft.reset(nullptr);
    targetTopRight.reset(nullptr);
    targetBottom.reset(nullptr);
    
    linearFree(bufferStart);
    
    C3D_Fini();
    gfxExit();
}

void RenderCore::BeginFrame() {
    ResetBuffer();
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
}

void RenderCore::EndFrame() {
    C3D_FrameEnd(0);
}

void RenderCore::BindTexture(C3D_Tex* tex, const Color& color) {
    C3D_TexBind(0, tex); // 0 should be correct
    
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_CONSTANT, 0);
    C3D_TexEnvOp(env, C3D_RGB, 0, 0, 0);
    C3D_TexEnvOp(env, C3D_Alpha, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA, 0);
    C3D_TexEnvFunc(env, C3D_RGB, GPU_MODULATE);//REPLACE); // let's see...
    C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
    C3D_TexEnvColor(env, color.Premultiplied());
}

void RenderCore::BindColor(const Color& color) {
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvSrc(env, C3D_Both, GPU_CONSTANT, 0, 0);
    C3D_TexEnvOp(env, C3D_RGB, 0, 0, 0);
    C3D_TexEnvOp(env, C3D_Alpha, GPU_TEVOP_A_SRC_ALPHA, 0, 0);
    C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);//REPLACE); // let's see...
    C3D_TexEnvFunc(env, C3D_Alpha, GPU_REPLACE);
    C3D_TexEnvColor(env, color.Premultiplied());
}

void RenderCore::DrawQuad(const VRect& rect, const VRect& src, bool noSnap) {
    vbo_xyzuv* verts = static_cast<vbo_xyzuv*>(AllocBuffer(4 * sizeof(vbo_xyzuv), 8));
    
    VRect r = noSnap ? rect : rect.IntSnap(); // screen-space snap
    
    /*setXYZUV(verts[0], r.TopLeft(), src.TopLeft());
    setXYZUV(verts[1], r.TopRight(), src.TopRight());
    setXYZUV(verts[2], r.BottomLeft(), src.BottomLeft());
    setXYZUV(verts[3], r.BottomRight(), src.BottomRight());*/
    
    // let's make this recalculate things a bit less
    float rl = r.pos.x, rr = rl + r.size.x, rt = r.pos.y, rb = rt + r.size.y;
    float srl = src.pos.x, srr = srl + src.size.x, srt = src.pos.y, srb = srt + src.size.y;
    setXYZUV(verts[0], rl, rt, 0, srl, srt);
    setXYZUV(verts[1], rr, rt, 0, srr, srt);
    setXYZUV(verts[2], rl, rb, 0, srl, srb);
    setXYZUV(verts[3], rr, rb, 0, srr, srb);
    
    C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3);
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2);

    C3D_BufInfo* bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, verts, sizeof(vbo_xyzuv), 2, 0x10);

    C3D_DrawArrays(GPU_TRIANGLE_STRIP, 0, 4);
}

void RenderCore::DrawQuad(const VRect& rect, const Vector2& anchor, float angle, const VRect& src) {
    vbo_xyzuv* verts = static_cast<vbo_xyzuv*>(AllocBuffer(4 * sizeof(vbo_xyzuv), 8));
    
    setXYZUV(verts[0], rect.TopLeft().RotateAround(anchor, angle), src.TopLeft());
    setXYZUV(verts[1], rect.TopRight().RotateAround(anchor, angle), src.TopRight());
    setXYZUV(verts[2], rect.BottomLeft().RotateAround(anchor, angle), src.BottomLeft());
    setXYZUV(verts[3], rect.BottomRight().RotateAround(anchor, angle), src.BottomRight());
    
    C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3);
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2);

    C3D_BufInfo* bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, verts, sizeof(vbo_xyzuv), 2, 0x10);

    C3D_DrawArrays(GPU_TRIANGLE_STRIP, 0, 4);
}

// specifically RGBA
CTexture* RenderCore::LoadTexture(void* src, int width, int height) {
    // flush to FCRAM
    GSPGPU_FlushDataCache(src, width*height*4);
    // assemble texture
    CRawTexture* tex = new CRawTexture(width, height);
    int owidth = tex->txSize.x, oheight = tex->txSize.y;
    constexpr u32 flags = (GX_TRANSFER_FLIP_VERT(1) | GX_TRANSFER_OUT_TILED(1) | GX_TRANSFER_RAW_COPY(0) |  GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | \
        GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGBA8) |  GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO));
    //C3D_SafeDisplayTransfer(static_cast<u32*>(src), GX_BUFFER_DIM(width, height), static_cast<u32*>(tex->texture->data), GX_BUFFER_DIM(owidth, oheight), flags);
    C3D_SafeDisplayTransfer(static_cast<u32*>(src), GX_BUFFER_DIM(width, height), static_cast<u32*>(tex->texture->data), GX_BUFFER_DIM(owidth, oheight), flags);
    gspWaitForPPF();
    //C3D_TexSetFilter(tex->texture, GPU_LINEAR, GPU_NEAREST);
    C3D_TexSetFilter(tex->texture, GPU_LINEAR, GPU_LINEAR); // nearest causes artifacts on ninepatches
    C3D_TexSetWrap(tex->texture, GPU_CLAMP_TO_BORDER, GPU_CLAMP_TO_BORDER);
    
    C3D_TexBind(0, tex->texture);
    
    printf("loaded image w %i (%i) h %i (%i)\n", width, owidth, height, oheight);
    
    return tex;
}

///////////////////
// CRenderTarget //
///////////////////

CRenderTarget::CRenderTarget(int width, int height, bool forceExact) {
    size = Vector2(width, height);
    auto w = forceExact ? width : NextPow2(width),
         h = forceExact ? height : NextPow2(height);
    txSize = Vector2(w, h);
    tgt = C3D_RenderTargetCreate(w, h, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8); // though actually, do we really need stenciling? could drop to 16 if we don't
    Mtx_Ortho(&projection, 0.0f, w, 0.0f, h, 0.0f, 1.0f, true);
    //Mtx_OrthoTilt(&projection, 0.0f, h, 0.0f, w, 0.0f, 1.0f, true);
}

CRenderTarget::~CRenderTarget() {
    C3D_RenderTargetDelete(tgt);
}

void CRenderTarget::Clear(Color color) {
    unsigned int c = color;
    c = ((c>>24)&0x000000FF) | ((c>>8)&0x0000FF00) | ((c<<8)&0x00FF0000) | ((c<<24)&0xFF000000); // reverse endianness
    C3D_RenderTargetSetClear(tgt, C3D_CLEAR_ALL, c, 0);
}

void CRenderTarget::BindTarget() {
    C3D_FrameDrawOn(tgt);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, sLocProjection, &projection);
}

void CRenderTarget::Bind(Color color) {
    C3D_RenderTargetSetClear(tgt, 0, 0, 0); // don't clear again until marked to
    RenderCore::BindTexture(&(tgt->renderBuf.colorBuf), color);
}
