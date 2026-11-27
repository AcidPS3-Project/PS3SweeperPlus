#include <ppu-lv2.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include "importer.h"
#include "rsx_renderer.h"

#include <pngdec/pngdec.h>
#include <sysmodule/sysmodule.h>

static RSX_Texture textures[RSX_MAX_TEXTURES];

static int sorted_ok = 1;

#define RSX_FENCE_LABEL 250
static u32 fence_value = 0;

typedef struct {
    float x, y, z, w;
    float r, g, b, a;
    float u, v;
} RSX_Vertex;

RSX_Vertex quad_vertices[] =
{
    // First triangle
    { -0.8f, -0.8f, 0.0f, 1.0f,  1, 1, 1, 1,  0.0f, 1.0f },
    {  0.8f, -0.8f, 0.0f, 1.0f,  1, 1, 1, 1,  1.0f, 1.0f },
    {  0.8f,  0.8f, 0.0f, 1.0f,  1, 1, 1, 1,  1.0f, 0.0f },

    // Second triangle
    { -0.8f, -0.8f, 0.0f, 1.0f,  1, 1, 1, 1,  0.0f, 1.0f },
    {  0.8f,  0.8f, 0.0f, 1.0f,  1, 1, 1, 1,  1.0f, 0.0f },
    { -0.8f,  0.8f, 0.0f, 1.0f,  1, 1, 1, 1,  0.0f, 0.0f }
};

#define MAX_SPRITES_PER_FRAME 16384

static RSX_Vertex *sprite_vb;
static u32 sprite_vb_offset;
static int frame_idx = 0, sprite_n = 0;

static float logical_width = 916.0f;
static float logical_height = 582.0f;

#define CB_SIZE   0x100000

u32 *depth_buffer = NULL;
u32 depth_offset = 0;
u32 depth_pitch = 0;

static videoState display_state;
static videoConfiguration display_config;
static videoResolution display_resolution;

extern const unsigned char basic_vp_vpo[];
extern const unsigned char basic_vp_vpo_size[];

extern const unsigned char basic_fp_fpo[];
extern const unsigned char basic_fp_fpo_size[];

/* adventure_fp — ported from adventure_map.gdshader */
extern const unsigned char adventure_fp_fpo[];
extern const unsigned char adventure_fp_fpo_size[];

static rsxVertexProgram *vertex_program = NULL;
static rsxFragmentProgram *fragment_program = NULL;

static void *vertex_ucode = NULL;
static void *fragment_ucode = NULL;

static u32 fragment_ucode_offset = 0;
static u32 fragment_ucode_size = 0;

/* adventure shader (second fragment program, same vertex program) */
static rsxFragmentProgram *adv_fragment_program    = NULL;
static void               *adv_fragment_ucode      = NULL;
static u32                 adv_fragment_ucode_offset = 0;
static u32                 adv_fragment_ucode_size   = 0;

u16 screen_width;
u16 screen_height;

static gcmContextData *rsx_context = NULL;
static void *host_addr = NULL;

#define RSX_WHITE_TEX   (RSX_MAX_TEXTURES - 1)   /* reserved id for untextured draws */
#define RSX_PI          3.14159265f

typedef struct { float layer; int seq; int tex; int count; RSX_Vertex v[6]; int blend; int shader; } QueuedDraw;
static QueuedDraw draw_q[MAX_SPRITES_PER_FRAME];
static int   draw_order[MAX_SPRITES_PER_FRAME];
static int   draw_n = 0;
static float cur_layer = 0.0f;

static int cur_blend = 0; /* 0 = normal, 1 = invert */
void RSX_SetBlendMode(int mode) { cur_blend = mode; }

static int cur_shader = 0; /* RSX_SHADER_DEFAULT or RSX_SHADER_ADVENTURE */
void RSX_SetShader(int shader_id) { cur_shader = shader_id; }

static int   crt_enabled         = 0;
static int   crt_ready           = 0;
static float crt_scanline_alpha  = 0.35f;
static float crt_vignette_alpha  = 0.55f;

void RSX_SetCRTEnabled(int on)    { crt_enabled = on; }
void RSX_SetScanlineAlpha(float a){ crt_scanline_alpha = a < 0 ? 0 : a > 1 ? 1 : a; }
void RSX_SetVignetteAlpha(float a){ crt_vignette_alpha = a < 0 ? 0 : a > 1 ? 1 : a; }

#define RSX_SCANLINE_TEX  RSX_MAX_TEXTURES - 3
#define RSX_VIGNETTE_TEX  RSX_MAX_TEXTURES - 4

/* --- Game Boy --- */
static int gb_enabled = 0;

static const u32 GB_PALETTE[4] =
{
    0x0f380fff,   /* darkest  – near black  */
    0x306230ff,   /* dark     – dark green  */
    0x8bac0fff,   /* light    – light green */
    0x9bbc0fff,   /* lightest – near white  */
};

void RSX_SetGameBoyEnabled(int on) { gb_enabled = on; }

void RSX_SetLayer(float l) { cur_layer = -l; }

static int cmp_draw(const void *a, const void *b)
{
    const QueuedDraw *x = &draw_q[*(const int*)a], *y = &draw_q[*(const int*)b];
    if (x->layer < y->layer) return -1;
    if (x->layer > y->layer) return  1;
    return x->seq - y->seq;
}

void RSX_SetLogicalResolution(float width, float height)
{
    logical_width = width;
    logical_height = height;
}

static float RSX_ScreenToNDC_X(float x)
{
    return (x / logical_width) * 2.0f - 1.0f;
}

static float RSX_ScreenToNDC_Y(float y)
{
    return 1.0f - (y / logical_height) * 2.0f;
}

