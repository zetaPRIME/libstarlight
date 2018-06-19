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
using starlight::gfx::BlendMode;
using starlight::gfx::CTexture;
using starlight::gfx::CRenderTarget;
using starlight::gfx::RenderCore;

namespace { // internals
    typedef struct {
        float x, y, z, u, v;
    } vbo_xyzuv;
    
    vbo_xyzuv* vboArray = nullptr;
    size_t vboIndex = 0;
    size_t vboSize = 0;
    
    void addVertXYZUV(float x, float y, float z, float u, float v) {
        // TODO: maybe add bounds check
        vbo_xyzuv* vert = &vboArray[vboIndex++];
        vert->x = x; vert->y = y; vert->z = z;
        vert->u = u; vert->v = v;
    }
    void addVertXYZUV(Vector2 xy, Vector2 uv) { addVertXYZUV(xy.x, xy.y, 0, uv.x, uv.y); }
    
    DVLB_s* dvlb = nullptr;
    shaderProgram_s shader;
    int sLocProjection = -1;
    
    C3D_AttrInfo* attrInfo = nullptr;
    
    inline unsigned int NextPow2(unsigned int x) {
        --x;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return std::min(std::max(++x, 64U), 1024U); // clamp size to keep gpu from locking
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
        void Bind(Color color = Color::white, BlendMode mode = BlendMode::Normal) override {
            RenderCore::BindTexture(texture, color, mode);
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
    
    // allocate and initialize VBO
    vboSize = 0x80000;
    void* vboAddr = linearAlloc(vboSize);
    vboArray = reinterpret_cast<vbo_xyzuv*>(vboAddr);
    vboIndex = 0;
    
    C3D_BufInfo* bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, vboAddr, sizeof(vbo_xyzuv), 2, 0x10);
    
    // set up shader attribute passing
    attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3);
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2);
    
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
    C3D_DepthTest(false, GPU_GEQUAL, GPU_WRITE_ALL); // hmm.
    C3D_CullFace(GPU_CULL_NONE);
    
}

void RenderCore::Close() {
    targetTopLeft.reset(nullptr);
    targetTopRight.reset(nullptr);
    targetBottom.reset(nullptr);
    
    linearFree(reinterpret_cast<void*>(vboArray));
    
    C3D_Fini();
    gfxExit();
}

void RenderCore::SyncFrame() {
    C3D_FrameSync();
}

void RenderCore::BeginFrame() {
    C3D_FrameBegin(0/*C3D_FRAME_SYNCDRAW*/);
    
    vboIndex = 0;
}

void RenderCore::EndFrame() {
    C3D_FrameEnd(0);
}

namespace {
    void ApplyBlendMode(C3D_TexEnv* env, BlendMode mode) {
        switch(mode) {
            case BlendMode::Mask: // multiplies the buffer contents by the mask texture
                C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ZERO, GPU_SRC_COLOR, GPU_ZERO, GPU_SRC_ALPHA); // zero + (buffer * texel)
                C3D_TexEnvOp(env, C3D_RGB, 0, 0, 0); // and the rest is the same as blend
                C3D_TexEnvOp(env, C3D_Alpha, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA, 0);
                C3D_TexEnvFunc(env, C3D_RGB, GPU_MODULATE);
                C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
                break;
            
            case BlendMode::Replace:
                C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ZERO, GPU_ONE, GPU_ZERO); // flat replace
                C3D_TexEnvOp(env, C3D_RGB, 0, 0, 0);
                C3D_TexEnvOp(env, C3D_Alpha, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA, 0);
                C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);
                C3D_TexEnvFunc(env, C3D_Alpha, GPU_REPLACE);
                break;
            
            default:
            case BlendMode::Blend:
                C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA, GPU_ONE, GPU_ONE_MINUS_SRC_ALPHA); // premult
                C3D_TexEnvOp(env, C3D_RGB, 0, 0, 0);
                C3D_TexEnvOp(env, C3D_Alpha, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA, 0); // for color, the second op was 0... but that's the same value so whatever
                C3D_TexEnvFunc(env, C3D_RGB, GPU_MODULATE); // and for color, this was REPLACE, not sure if that actually matters
                C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
                break;
        }
    }
}

void RenderCore::BindTexture(C3D_Tex* tex, const Color& color, BlendMode mode) {
    C3D_TexBind(0, tex); // 0 should be correct
    
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_CONSTANT, GPU_PRIMARY_COLOR);
    ApplyBlendMode(env, mode);
    C3D_TexEnvColor(env, color.Premultiplied());
}

