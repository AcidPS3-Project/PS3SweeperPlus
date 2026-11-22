#ifndef MP4PLAYER_H
#define MP4PLAYER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * mp4player.h — Minimal MP4 video player for PS3 homebrew (PSL1GHT)
 *
 * Integration with rsx_renderer:
 *   - Call MP4Player_Init() once.
 *   - Each frame, call MP4Player_Tick() — it decodes the next video frame
 *     and uploads it to a texture slot you pick.
 *   - Draw it with RSX_DrawSprite2D() (or any other RSX draw call).
 *   - Call MP4Player_Close() when done.
 *
 * Audio: not handled here. Pre-extract audio with:
 *     ffmpeg -i input.mp4 -vn -acodec pcm_s16be -ar 48000 audio.raw
 *   then play through your existing audio port setup.
 *
 * Supported: H.264 Baseline/Main/High up to level 4.1 (1080p30 / 720p60).
 * File must be on PS3 filesystem (HDD, USB, etc.) — not streamed.
 */

typedef struct
{
    int    tex_id;      /* RSX texture slot to upload frames into          */
    int    video_track; /* index into the MP4 file's track list (usually 0)*/
    float  fps;         /* detected frame rate (may be 0 if undetected)    */
    int    width;       /* frame width in pixels                            */
    int    height;      /* frame height in pixels                          */
    int    is_playing;
    int    is_finished;

    /* private — do not touch */
    void  *_priv;
} MP4Player;

/*
 * Open an MP4 file and prepare for playback.
 *   ctx      : pointer to an MP4Player you allocated (stack or heap is fine)
 *   path     : PS3 path, e.g. "/dev_hdd0/movies/video.mp4"
 *   tex_id   : RSX texture slot (0..RSX_MAX_TEXTURES-5) to use for frames
 *
 * Returns 1 on success, 0 on failure.
 * On success, ctx->width/height/fps are filled.
 */
int MP4Player_Init(MP4Player *ctx, const char *path, int tex_id);

/*
 * Call once per game loop iteration.
 * Checks the frame timer, decodes the next frame if due, converts NV12→ARGB,
 * and uploads it via RSX_LoadTexture into ctx->tex_id.
 *
 * Returns:
 *   1  — a new frame was uploaded (draw it)
 *   0  — no new frame yet (not time for the next frame; don't redraw)
 *  -1  — playback finished (all frames decoded); call MP4Player_Close()
 */
int MP4Player_Tick(MP4Player *ctx);

/*
 * Seek to the beginning and restart playback.
 * Note: vdec does not support mid-stream seeking to arbitrary positions
 * without re-initializing; this restarts from frame 0.
 */
void MP4Player_Rewind(MP4Player *ctx);

/* Release all resources. Safe to call even if Init failed. */
void MP4Player_Close(MP4Player *ctx);

#ifdef __cplusplus
}
#endif
#endif /* MP4PLAYER_H */