int RSX_Init(void)
{
    DebugLog("RSX: allocating shared IO memory...\n");

    host_addr = memalign(1024 * 1024, HOST_SIZE);

    if (!host_addr)
    {
        DebugLog("RSX: IO allocation failed!\n");
        return -1;
    }

    DebugLog("RSX: IO buffer = %p\n", host_addr);
    DebugLog("RSX: alignment = 0x%X\n", (unsigned int)((uintptr_t)host_addr & 0xFFFFF));

    DebugLog("RSX: initializing...\n");

    s32 result = rsxInit(&rsx_context, CB_SIZE, HOST_SIZE, host_addr);

    DebugLog("RSX: rsxInit status = 0x%08X\n", (unsigned int)result);
    DebugLog("RSX: context = %p\n", rsx_context);

    if (result != 0 || rsx_context == NULL)
    {
        DebugLog("RSX: initialization failed!\n");

        if (rsx_context)
        {
            rsxFinish(rsx_context, 1);
            rsx_context = NULL;
        }

        free(host_addr);
        host_addr = NULL;

        return -1;
    }

	rsxSetBlendEnable(rsx_context, GCM_TRUE);
	rsxSetBlendFunc(rsx_context, GCM_SRC_ALPHA, GCM_ONE_MINUS_SRC_ALPHA, GCM_SRC_ALPHA, GCM_ONE_MINUS_SRC_ALPHA);
	rsxSetBlendEquation(rsx_context, GCM_FUNC_ADD, GCM_FUNC_ADD);
	
	rsxSetColorMask(rsx_context, GCM_COLOR_MASK_R|GCM_COLOR_MASK_G|GCM_COLOR_MASK_B|GCM_COLOR_MASK_A);
	rsxSetColorMaskMrt(rsx_context, 0);
	
    DebugLog("RSX: initialized successfully!\n");

    // Get display state
    if (videoGetState(0, 0, &display_state) != 0)
    {
        DebugLog("RSX: videoGetState failed!\n");
        return -1;
    }

    if (display_state.state != 0)
    {
        DebugLog("RSX: display is disabled!\n");
        return -1;
    }

    // Get resolution
    if (videoGetResolution(display_state.displayMode.resolution, &display_resolution) != 0)
    {
        DebugLog("RSX: videoGetResolution failed!\n");
        return -1;
    }

    screen_width = display_resolution.width;
    screen_height = display_resolution.height;

    DebugLog("RSX: display resolution: %dx%d\n", screen_width, screen_height);

    memset(&display_config, 0, sizeof(display_config));

    display_config.resolution = display_state.displayMode.resolution;
    display_config.format = VIDEO_BUFFER_FORMAT_XRGB;
    display_config.pitch = screen_width * sizeof(u32);
    display_config.aspect = display_state.displayMode.aspect;

    if (videoConfigure(0, &display_config, NULL, 0) != 0)
    {
        DebugLog("RSX: videoConfigure failed!\n");
        return -1;
    }

    gcmSetFlipMode(GCM_FLIP_VSYNC);
    gcmResetFlipStatus();
	
    DebugLog("RSX: video configured!\n");

    return 0;
}

void RSX_TestDraw(rsxBuffer *buffer)
{
    int x, y;

    for (y = 0; y < buffer->height; y++)
    {
        for (x = 0; x < buffer->width; x++)
        {
            buffer->ptr[y * buffer->width + x] = 0xFF2020FF;
        }
    }
}

gcmContextData *RSX_GetContext(void)
{
    return rsx_context;
}

static void RSX_ApplyFrameState(void)
{
    gcmContextData *c = RSX_GetContext();
    if (!vertex_program || !fragment_program) return;

    rsxLoadVertexProgram(c, vertex_program, vertex_ucode);
    rsxLoadFragmentProgramLocation(c, fragment_program, fragment_ucode_offset, GCM_LOCATION_RSX);

    rsxSetColorMask(c, GCM_COLOR_MASK_R | GCM_COLOR_MASK_G | GCM_COLOR_MASK_B | GCM_COLOR_MASK_A);
    rsxSetColorMaskMrt(c, 0);
    rsxSetBlendEnable(c, GCM_TRUE);
    rsxSetBlendFunc(c, GCM_SRC_ALPHA, GCM_ONE_MINUS_SRC_ALPHA, GCM_SRC_ALPHA, GCM_ONE_MINUS_SRC_ALPHA);
    rsxSetBlendEquation(c, GCM_FUNC_ADD, GCM_FUNC_ADD);
    rsxSetDepthTestEnable(c, GCM_FALSE);
    rsxSetDepthWriteEnable(c, GCM_FALSE);
    rsxSetCullFaceEnable(c, GCM_FALSE);
    rsxSetScissor(c, 0, 0, screen_width, screen_height);
	
	rsxSetAlphaTestEnable(c, GCM_FALSE);
	rsxSetStencilTestEnable(c, GCM_FALSE);
	rsxSetShadeModel(c, GCM_SHADE_MODEL_SMOOTH);
}

void RSX_SetRenderTarget(rsxBuffer *buffer)
{
    gcmSurface surface;
    memset(&surface, 0, sizeof(surface));

    surface.type = GCM_SURFACE_TYPE_LINEAR;
    surface.antiAlias = GCM_SURFACE_CENTER_1;

    surface.colorFormat = GCM_SURFACE_X8R8G8B8;
    surface.colorTarget = GCM_SURFACE_TARGET_0;

    surface.colorLocation[0] = GCM_LOCATION_RSX;
    surface.colorLocation[1] = GCM_LOCATION_RSX;
    surface.colorLocation[2] = GCM_LOCATION_RSX;
    surface.colorLocation[3] = GCM_LOCATION_RSX;
    surface.colorOffset[0] = buffer->offset;
    surface.colorOffset[1] = 0;
    surface.colorOffset[2] = 0;
    surface.colorOffset[3] = 0;
    surface.colorPitch[0] = buffer->width * 4;
    surface.colorPitch[1] = 64;
    surface.colorPitch[2] = 64;
    surface.colorPitch[3] = 64;

    surface.depthFormat = GCM_SURFACE_ZETA_Z16;
    surface.depthLocation = GCM_LOCATION_RSX;
    surface.depthOffset = depth_offset;
    surface.depthPitch = depth_pitch;

    surface.width = buffer->width;
    surface.height = buffer->height;
    surface.x = 0;
    surface.y = 0;

    rsxSetSurface(RSX_GetContext(), &surface);
	RSX_ApplyFrameState();
}