void RenderCore::BindColor(const Color& color, BlendMode mode) {
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvSrc(env, C3D_Both, GPU_CONSTANT, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
    ApplyBlendMode(env, mode);
    C3D_TexEnvColor(env, color.Premultiplied());
}

void RenderCore::DrawQuad(const VRect& rect, const VRect& src, bool noSnap) {
    size_t vboNum = vboIndex;
    
    VRect r = noSnap ? rect : rect.IntSnap(); // screen-space snap
    
    /*setXYZUV(verts[0], r.TopLeft(), src.TopLeft());
    setXYZUV(verts[1], r.TopRight(), src.TopRight());
    setXYZUV(verts[2], r.BottomLeft(), src.BottomLeft());
    setXYZUV(verts[3], r.BottomRight(), src.BottomRight());*/
    
    // let's make this recalculate things a bit less
    float rl = r.pos.x, rr = rl + r.size.x, rt = r.pos.y, rb = rt + r.size.y;
    float srl = src.pos.x, srr = srl + src.size.x, srt = src.pos.y, srb = srt + src.size.y;
    addVertXYZUV(rl, rt, 0, srl, srt);
    addVertXYZUV(rr, rt, 0, srr, srt);
    addVertXYZUV(rl, rb, 0, srl, srb);
    addVertXYZUV(rr, rb, 0, srr, srb);
    
    C3D_DrawArrays(GPU_TRIANGLE_STRIP, vboNum, 4);
}

void RenderCore::DrawQuad(const VRect& rect, const Vector2& anchor, float angle, const VRect& src) {
    size_t vboNum = vboIndex;
    
    addVertXYZUV(rect.TopLeft().RotateAround(anchor, angle), src.TopLeft());
    addVertXYZUV(rect.TopRight().RotateAround(anchor, angle), src.TopRight());
    addVertXYZUV(rect.BottomLeft().RotateAround(anchor, angle), src.BottomLeft());
    addVertXYZUV(rect.BottomRight().RotateAround(anchor, angle), src.BottomRight());
    
    C3D_DrawArrays(GPU_TRIANGLE_STRIP, vboNum, 4);
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
    
    //printf("loaded image w %i (%i) h %i (%i)\n", width, owidth, height, oheight);
    
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
    
    tgt = C3D_RenderTargetCreate(w, h, GPU_RB_RGBA8, -1/*GPU_RB_DEPTH24_STENCIL8*/); // I don't think we need a depth buffer >.>
    //tgt = C3D_RenderTargetCreateFromTex(&tex, GPU_TEXFACE_2D, -1, -1); // create target from texture, actually
    
    C3D_TexInit(&tex, w, h, GPU_RGBA8);
    //memset(tex.data, 0, w*h*4); // manually zero out; TODO: optimize this
    C3D_TexDelete(&tex); tex.data = tgt->frameBuf.colorBuf; // replace stuff...
    
    Mtx_Ortho(&projection, 0.0f, w, 0.0f, h, 0.0f, 1.0f, true);
    //Mtx_OrthoTilt(&projection, 0.0f, h, 0.0f, w, 0.0f, 1.0f, true);
    
    //C3D_FrameBufClear(&(tgt->frameBuf), C3D_CLEAR_ALL, 0, 0);
    //Clear(Color::transparent);
    //RenderCore::BindTexture(&tex, Color::white);
    //C3D_FrameBufClear(&(tgt->frameBuf), C3D_CLEAR_COLOR, 0, 0);
    C3D_RenderTargetSetClear(tgt, static_cast<C3D_ClearBits>(0), 0, 0);
}

CRenderTarget::~CRenderTarget() {
    C3D_RenderTargetDelete(tgt);
}

void CRenderTarget::Clear(Color color) {
    //unsigned int c = color;
    //c = ((c>>24)&0x000000FF) | ((c>>8)&0x0000FF00) | ((c<<8)&0x00FF0000) | ((c<<24)&0xFF000000); // reverse endianness
    //C3D_RenderTargetSetClear(tgt, C3D_CLEAR_ALL, c, 0);
    //C3D_FrameBufClear(&(tgt->frameBuf), C3D_CLEAR_COLOR, c, 0);
    clearColor = color;
}

void CRenderTarget::BindTarget() {
    if (clearColor.Valid()) { // clear if color valid
        unsigned int c = clearColor;
        c = ((c>>24)&0x000000FF) | ((c>>8)&0x0000FF00) | ((c<<8)&0x00FF0000) | ((c<<24)&0xFF000000); // reverse endianness
        //C3D_RenderTargetSetClear(tgt, static_cast<C3D_ClearBits>(0), c, 0);
        //C3D_RenderTargetSetClear(tgt, C3D_CLEAR_ALL, c, 0);
        C3D_FrameBufClear(&(tgt->frameBuf), C3D_CLEAR_COLOR, c, 0);
    }
    C3D_FrameDrawOn(tgt);
    C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, sLocProjection, &projection);
    
    if (!firstClearDone) { firstClearDone = true; // workaround for faulty clearing; just draw a quad in replace mode to force the matter!
        if (clearColor.Valid()) {
            RenderCore::BindColor(clearColor, BlendMode::Replace);
            RenderCore::DrawQuad(VRect(Vector2::zero, txSize), VRect::zero, false);
        }
    }
}

void CRenderTarget::Bind(Color color, BlendMode mode) {
    //C3D_RenderTargetSetClear(tgt, static_cast<C3D_ClearBits>(0), 0, 0); // don't clear again until marked to
    RenderCore::BindTexture(&tex, color, mode);
}
