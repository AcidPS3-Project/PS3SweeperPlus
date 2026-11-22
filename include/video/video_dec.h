#ifndef VIDEO_DEC_H
#define VIDEO_DEC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/*
 * video_dec.h — PS3 hardware H.264 decoder wrapper
 *
 * vdecGetPicture writes decoded pixels into a caller-supplied buffer
 * in whatever format you request. We ask for VDEC_PICFMT_ARGB32, so
 * the frame comes out ready for RSX_LoadTexture — no NV12 conversion.
 *
 * The argb_buf passed to VDEC_Init must be:
 *   - width * height * 4 bytes
 *   - 128-byte aligned (use memalign)
 *   - kept alive until VDEC_Close()
 */

#define VDEC_MAX_QUEUE  16

typedef struct
{
    uint32_t *argb;     /* points into the buf you passed to VDEC_Init */
    int       width;
    int       height;
    uint32_t  frame_id; /* unused, kept for API compat */
} VDEC_Frame;

/*
 * argb_buf : caller-allocated, width*height*4 bytes, 128-byte aligned.
 *            vdecGetPicture writes each decoded frame here.
 */
int  VDEC_Init(int frame_width, int frame_height, uint32_t *argb_buf);
void VDEC_Close(void);

int  VDEC_SetSPSPPS(const uint8_t *avcc_data, uint32_t avcc_size);

/* Returns 1=ok, -1=busy (retry), 0=fatal */
int  VDEC_DecodeNAL(const uint8_t *data, uint32_t size);

/* Non-blocking. Returns 1 and fills *frame if a frame is ready. */
int  VDEC_GetFrame(VDEC_Frame *frame);

/* Call after done with a frame. No-op in this implementation. */
void VDEC_ReleaseFrame(VDEC_Frame *frame);

/* Flush decoder (end+restart sequence). */
void VDEC_Flush(void);

/* Logs callback counters; call about once per second while playing. */
void VDEC_LogStats(void);

#ifdef __cplusplus
}
#endif
#endif /* VIDEO_DEC_H */
