#ifndef RSX_RENDERER_H
#define RSX_RENDERER_H

#include <ppu-types.h>

#include <rsx/rsx.h>
#include <sysutil/video.h>
#include <rsx/gcm_sys.h>
#include "rsxutil.h"

#define RSX_MAX_TEXTURES 64

typedef struct
{
    u32 *data;
    u32 offset;

    u16 width;
    u16 height;

    gcmTexture texture;

    int loaded;
	int linear;
} RSX_Texture;

int RSX_LoadTexture(int id, const u32 *pixels, u16 width, u16 height);
void RSX_UnloadTexture(int id);
void RSX_BindTexture(int id);
void RSX_CreateTextureTest(void);

extern u32 *depth_buffer;
extern u32 depth_offset;
extern u32 depth_pitch;

int RSX_Init(void);
void RSX_TestDraw(rsxBuffer *buffer);
gcmContextData *RSX_GetContext(void);
void RSX_TestClear(rsxBuffer *buffer);
int RSX_LoadShaders(void);
void RSX_SetLogicalResolution(float width, float height);
int RSX_CreateSpriteBuffers(void);
void RSX_SetupViewport(void);
void RSX_SetRenderTarget(rsxBuffer *buffer);
void RSX_ClearScreen(void);
int RSX_UploadSpritesheets(void);
void RSX_BeginFrame(void);

void RSX_DrawSprite2D(int texture_id, float x, float y, float width, float height, float u, float v, float uv_width, float uv_height, u32 color);
void RSX_DrawSprite(int texture_id, int id, float x, float y, int dest_w, int dest_h, int sprite_w, int sprite_h, u32 color);

void RSX_DrawBackground2D(u32 rgba);
void RSX_DrawRectangle2D(float x, float y, float w, float h, u32 rgba);
void RSX_DrawRotatingTriangle(float x, float y, float size, float angle, u32 color);
void RSX_DrawSpriteUI(int texture_id, float x, float y, int wid1, int hei1, int wid2, int hei2, int wid0, int hei0, int wid3, int hei3, u32 color);
void RSX_DrawSpriteUIR(int texture_id, float x, float y, int wid1, int hei1, int wid2, int hei2, int wid0, int hei0, int wid3, int hei3, float angle, u32 color);
void RSX_DrawSprite2DR(int texture_id, int id, float x, float y, int wid2, int hei2, int wid1, int hei1, int wid0, int hei0, float angle, u32 color);

void RSX_SetTextureFilter(int id, int linear);
void RSX_EndFrameMark(void);

#endif