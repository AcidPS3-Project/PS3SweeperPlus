#ifndef PS_EYE_H
#define PS_EYE_H

#include <ppu-types.h>

/*
 * PS Eye integration for PS3Sweeper+
 *
 * Call PS_Eye_Init() once after RSX is ready.
 * Call PS_Eye_Update() every frame inside the game loop — it reads a
 * camera frame, converts YUV422→ARGB, and uploads it to texture slot
 * RSX_CAMERA_TEX so the renderer can use it like any other texture.
 * Call PS_Eye_Shutdown() on exit.
 *
 * PS_Eye_IsLive() returns non-zero when a frame is available and the
 * camera is running; drawing code uses this to decide whether to show
 * the camera feed or fall back to the captain portrait.
 */

/* Texture slot reserved for the camera feed.
 * RSX_MAX_TEXTURES is 96; slots 93-95 are taken by the renderer's
 * own effects, so we use slot 92 (RSX_MAX_TEXTURES - 4 - 1). */
#define RSX_CAMERA_TEX  91

#define PSEYE_WIDTH  640
#define PSEYE_HEIGHT 480

/* Initialise the camera subsystem.  Returns 0 on success. */
int  PS_Eye_Init(void);

/* Read one camera frame and push it to RSX_CAMERA_TEX.
 * Call once per game-loop iteration; safe to call even when the camera
 * is not present — it is a no-op in that case. */
void PS_Eye_Update(void);

/* Shut down the camera and free its resources. */
void PS_Eye_Shutdown(void);

/* Returns non-zero when the camera feed texture is ready to display. */
int  PS_Eye_IsLive(void);

#endif /* PS_EYE_H */
