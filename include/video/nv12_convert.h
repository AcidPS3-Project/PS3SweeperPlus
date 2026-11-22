#ifndef NV12_CONVERT_H
#define NV12_CONVERT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * nv12_convert.h — NV12 (YUV 4:2:0 semi-planar) → ARGB8888
 *
 * vdec on PS3 outputs NV12:
 *   - Y plane:  width x height bytes
 *   - UV plane: width x (height/2) bytes, interleaved U,V pairs
 *
 * Output: ARGB8888 (0xFF_RR_GG_BB) suitable for RSX_LoadTexture.
 *
 * This runs on PPU (no SPU). For 1280x720 it takes ~8ms per frame on a Slim,
 * which is fine for 30fps video (33ms budget). If you need faster, run it in
 * a PPU thread while the GPU blits the previous frame.
 */

static inline uint8_t nv12_clamp(int v)
{
    if (v < 0)   return 0;
    if (v > 255) return 255;
    return (uint8_t)v;
}

/*
 * Convert one NV12 frame to ARGB.
 *
 * y_plane   : pointer to Y data (width * height bytes)
 * uv_plane  : pointer to UV data (width * height/2 bytes, interleaved)
 * y_stride  : bytes per row in Y plane (usually == width, but vdec may pad)
 * uv_stride : bytes per row in UV plane (usually == width)
 * out_argb  : output buffer, must be width*height*4 bytes, 128-byte aligned
 * width     : frame width  (must be even)
 * height    : frame height (must be even)
 */
static void nv12_to_argb(
    const uint8_t *y_plane,  int y_stride,
    const uint8_t *uv_plane, int uv_stride,
    uint32_t      *out_argb,
    int width, int height)
{
    int row, col;

    for (row = 0; row < height; row++)
    {
        const uint8_t *y_row  = y_plane  + row * y_stride;
        const uint8_t *uv_row = uv_plane + (row >> 1) * uv_stride;
        uint32_t      *out    = out_argb + row * width;

        for (col = 0; col < width; col += 2)
        {
            int Y0 = (int)y_row[col]   - 16;
            int Y1 = (int)y_row[col+1] - 16;
            int U  = (int)uv_row[col]  - 128;
            int V  = (int)uv_row[col+1]- 128;

            /* BT.601 coefficients, integer approximation (*1024 scale) */
            int rv =  1402 * V;
            int gu = - 344 * U;
            int gv = - 714 * V;
            int bu =  1772 * U;
            int Yc0 = 1164 * Y0;
            int Yc1 = 1164 * Y1;

            out[col]   = 0xFF000000
                | ((uint32_t)nv12_clamp((Yc0 + rv)        >> 10) << 16)
                | ((uint32_t)nv12_clamp((Yc0 + gu + gv)   >> 10) <<  8)
                | ((uint32_t)nv12_clamp((Yc0 + bu)        >> 10));

            out[col+1] = 0xFF000000
                | ((uint32_t)nv12_clamp((Yc1 + rv)        >> 10) << 16)
                | ((uint32_t)nv12_clamp((Yc1 + gu + gv)   >> 10) <<  8)
                | ((uint32_t)nv12_clamp((Yc1 + bu)        >> 10));
        }
    }
}

#ifdef __cplusplus
}
#endif
#endif /* NV12_CONVERT_H */