void RSX_SetupViewport(void)
{
    f32 scale[4] = {screen_width * 0.5f, -screen_height * 0.5f, 0.5f, 0.0f};
    f32 offset[4] = {screen_width * 0.5f, screen_height * 0.5f, 0.5f, 0.0f};
    rsxSetViewport(RSX_GetContext(), 0, 0, screen_width, screen_height, 0.0f, 1.0f, scale, offset);
    rsxSetScissor(RSX_GetContext(), 0, 0, screen_width, screen_height);

    rsxSetDepthTestEnable(RSX_GetContext(), GCM_FALSE);
    rsxSetDepthWriteEnable(RSX_GetContext(), GCM_FALSE);
    rsxSetCullFaceEnable(RSX_GetContext(), GCM_FALSE);
}

void RSX_ClearScreen(void)
{
    gcmContextData *context = RSX_GetContext();

    rsxSetClearColor(context, 0xFF000000);
    rsxClearSurface(context, GCM_CLEAR_R | GCM_CLEAR_G | GCM_CLEAR_B | GCM_CLEAR_A);
}

void RSX_TestClear(rsxBuffer *buffer)
{
    gcmContextData *context = RSX_GetContext();

    RSX_SetRenderTarget(buffer);
    RSX_SetupViewport();

    rsxSetClearColor(context, 0xFF000000);
    rsxClearSurface(context, GCM_CLEAR_R | GCM_CLEAR_G | GCM_CLEAR_B | GCM_CLEAR_A);

    rsxFlushBuffer(context);
}

int RSX_LoadShaders(void)
{
    gcmContextData *context = RSX_GetContext();

    DebugLog("RSX: loading shaders...\n");

    // Embedded shader metadata
    vertex_program = (rsxVertexProgram*)basic_vp_vpo;
    fragment_program = (rsxFragmentProgram*)basic_fp_fpo;

    // Get vertex microcode
    u32 vertex_ucode_size = 0;
    rsxVertexProgramGetUCode(vertex_program, &vertex_ucode, &vertex_ucode_size);

    DebugLog("RSX: vertex shader size: %u\n", vertex_ucode_size);

    // Get fragment microcode
    void *embedded_fragment_ucode = NULL;
    rsxFragmentProgramGetUCode(fragment_program, &embedded_fragment_ucode, &fragment_ucode_size);
    DebugLog("RSX: fragment shader size: %u\n", fragment_ucode_size);

    // Allocate RSX memory for fragment microcode
    fragment_ucode = rsxMemalign(64, fragment_ucode_size);

    if (!fragment_ucode)
    {
        DebugLog("RSX: fragment shader allocation failed!\n");
        return -1;
    }

    memcpy(fragment_ucode, embedded_fragment_ucode, fragment_ucode_size);
    if (rsxAddressToOffset(fragment_ucode, &fragment_ucode_offset) != 0)
    {
        DebugLog("RSX: fragment shader offset failed!\n");
        return -1;
    }

    rsxLoadVertexProgram(context, vertex_program, vertex_ucode);
    rsxLoadFragmentProgramLocation(context, fragment_program, fragment_ucode_offset, GCM_LOCATION_RSX);

    /* ---- adventure fragment shader ---- */
    adv_fragment_program = (rsxFragmentProgram*)adventure_fp_fpo;
    void *adv_embedded_ucode = NULL;
    rsxFragmentProgramGetUCode(adv_fragment_program, &adv_embedded_ucode, &adv_fragment_ucode_size);
    DebugLog("RSX: adventure fragment shader size: %u\n", adv_fragment_ucode_size);

    adv_fragment_ucode = rsxMemalign(64, adv_fragment_ucode_size);
    if (!adv_fragment_ucode)
    {
        DebugLog("RSX: adventure fragment shader allocation failed!\n");
        return -1;
    }
    memcpy(adv_fragment_ucode, adv_embedded_ucode, adv_fragment_ucode_size);
    if (rsxAddressToOffset(adv_fragment_ucode, &adv_fragment_ucode_offset) != 0)
    {
        DebugLog("RSX: adventure fragment shader offset failed!\n");
        return -1;
    }
    DebugLog("RSX: adventure shader loaded!\n");
    /* ----------------------------------- */

	rsxProgramAttrib *attribs = rsxVertexProgramGetAttribs(vertex_program);
	u16 count = rsxVertexProgramGetNumAttrib(vertex_program);
	DebugLog("RSX: vertex attributes: %u\n", count);

	for (u16 i = 0; i < count; i++)
	{
		DebugLog("  Attribute %u: index=%u name=%s\n", i, attribs[i].index, (char*)vertex_program + attribs[i].name_off);
	}
	
	rsxProgramConst *consts = rsxFragmentProgramGetConsts(fragment_program);
	u16 const_count = rsxFragmentProgramGetNumConst(fragment_program);
	DebugLog("RSX: fragment constants: %u\n", const_count);

	for (u16 i = 0; i < const_count; i++)
	{
		DebugLog("  Constant %u: index=%u name=%s type=%u\n", i, consts[i].index, (char*)fragment_program + consts[i].name_off, consts[i].type);
	}
	
    DebugLog("RSX: shaders loaded!\n");

    return 0;
}

