/*
 * ps_eye.c — PS Eye camera integration for PS3Sweeper+
 *
 * Adapted from KoishisEyeSample. Handles:
 *   - sysmodule load / cameraInit / cameraOpenEx / cameraStart
 *   - per-frame YUV422 -> ARGB conversion (same math as the sample)
 *   - upload to RSX texture slot RSX_CAMERA_TEX via RSX_UpdateTexture()
 *   - graceful fallback when no camera is attached
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <malloc.h>

#include <io/camera.h>
#include <sysmodule/sysmodule.h>

#include "render/rsx_renderer.h"   /* RSX_UpdateTexture, RSX_CAMERA_TEX */
#include "camera/ps_eye.h"

/* ------------------------------------------------------------------ */
/* Internal state                                                       */
/* ------------------------------------------------------------------ */

static int  g_module_loaded   = 0;
static int  g_cam_initialized = 0;
static int  g_cam_opened      = 0;
static int  g_cam_started     = 0;
static int  g_live            = 0;   /* frame has been successfully uploaded */

static sys_mem_container_t g_container;
static int                 g_container_created = 0;

static cameraInfoEx g_cam_info;

/* ARGB pixel buffer for the converted frame — allocated from main RAM,
   but only used as a CPU-side staging area before memcpy into RSX mem. */
static u32 *g_rgb_buf = NULL;

/* ------------------------------------------------------------------ */
/* YUV422 -> ARGB conversion (same algorithm as KoishisEyeSample)      */
/* ------------------------------------------------------------------ */

static inline u32 yuv_to_argb(int y, int u, int v)
{
    int r, g, b;

    v -= 128;
    u -= 128;

    r = y + u;
    g = y - u / 2 - v / 8;
    b = y + v;

    if (r < 0) r = 0;
	else if (r > 255) r = 255;
    if (g < 0) g = 0;
	else if (g > 255) g = 255;
    if (b < 0) b = 0;
	else if (b > 255) b = 255;

    return 0xFF000000u | ((u32)r << 16) | ((u32)g << 8) | (u32)b;
}

static void convert_frame(const u8 *yuv, u32 *rgb)
{
    int x, y;
    for (y = 0; y < PSEYE_HEIGHT; y++)
    {
        const u8 *src = yuv + y * PSEYE_WIDTH * 2;   /* YUY2: 2 bytes/pixel */
        u32      *dst = rgb + y * PSEYE_WIDTH;

        for (x = 0; x < PSEYE_WIDTH; x += 2)
        {
            int y1 = src[0];
            int v  = src[1];
            int y2 = src[2];
            int u  = src[3];

            dst[x]     = yuv_to_argb(y1, u, v);
            dst[x + 1] = yuv_to_argb(y2, u, v);
            src += 4;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */

int PS_Eye_Init(void)
{
    s32 ret;

    /* Allocate the CPU-side conversion buffer. */
    g_rgb_buf = (u32 *)malloc(PSEYE_WIDTH * PSEYE_HEIGHT * sizeof(u32));
    if (!g_rgb_buf)
    {
        printf("[PSEye] malloc failed\n");
        return -1;
    }
    memset(g_rgb_buf, 0, PSEYE_WIDTH * PSEYE_HEIGHT * sizeof(u32));

    /* Memory container required by cameraOpenEx. */
    ret = sysMemContainerCreate(&g_container, 2 * 1024 * 1024);
    if (ret != 0)
    {
        printf("[PSEye] sysMemContainerCreate failed: 0x%08x\n", (unsigned)ret);
        goto fail;
    }
    g_container_created = 1;

    /* Load the camera sysmodule (may already be loaded — that's fine). */
    ret = sysModuleLoad(SYSMODULE_CAMERA);
    if (ret != 0 && ret != (s32)0x80012001 /* already loaded */)
    {
        printf("[PSEye] sysModuleLoad(CAMERA) failed: 0x%08x\n", (unsigned)ret);
        goto fail;
    }
    g_module_loaded = 1;

    ret = cameraInit();
    if (ret != 0)
    {
        printf("[PSEye] cameraInit failed: 0x%08x\n", (unsigned)ret);
        goto fail;
    }
    g_cam_initialized = 1;

    /* Check if a PS Eye is actually connected before bothering to open. */
    if (!cameraIsAttached(0))
    {
        printf("[PSEye] no camera attached — running without camera\n");
        return 0;   /* non-fatal; PS_Eye_IsLive() will return 0 */
    }

    memset(&g_cam_info, 0, sizeof(g_cam_info));
    g_cam_info.format     = CAM_FORM_YUV422;
    g_cam_info.framerate  = 30;
    g_cam_info.resolution = CAM_RESO_VGA;      /* 640×480 */
    g_cam_info.info_ver   = 0x101;
    g_cam_info.container  = g_container;

    ret = cameraOpenEx(0, &g_cam_info);
    if (ret != 0)
    {
        printf("[PSEye] cameraOpenEx failed: 0x%08x\n", (unsigned)ret);
        goto fail;
    }
    g_cam_opened = 1;

    cameraReset(0);

    ret = cameraStart(0);
    if (ret != 0)
    {
        printf("[PSEye] cameraStart failed: 0x%08x\n", (unsigned)ret);
        goto fail;
    }
    g_cam_started = 1;

    printf("[PSEye] PS Eye initialised OK (640x480 YUV422 @ 30fps)\n");
    return 0;

fail:
    PS_Eye_Shutdown();
    return -1;
}

void PS_Eye_Update(void)
{
    u32  frame     = 0;
    u32  readcount = 0;
    s32  ret;

    if (!g_cam_started) return;

    /* Re-attach if the camera was briefly disconnected. */
    if (!cameraIsAttached(0) || !cameraIsAvailable(0))
    {
        g_live = 0;
        return;
    }

    ret = cameraRead(0, &frame, &readcount);

    if (ret == CAMERA_ERRO_NEED_START)
    {
        cameraReset(0);
        cameraStart(0);
        g_live = 0;
        return;
    }

    if (ret != 0 || readcount == 0) return;

    /* Convert YUV422 -> ARGB into our CPU buffer. */
    convert_frame((const u8 *)(u64)g_cam_info.buffer, g_rgb_buf);

    /* Upload to RSX texture slot RSX_CAMERA_TEX.
     * RSX_UpdateTexture() does a WaitIdle + memcpy on first call it
     * allocates RSX memory; on subsequent calls it just memcpy's. */
    if (RSX_UpdateTexture(RSX_CAMERA_TEX,
                          g_rgb_buf,
                          (u16)PSEYE_WIDTH,
                          (u16)PSEYE_HEIGHT))
    {
        g_live = 1;
    }
}

void PS_Eye_Shutdown(void)
{
    g_live = 0;

    if (g_cam_started)     { cameraStop(0);              g_cam_started     = 0; }
    if (g_cam_opened)      { cameraClose(0);             g_cam_opened      = 0; }
    if (g_cam_initialized) { cameraEnd();                g_cam_initialized = 0; }
    if (g_container_created){ sysMemContainerDestroy(g_container);
                              g_container_created = 0; }
    if (g_module_loaded)   { /* sysModuleUnload is optional and risky */
                              g_module_loaded = 0; }

    if (g_rgb_buf)
    {
        free(g_rgb_buf);
        g_rgb_buf = NULL;
    }

    /* Release the RSX texture slot. */
    RSX_UnloadTextureSafe(RSX_CAMERA_TEX);
}

int PS_Eye_IsLive(void)
{
    return g_live;
}