int RSX_UploadSpritesheets(void)
{
    int i;
    int loaded = 0;

    for (i = 0; i < TOTAL_SHEETS; i++)
    {
        u32 *pixels;
        u32 y;
        u32 row_size;
        pngData *sheet = &spritesheet[i];

        // Skip unused or unloaded slots.
        if (!sheet->bmp_out || !sheet->width || !sheet->height) continue;
        row_size = sheet->width * sizeof(u32);

        pixels = malloc(row_size * sheet->height);
        if (!pixels)
        {
            DebugLog("RSX: failed to allocate sheet %d\n", i);
            continue;
        }

        // Copy each row, accounting for PNG decoder pitch.
        for (y = 0; y < sheet->height; y++)
        {
            memcpy((u8 *)pixels + y * row_size, (u8 *)sheet->bmp_out + y * sheet->pitch, row_size);
        }
		
		int up_ok = RSX_LoadTexture(i, pixels, (u16)sheet->width, (u16)sheet->height);
		if (up_ok)
		{
			loaded++;
			DebugLog("RSX: loaded spritesheet %d (%lux%lu)\n", i,
					 (unsigned long)sheet->width, (unsigned long)sheet->height);
		}
		else DebugLog("RSX: failed to upload spritesheet %d\n", i);

		free(pixels);

		if (up_ok)
		{
			free(sheet->bmp_out);
			sheet->bmp_out = NULL;
		}
		/*
        if (RSX_LoadTexture(i, pixels, (u16)sheet->width, (u16)sheet->height))
        {
            loaded++;
            DebugLog("RSX: loaded spritesheet %d (%lux%lu)\n", i, (unsigned long)sheet->width, (unsigned long)sheet->height);
        }
        else DebugLog("RSX: failed to upload spritesheet %d\n", i);

        free(pixels);
		*/
    }

    DebugLog("RSX: uploaded %d spritesheets\n", loaded);
    return loaded;
}

int RSX_LoadTexture(int id, const u32 *pixels, u16 width, u16 height)
{
    RSX_Texture *tex;
    u32 size;

    if (id < 0 || id >= RSX_MAX_TEXTURES) return 0;
    if (!pixels || width == 0 || height == 0) return 0;

    tex = &textures[id];
    size = width * height * sizeof(u32);

    /* Release an existing texture with this ID. */
    RSX_UnloadTexture(id);
    tex->data = (u32*)rsxMemalign(128, size);

    if (!tex->data) return 0;
    if (rsxAddressToOffset(tex->data, &tex->offset) != 0)
    {
        rsxFree(tex->data);
        tex->data = NULL;
        return 0;
    }
	
    memcpy(tex->data, pixels, size);

    tex->width = width;
    tex->height = height;

    memset(&tex->texture, 0, sizeof(gcmTexture));

    tex->texture.format = GCM_TEXTURE_FORMAT_A8R8G8B8 | GCM_TEXTURE_FORMAT_LIN;
    tex->texture.mipmap = 1;
    tex->texture.dimension = GCM_TEXTURE_DIMS_2D;
    tex->texture.cubemap = 0;

    tex->texture.remap =
		(GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_A_SHIFT) |
		(GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_R_SHIFT) |
		(GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_G_SHIFT) |
		(GCM_TEXTURE_REMAP_TYPE_REMAP << GCM_TEXTURE_REMAP_TYPE_B_SHIFT) |

		(GCM_TEXTURE_REMAP_COLOR_A << GCM_TEXTURE_REMAP_COLOR_A_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_R << GCM_TEXTURE_REMAP_COLOR_R_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_G << GCM_TEXTURE_REMAP_COLOR_G_SHIFT) |
		(GCM_TEXTURE_REMAP_COLOR_B << GCM_TEXTURE_REMAP_COLOR_B_SHIFT);

    tex->texture.width = width;
    tex->texture.height = height;
    tex->texture.depth = 1;
    tex->texture.location = GCM_LOCATION_RSX;
    tex->texture.pitch = width * sizeof(u32);
    tex->texture.offset = tex->offset;

    tex->loaded = 1;

    return 1;
}

void RSX_UnloadTexture(int id)
{
    RSX_Texture *tex;

    if (id < 0 || id >= RSX_MAX_TEXTURES) return;
    tex = &textures[id];

    if (tex->data)
    {
        rsxFree(tex->data);
        tex->data = NULL;
    }

    memset(tex, 0, sizeof(RSX_Texture));
}

void RSX_BindTexture(int id)
{
    gcmContextData *context = RSX_GetContext();
    RSX_Texture *tex;

    if (id < 0 || id >= RSX_MAX_TEXTURES) return;
    tex = &textures[id];
    if (!tex->loaded) return;

    rsxLoadTexture(context, 0, &tex->texture);
    rsxTextureControl(context, 0, GCM_TRUE, 0, 0, GCM_TEXTURE_MAX_ANISO_1);
	
	int f = tex->linear ? GCM_TEXTURE_LINEAR : GCM_TEXTURE_NEAREST;
	rsxTextureFilter(context, 0, 0, f, f, GCM_TEXTURE_CONVOLUTION_QUINCUNX);
	
    rsxTextureWrapMode( context, 0, GCM_TEXTURE_CLAMP_TO_EDGE, GCM_TEXTURE_CLAMP_TO_EDGE, GCM_TEXTURE_CLAMP_TO_EDGE,
        GCM_TEXTURE_UNSIGNED_REMAP_NORMAL, GCM_TEXTURE_ZFUNC_ALWAYS, GCM_FALSE);
}

int RSX_CreateSpriteBuffers(void)
{
    size_t bytes = sizeof(RSX_Vertex) * 6 * MAX_SPRITES_PER_FRAME * 2;
    sprite_vb = (RSX_Vertex*)rsxMemalign(128, bytes);
    if (!sprite_vb) return 0;
    if (rsxAddressToOffset(sprite_vb, &sprite_vb_offset) != 0) return 0;
    return 1;
}

void RSX_BeginFrame(void)
{
	sorted_ok = 1;
	draw_n = 0;
	cur_blend = 0;
	cur_shader = 0;
    volatile u32 *label = (volatile u32*)gcmGetLabelAddress(RSX_FENCE_LABEL);
    int tries = 0;
    while (fence_value > 1 && (s32)(*label - (fence_value - 1)) < 0 && ++tries < 10000) usleep(50);

    frame_idx ^= 1;
    sprite_n = 0;
}

static RSX_Vertex RSX_MakeVertex(float x, float y, u32 c, float u, float v)
{
    RSX_Vertex o;
    o.x = RSX_ScreenToNDC_X(x);
    o.y = RSX_ScreenToNDC_Y(y);
    o.z = 0.0f;
    o.w = 1.0f;
    o.r = ((c >> 24) & 0xFF) / 255.0f;
    o.g = ((c >> 16) & 0xFF) / 255.0f;
    o.b = ((c >>  8) & 0xFF) / 255.0f;
    o.a = ( c        & 0xFF) / 255.0f;
    o.u = u;
    o.v = v;
    return o;
}

static u32 RSX_MapToGameBoy(u32 rgba)
{
    float r = ((rgba >> 24) & 0xFF) / 255.0f;
    float g = ((rgba >> 16) & 0xFF) / 255.0f;
    float b = ((rgba >>  8) & 0xFF) / 255.0f;
    float lum = 0.299f * r + 0.587f * g + 0.114f * b;
    u32 col;
    if (lum < 0.26f) col = GB_PALETTE[0];
    else if (lum < 0.49f) col = GB_PALETTE[1];
    else if (lum < 0.67f) col = GB_PALETTE[2];
    else col = GB_PALETTE[3];
    return (col & 0xFFFFFF00) | (rgba & 0xFF);
}

static void RSX_EmitVerts(int texture_id, const RSX_Vertex *verts, int count)
{
    if (count <= 0 || count > 6 || draw_n >= MAX_SPRITES_PER_FRAME) return;
	
	float minx = verts[0].x, maxx = verts[0].x, miny = verts[0].y, maxy = verts[0].y;
    int k;
    for (k = 1; k < count; k++)
    {
        if (verts[k].x < minx) minx = verts[k].x;
        if (verts[k].x > maxx) maxx = verts[k].x;
        if (verts[k].y < miny) miny = verts[k].y;
        if (verts[k].y > maxy) maxy = verts[k].y;
    }
    if (maxx < -1.0f || minx > 1.0f || maxy < -1.0f || miny > 1.0f) return;
	
    QueuedDraw *d = &draw_q[draw_n];
    d->layer = cur_layer; d->seq = draw_n; d->tex = texture_id; d->count = count;
	d->blend = cur_blend; d->shader = cur_shader;
	
    if (gb_enabled)
    {
        int _gbi;
        for (_gbi = 0; _gbi < count; _gbi++)
        {
            d->v[_gbi] = verts[_gbi];
            u32 orig = ((u32)(verts[_gbi].r * 255) << 24) | ((u32)(verts[_gbi].g * 255) << 16)
                     | ((u32)(verts[_gbi].b * 255) <<  8) | ((u32)(verts[_gbi].a * 255));
            u32 mapped = RSX_MapToGameBoy(orig);
            d->v[_gbi].r = ((mapped >> 24) & 0xFF) / 255.0f;
            d->v[_gbi].g = ((mapped >> 16) & 0xFF) / 255.0f;
            d->v[_gbi].b = ((mapped >>  8) & 0xFF) / 255.0f;
            d->v[_gbi].a = ( mapped & 0xFF) / 255.0f;
        }
    }
    else memcpy(d->v, verts, sizeof(RSX_Vertex) * count);
	
	if (draw_n > 0 && d->layer < draw_q[draw_n - 1].layer) sorted_ok = 0;
    draw_n++;
}

static int RSX_ResolveTex(int t)
{
    if (t >= 0) return t;
    if (!textures[RSX_WHITE_TEX].loaded)
    {
        static u32 white[16 * 16];
        int i;
        for (i = 0; i < 16 * 16; i++) white[i] = 0xFFFFFFFF;
        if (!RSX_LoadTexture(RSX_WHITE_TEX, white, 16, 16)) return -1;
    }
    return RSX_WHITE_TEX;
}

static void RSX_ApplyBlend(gcmContextData *c, int mode)
{
    rsxSetBlendEnable(c, GCM_TRUE);
    switch (mode)
    {
        case 1: rsxSetBlendFunc(c, GCM_ONE_MINUS_DST_COLOR, GCM_ZERO, GCM_ONE, GCM_ZERO); break;
        case 2: rsxSetBlendFunc(c, GCM_DST_COLOR, GCM_ZERO, GCM_ONE, GCM_ZERO); break;
        default: rsxSetBlendFunc(c, GCM_SRC_ALPHA, GCM_ONE_MINUS_SRC_ALPHA, GCM_SRC_ALPHA, GCM_ONE_MINUS_SRC_ALPHA); break;
    }
    rsxSetBlendEquation(c, GCM_FUNC_ADD, GCM_FUNC_ADD);
}

void RSX_FlushDraws(void)
{
    gcmContextData *context = RSX_GetContext();
    const u32 frame_verts = MAX_SPRITES_PER_FRAME * 6;
    u32 vert_n = 0;
    int i, j;

    for (i = 0; i < draw_n; i++) draw_order[i] = i;
	if (!sorted_ok) qsort(draw_order, draw_n, sizeof(int), cmp_draw);

    i = 0;
    while (i < draw_n)
    {
        int tex    = RSX_ResolveTex(draw_q[draw_order[i]].tex);
		int blend  = draw_q[draw_order[i]].blend;
		int shader = draw_q[draw_order[i]].shader;
        u32 first = vert_n;
        RSX_Vertex *run = sprite_vb + frame_idx * frame_verts + first;

        for (j = i; j < draw_n; j++)
        {
            QueuedDraw *d = &draw_q[draw_order[j]];
            if (RSX_ResolveTex(d->tex) != tex || d->blend != blend || d->shader != shader) break;
            if (vert_n + d->count > frame_verts) break;
            memcpy(sprite_vb + frame_idx * frame_verts + vert_n, d->v, sizeof(RSX_Vertex) * d->count);
            vert_n += d->count;
        }
        if (j == i) break;                       // out of vertex space

        if (tex >= 0)
        {
            u32 off = sprite_vb_offset + (u32)((u8*)run - (u8*)sprite_vb);
            u32 n = vert_n - first, start = 0;

            /* Switch shader — load FP first, then VP.
             * rsxInvalidateVertexCache flushes the program cache so RSX
             * doesn't silently ignore a re-bind of the same VP pointer. */
            if (shader == 1 && adv_fragment_program && adv_fragment_ucode)
            {
				//DebugLog("RSX: switching to adventure shader\n");
                rsxLoadFragmentProgramLocation(context, adv_fragment_program,
                    adv_fragment_ucode_offset, GCM_LOCATION_RSX);
            }
            else
            {
                rsxLoadFragmentProgramLocation(context, fragment_program,
                    fragment_ucode_offset, GCM_LOCATION_RSX);
            }
            rsxInvalidateVertexCache(context);
            rsxLoadVertexProgram(context, vertex_program, vertex_ucode);

            rsxBindVertexArrayAttrib(context, 0, 0, off + offsetof(RSX_Vertex, x), sizeof(RSX_Vertex), 4, GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX);
            rsxBindVertexArrayAttrib(context, 3, 0, off + offsetof(RSX_Vertex, r), sizeof(RSX_Vertex), 4, GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX);
            rsxBindVertexArrayAttrib(context, 8, 0, off + offsetof(RSX_Vertex, u), sizeof(RSX_Vertex), 2, GCM_VERTEX_DATA_TYPE_F32, GCM_LOCATION_RSX);
            rsxInvalidateVertexCache(context);
            rsxInvalidateTextureCache(context, GCM_INVALIDATE_TEXTURE);
			RSX_ApplyBlend(context, blend);
            RSX_BindTexture(tex);
			if (tex == RSX_SCANLINE_TEX) rsxTextureWrapMode(context, 0, GCM_TEXTURE_CLAMP_TO_EDGE, GCM_TEXTURE_REPEAT,
				GCM_TEXTURE_CLAMP_TO_EDGE, GCM_TEXTURE_UNSIGNED_REMAP_NORMAL, GCM_TEXTURE_ZFUNC_ALWAYS, GCM_FALSE);
            while (n)
            {
                u32 c = (n > 252) ? 252 : n;
                rsxDrawVertexArray(context, GCM_TYPE_TRIANGLES, start, c);
                start += c; n -= c;
            }
        }
        i = j;
    }
    draw_n = 0;
}

static void RSX_EmitQuad(int tex, RSX_Vertex a, RSX_Vertex b, RSX_Vertex c, RSX_Vertex d)
{
    RSX_Vertex v[6] = { a, b, c, a, c, d };
    RSX_EmitVerts(tex, v, 6);
}

static void RSX_BuildVignette(u32 *out, int w, int h)
{
    int x, y;
    float cx = w * 0.5f, cy = h * 0.5f;
    float maxd = cx * cx + cy * cy;
    for (y = 0; y < h; y++)
    {
        for (x = 0; x < w; x++)
        {
            float dx = x - cx, dy = y - cy;
            float d  = (dx*dx + dy*dy) / maxd;
            float v  = d * d * d;
            if (v > 1.0f) v = 1.0f;
            u8 a = (u8)(v * 255.0f);
            out[y * w + x] = ((u32)a << 24) | 0x000000;
        }
    }
}

static void RSX_BuildScanline(u32 *out)
{
    out[0] = 0x00000000;   /* transparent – bright row */
    out[1] = 0xFF000000;   /* opaque black – dark row  */
}

int RSX_InitCRT(void)
{
    static u32 vignette[128 * 128];
    u32 scanline[2];

    RSX_BuildVignette(vignette, 128, 128);
    if (!RSX_LoadTexture(RSX_VIGNETTE_TEX, vignette, 128, 128))
	{
		DebugLog("RSX: LoadTextureVignette failed\n");
		return 0;
	}
    RSX_SetTextureFilter(RSX_VIGNETTE_TEX, 1);

    RSX_BuildScanline(scanline);
    if (!RSX_LoadTexture(RSX_SCANLINE_TEX, scanline, 1, 2))
	{
		DebugLog("RSX: LoadTextureScanline failed\n");
		return 0;
	}
    RSX_SetTextureFilter(RSX_SCANLINE_TEX, 0);

    crt_ready = 1;
    DebugLog("RSX: CRT effects ready\n");
    return 1;
}

void RSX_DrawCRT(void)
{
    float prev_layer;

    if (!crt_enabled || !crt_ready) return;

    prev_layer = cur_layer;
    cur_layer  = 20.0f;

    if (crt_scanline_alpha > 0.0f)
    {
        RSX_Vertex a, b, c, d;
        float x0 = RSX_ScreenToNDC_X(-40),   y0 = RSX_ScreenToNDC_Y(-22);
        float x1 = RSX_ScreenToNDC_X(1040),  y1 = RSX_ScreenToNDC_Y(1022);
        float vtiles = logical_height / 2.0f;
        float sa = crt_scanline_alpha;

        RSX_SetBlendMode(0);

        a.x=x0; a.y=y0; a.z=0; a.w=1; a.r=0; a.g=0; a.b=0; a.a=sa; a.u=0;      a.v=0;
        b.x=x1; b.y=y0; b.z=0; b.w=1; b.r=0; b.g=0; b.b=0; b.a=sa; b.u=1;      b.v=0;
        c.x=x1; c.y=y1; c.z=0; c.w=1; c.r=0; c.g=0; c.b=0; c.a=sa; c.u=1;      c.v=vtiles;
        d.x=x0; d.y=y1; d.z=0; d.w=1; d.r=0; d.g=0; d.b=0; d.a=sa; d.u=0;      d.v=vtiles;

        RSX_EmitQuad(RSX_SCANLINE_TEX, a, b, c, d);
    }

    if (crt_vignette_alpha > 0.0f)
    {
        RSX_Vertex a, b, c, d;
        float x0 = RSX_ScreenToNDC_X(-40),   y0 = RSX_ScreenToNDC_Y(-22);
        float x1 = RSX_ScreenToNDC_X(1040),  y1 = RSX_ScreenToNDC_Y(1022);
        float va = crt_vignette_alpha;

        RSX_SetBlendMode(0);

        a.x=x0; a.y=y0; a.z=0; a.w=1; a.r=0; a.g=0; a.b=0; a.a=va; a.u=0; a.v=0;
        b.x=x1; b.y=y0; b.z=0; b.w=1; b.r=0; b.g=0; b.b=0; b.a=va; b.u=1; b.v=0;
        c.x=x1; c.y=y1; c.z=0; c.w=1; c.r=0; c.g=0; c.b=0; c.a=va; c.u=1; c.v=1;
        d.x=x0; d.y=y1; d.z=0; d.w=1; d.r=0; d.g=0; d.b=0; d.a=va; d.u=0; d.v=1;

        RSX_EmitQuad(RSX_VIGNETTE_TEX, a, b, c, d);
    }

    RSX_SetBlendMode(0);
    cur_layer = prev_layer;
}

static void RSX_EmitRotatedQuad(int tex, float cx, float cy, float hw, float hh, float angle, float u0, float v0, float u1, float v1, u32 color)
{
    float rad = angle * (RSX_PI / 180.0f);
    float c = cosf(rad), s = sinf(rad);

    float x0 = (-hw * c) - (-hh * s), y0 = (-hw * s) + (-hh * c);
    float x1 = ( hw * c) - (-hh * s), y1 = ( hw * s) + (-hh * c);
    float x2 = ( hw * c) - ( hh * s), y2 = ( hw * s) + ( hh * c);
    float x3 = (-hw * c) - ( hh * s), y3 = (-hw * s) + ( hh * c);

    RSX_EmitQuad(tex, RSX_MakeVertex(cx + x0, cy + y0, color, u0, v0),
        RSX_MakeVertex(cx + x1, cy + y1, color, u1, v0),
        RSX_MakeVertex(cx + x2, cy + y2, color, u1, v1),
        RSX_MakeVertex(cx + x3, cy + y3, color, u0, v1));
}

void RSX_DrawBackground2D(u32 rgba)
{
    RSX_EmitQuad(-1, RSX_MakeVertex( -40.0f,  -22.0f, rgba, 0, 0),
        RSX_MakeVertex(1040.0f,  -22.0f, rgba, 0, 0),
        RSX_MakeVertex(1040.0f, 1022.0f, rgba, 0, 0),
        RSX_MakeVertex( -40.0f, 1022.0f, rgba, 0, 0));
}

void RSX_DrawRectangle2D(float x, float y, float w, float h, u32 rgba)
{
    RSX_EmitQuad(-1, RSX_MakeVertex(x + 40,     y + 22,     rgba, 0, 0),
        RSX_MakeVertex(x + w + 40, y + 22,     rgba, 0, 0),
        RSX_MakeVertex(x + w + 40, y + h + 22, rgba, 0, 0),
        RSX_MakeVertex(x + 40,     y + h + 22, rgba, 0, 0));
}

void RSX_DrawRotatingTriangle(float x, float y, float size, float angle, u32 color)
{
    float rad = angle * RSX_PI / 180.0f;
    float a1 = rad - RSX_PI / 2.0f;
    float a2 = a1 + 2.0f * RSX_PI / 3.0f;
    float a3 = a1 + 4.0f * RSX_PI / 3.0f;

    RSX_Vertex v[3] =
    {
        RSX_MakeVertex(x + cosf(a1) * size + 40, y + sinf(a1) * size + 22, color, 0, 0),
        RSX_MakeVertex(x + cosf(a2) * size + 40, y + sinf(a2) * size + 22, color, 0, 0),
        RSX_MakeVertex(x + cosf(a3) * size + 40, y + sinf(a3) * size + 22, color, 0, 0)
    };
    RSX_EmitVerts(-1, v, 3);
}

void RSX_DrawSpriteUI(int texture_id, float x, float y, int wid1, int hei1, int wid2, int hei2, int wid0, int hei0, int wid3, int hei3, u32 color)
{
    float u0 = (float)wid1 / (float)wid0, v0 = (float)hei1 / (float)hei0;
    float u1 = (float)wid2 / (float)wid0, v1 = (float)hei2 / (float)hei0;

    RSX_EmitQuad(texture_id, RSX_MakeVertex(x + 40,        y + 22,        color, u0, v0),
        RSX_MakeVertex(x + wid3 + 40, y + 22,        color, u1, v0),
        RSX_MakeVertex(x + wid3 + 40, y + hei3 + 22, color, u1, v1),
        RSX_MakeVertex(x + 40,        y + hei3 + 22, color, u0, v1));
}

void RSX_DrawSpriteUIR(int texture_id, float x, float y, int wid1, int hei1, int wid2, int hei2, int wid0, int hei0, int wid3, int hei3, float angle, u32 color)
{
    float u0 = (float)wid1 / (float)wid0, v0 = (float)hei1 / (float)hei0;
    float u1 = (float)wid2 / (float)wid0, v1 = (float)hei2 / (float)hei0;

    RSX_EmitRotatedQuad(texture_id, x + wid3 * 0.5f + 40, y + hei3 * 0.5f + 22, wid3 * 0.5f, hei3 * 0.5f, angle, u0, v0, u1, v1, color);
}

void RSX_DrawSprite2DR(int texture_id, int id, float x, float y, int wid2, int hei2, int wid1, int hei1, int wid0, int hei0, float angle, u32 color)
{
    int cols = wid0 / wid1;
    int col = id % cols;
    int row = id / cols;

    float u0 = (float)(col * wid1) / wid0,       v0 = (float)(row * hei1) / hei0;
    float u1 = (float)((col + 1) * wid1) / wid0, v1 = (float)((row + 1) * hei1) / hei0;

    RSX_EmitRotatedQuad(texture_id, x + wid2 * 0.5f + 40, y + hei2 * 0.5f + 22, wid2 * 0.5f, hei2 * 0.5f, angle, u0, v0, u1, v1, color);
}

void RSX_SetTextureFilter(int id, int linear)
{
    if (id < 0 || id >= RSX_MAX_TEXTURES) return;
    textures[id].linear = linear ? 1 : 0;
}

void RSX_EndFrameMark(void)
{
    gcmContextData *ctx = RSX_GetContext();
    fence_value++;
    rsxSetWriteBackendLabel(ctx, RSX_FENCE_LABEL, fence_value);
    rsxFlushBuffer(ctx);
}

void drawFrame(rsxBuffer *buffer, long frame)
{
	s32 i, j;
	for(i = 0; i < buffer->height; i++)
	{
		s32 color = (i / (buffer->height * 1.0) * 256);
		color = (color << 8) | ((frame % 255) << 16);
		for(j = 0; j < buffer->width; j++) buffer->ptr[i* buffer->width + j] = color;
	}
}

void RSX_DrawSprite2D(int texture_id, float x, float y, float width, float height,
	float u, float v, float uv_width, float uv_height, u32 color)
{
    RSX_EmitQuad(texture_id,
	RSX_MakeVertex(x,         y,          color, u,            v),
	RSX_MakeVertex(x + width, y,          color, u + uv_width, v),
	RSX_MakeVertex(x + width, y + height, color, u + uv_width, v + uv_height),
	RSX_MakeVertex(x,         y + height, color, u,            v + uv_height));
}


void RSX_DrawSprite(int texture_id, int id, float x, float y, int dest_w, int dest_h, int sprite_w, int sprite_h, u32 color)
{
    RSX_Texture *tex;
    int cols;
    int rows;
    int col;
    int row;

    float u, v;
    float uw, vh;

    if (texture_id < 0 || texture_id >= RSX_MAX_TEXTURES) return;
    tex = &textures[texture_id];
    if (!tex->loaded || sprite_w <= 0 || sprite_h <= 0) return;

    cols = tex->width / sprite_w;
    rows = tex->height / sprite_h;

    if (cols <= 0 || rows <= 0 || id < 0 || id >= cols * rows) return;

    col = id % cols;
    row = id / cols;

    // Calculate UV coordinates.
    u  = (float)(col * sprite_w) / tex->width;
    v  = (float)(row * sprite_h) / tex->height;

    uw = (float)sprite_w / tex->width;
    vh = (float)sprite_h / tex->height;

    RSX_DrawSprite2D(texture_id, x+40, y+22, dest_w, dest_h, u, v, uw, vh, color);
}

/* Waits until the RSX has finished everything queued so far. */
static void RSX_WaitIdle(void)
{
    gcmContextData *c = RSX_GetContext();
    volatile u32 *label = (volatile u32*)gcmGetLabelAddress(RSX_FENCE_LABEL);
    int tries = 0;

    fence_value++;
    rsxSetWriteBackendLabel(c, RSX_FENCE_LABEL, fence_value);
    rsxFlushBuffer(c);
    while (*label != fence_value && ++tries < 20000) usleep(50);
}

int RSX_UpdateTexture(int id, const u32 *pixels, u16 width, u16 height)
{
    RSX_Texture *tex;
    if (id < 0 || id >= RSX_MAX_TEXTURES || !pixels) return 0;
    tex = &textures[id];
    if (!tex->data || tex->width != width || tex->height != height)
        return RSX_LoadTexture(id, pixels, width, height);   /* first frame */
    RSX_WaitIdle();                                          /* GPU done with last frame */
    memcpy(tex->data, pixels, (size_t)width * height * sizeof(u32));
    return 1;
}

int RSX_GetTextureRSXOffset(int id, u32 *out_offset)
{
    RSX_Texture *tex;
    if (!out_offset) return 0;
    *out_offset = 0;
    if (id < 0 || id >= RSX_MAX_TEXTURES) return 0;
    tex = &textures[id];
    if (!tex->loaded || !tex->data) return 0;
    *out_offset = tex->offset;
    return 1;
}

void RSX_UnloadTextureSafe(int id)
{
    RSX_WaitIdle();
    RSX_UnloadTexture(id);
}

int RSX_UploadSheet(int i)
{
	DebugLog("RSX_UploadSheet\n");
    pngData *sheet = &spritesheet[i];
    u32 y, row_size;
    int ok;

    if (i < 0 || i >= TOTAL_SHEETS) return 0;
    if (!sheet->bmp_out || !sheet->width || !sheet->height) return 0;

    RSX_WaitIdle();
    if (textures[i].loaded) RSX_UnloadTexture(i);

	const u32 *src;
	u32 *pixels = NULL;
	row_size = sheet->width * sizeof(u32);

	if (sheet->pitch == row_size) src = (const u32*)sheet->bmp_out;
	else
	{
		pixels = malloc(row_size * sheet->height);
		if (!pixels)
		{
			DebugLog("SHEET %d: malloc failed (%ux%u pitch=%u)\n", i, (unsigned)sheet->width, (unsigned)sheet->height, (unsigned)sheet->pitch);
			return 0;
		}
		for (y = 0; y < sheet->height; y++) memcpy((u8*)pixels + y * row_size, (u8*)sheet->bmp_out + y * sheet->pitch, row_size);
		src = pixels;
	}

	ok = RSX_LoadTexture(i, src, (u16)sheet->width, (u16)sheet->height);
	free(pixels);

    free(sheet->bmp_out);
    sheet->bmp_out = NULL;
	DebugLog("%d\n", ok);
    return ok;
}

void RSX_UnloadSheet(int i)
{
    if (i < 0 || i >= RSX_MAX_TEXTURES || !textures[i].loaded) return;
    RSX_WaitIdle();
    RSX_UnloadTexture(i);
}