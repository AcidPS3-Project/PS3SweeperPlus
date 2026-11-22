/*
 * mp4player.c — MP4 video player for PS3 (PSL1GHT)
 *
 * Flow: minimp4 demux → vdec hardware decode → vdecGetPicture(ARGB32)
 *       → RSX_LoadTexture
 *
 * No NV12 conversion needed: we ask vdec to output ARGB32 directly.
 * nv12_convert.h is no longer used but kept in the folder for reference.
 */

#include "mp4player.h"
#include "video_dec.h"
#include "rsx_renderer.h"   /* RSX_LoadTexture */

#define MINIMP4_IMPLEMENTATION
#include "minimp4.h"
#include "importer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/time.h>
#include <sys/systime.h>
#include <ppu-lv2.h>

/* ------------------------------------------------------------------ */
/* Private state                                                        */
/* ------------------------------------------------------------------ */

typedef struct
{
    FILE          *fp;
    MP4D_demux_t   demux;
    int            video_track_idx;

    uint32_t       sample_idx;
    uint32_t       total_samples;

    uint8_t       *sample_buf;
    uint32_t       sample_buf_sz;

    uint32_t      *argb_buf;        /* width*height*4, 128-byte aligned */

    uint64_t       frame_period_us;

    int            decode_ahead;
    int            max_ahead;

    uint64_t       last_output_time;
    uint64_t       last_stats_time;

    /* A/V sync: picture k is due at start_time + k * frame_period_us */
    int            started;
    uint64_t       start_time;
    uint32_t       pics_shown;       /* pictures taken from the decoder so far */
} MP4PlayerPriv;

/* Shift the video against the music (microseconds). Positive = video later.
 * Tune on real hardware if the BGM starts a little earlier/later than the picture. */
#define MP4_AV_OFFSET_US   0
/* Most pictures dropped/fetched per Tick while catching up after a stall. */
#define MP4_MAX_CATCHUP    6
/* Periodic decoder stats in the log. Each DebugLog opens/closes a file on
 * the HDD, which stalls real hardware for 100+ ms. Leave OFF for normal use. */
/* #define MP4_DEBUG_STATS */

/* ------------------------------------------------------------------ */
/* minimp4 file read callback                                          */
/* ------------------------------------------------------------------ */

static int mp4_read_cb(int64_t offset, void *buffer, size_t size, void *token)
{
    FILE *fp = (FILE*)token;
    if (fseek(fp, (long)offset, SEEK_SET) != 0) return 0;
    return (fread(buffer, 1, size, fp) == size) ? 1 : 0;
}

/* ------------------------------------------------------------------ */
/* Time                                                                 */
/* ------------------------------------------------------------------ */

static uint64_t get_time_us(void)
{
    u64 sec  = 0;
    u64 nsec = 0;
    sysGetCurrentTime(&sec, &nsec);
    return sec * 1000000ULL + (uint64_t)nsec / 1000ULL;
}

/* ------------------------------------------------------------------ */
/* Internal helpers                                                     */
/* ------------------------------------------------------------------ */

static int submit_next_sample(MP4Player *ctx, MP4PlayerPriv *priv)
{
    int64_t  off;
    uint32_t sz;
    int      ret;

    if (priv->sample_idx >= priv->total_samples) return 0;
    if (!MP4D_get_sample(&priv->demux, priv->video_track_idx,
                         priv->sample_idx, &off, &sz)) return 0;

    if (sz > priv->sample_buf_sz) {
        free(priv->sample_buf);
        priv->sample_buf_sz = sz + 4096;
        priv->sample_buf = (uint8_t*)malloc(priv->sample_buf_sz);
        if (!priv->sample_buf) return 0;
    }

    if (fseek(priv->fp, (long)off, SEEK_SET) != 0) return 0;
    if (fread(priv->sample_buf, 1, sz, priv->fp) != sz) return 0;

    ret = VDEC_DecodeNAL(priv->sample_buf, sz);
    if (ret == 0)  return 0;   /* fatal */
    if (ret == -1) return -1;  /* busy — retry later */

    priv->sample_idx++;
    priv->decode_ahead++;
    return 1;
    (void)ctx;
}

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */

int MP4Player_Init(MP4Player *ctx, const char *path, int tex_id)
{
    MP4PlayerPriv *priv;
    long file_size;
    int i, found_video;
    float fps;

    memset(ctx, 0, sizeof(*ctx));
    ctx->tex_id = tex_id;

    priv = (MP4PlayerPriv*)calloc(1, sizeof(MP4PlayerPriv));
    if (!priv) return 0;
    ctx->_priv = priv;

    priv->fp = fopen(path, "rb");
    if (!priv->fp) { DebugLog("MP4Player: cannot open '%s'\n", path); goto fail; }

    fseek(priv->fp, 0, SEEK_END);
    file_size = ftell(priv->fp);
    rewind(priv->fp);

    if (!MP4D_open(&priv->demux, mp4_read_cb, priv->fp, (int64_t)file_size)) {
        DebugLog("MP4Player: MP4D_open failed\n"); goto fail;
    }

    /* Find H.264 video track */
    found_video = -1;
    for (i = 0; i < priv->demux.track_count; i++) {
        if (priv->demux.track[i].handler_type == 0x76696465 /* 'vide' */ &&
            priv->demux.track[i].dsi != NULL) {
            found_video = i; break;
        }
    }
    if (found_video < 0) { DebugLog("MP4Player: no H.264 track\n"); goto fail; }

    priv->video_track_idx = found_video;
    priv->total_samples   = priv->demux.track[found_video].sample_count;
    ctx->width            = priv->demux.track[found_video].SampleDescription_width;
    ctx->height           = priv->demux.track[found_video].SampleDescription_height;
    ctx->video_track      = found_video;

    if (!ctx->width || !ctx->height) {
        DebugLog("MP4Player: zero frame dimensions\n"); goto fail;
    }

    /* FPS */
    fps = 30.0f;
    {
        uint32_t num = priv->demux.track[found_video].avg_fps_num;
        uint32_t den = priv->demux.track[found_video].avg_fps_den;
        if (num && den) fps = (float)num / (float)den;
        if      (fps > 23.9f && fps < 24.1f) fps = 24.0f;
        else if (fps > 29.9f && fps < 30.1f) fps = 30.0f;
        else if (fps > 59.9f && fps < 60.1f) fps = 60.0f;
    }
    ctx->fps = fps;
    priv->frame_period_us = (uint64_t)(1000000.0f / fps);

    DebugLog("MP4Player: %dx%d @ %.2f fps, %u samples\n",
           ctx->width, ctx->height, ctx->fps, priv->total_samples);

    /* ARGB buffer — vdecGetPicture writes here */
    priv->argb_buf = (uint32_t*)memalign(128,
                        (size_t)(ctx->width * ctx->height * 4));
    if (!priv->argb_buf) { DebugLog("MP4Player: ARGB alloc failed\n"); goto fail; }

    if (!VDEC_Init(ctx->width, ctx->height, priv->argb_buf)) {
        DebugLog("MP4Player: VDEC_Init failed\n"); goto fail;
    }

    if (!VDEC_SetSPSPPS(priv->demux.track[found_video].dsi,
                         priv->demux.track[found_video].dsi_bytes)) {
        DebugLog("MP4Player: SPS/PPS feed failed\n"); goto fail;
    }

    /* Pre-fill decode pipeline */
    priv->max_ahead = 10;
    {
        int n;
        for (n = 0; n < priv->max_ahead && priv->sample_idx < priv->total_samples; n++)
            submit_next_sample(ctx, priv);
    }

    priv->last_output_time = get_time_us();
    priv->last_stats_time  = get_time_us();
    priv->started    = 0;
    priv->pics_shown = 0;
    ctx->is_playing  = 1;
    ctx->is_finished = 0;
    return 1;

fail:
	DebugLog("MP4Player: Closing cuz failed\n");
    MP4Player_Close(ctx);
    return 0;
}

int MP4Player_Tick(MP4Player *ctx)
{
    MP4PlayerPriv *priv;
    uint64_t       now, elapsed;
    uint32_t       due;
    int            fetched = 0, tries = 0;
    VDEC_Frame     frame;

    if (!ctx || !ctx->_priv || !ctx->is_playing) return -1;
    priv = (MP4PlayerPriv*)ctx->_priv;
    if (ctx->is_finished) return -1;

    /* Keep pipeline fed */
    while (priv->decode_ahead < priv->max_ahead &&
           priv->sample_idx < priv->total_samples) {
        int r = submit_next_sample(ctx, priv);
        if (r <= 0) break;
    }

    now = get_time_us();

    /* The clock starts on the first Tick, i.e. the same game frame in which
     * the screen (and its BGM) starts - not when the decoder was opened. */
    if (!priv->started) {
        priv->started    = 1;
        priv->start_time = now + MP4_AV_OFFSET_US;
        priv->last_output_time = now;
    }

#ifdef MP4_DEBUG_STATS
    if (now - priv->last_stats_time > 1000000ULL) {
        priv->last_stats_time = now;
        VDEC_LogStats();
    }
#endif

    if (now < priv->start_time) return 0;

    /* How many pictures should have been shown by now? (picture k is due at
     * start + k*period). If we are behind, fetch the late ones and drop all
     * but the newest, so the picture catches up with the music instead of
     * staying late forever. */
    elapsed = now - priv->start_time;
    due     = (uint32_t)(elapsed / priv->frame_period_us) + 1;

    while (priv->pics_shown < due && tries < MP4_MAX_CATCHUP) {
        tries++;
        if (!VDEC_GetFrame(&frame)) break;        /* decoder hasn't got it yet */
        priv->pics_shown++;
        priv->decode_ahead--;
        fetched = 1;                              /* frame.argb holds the newest */
    }

    if (fetched) {
        priv->last_output_time = now;
        RSX_UpdateTexture(ctx->tex_id, frame.argb,
                          (u16)frame.width, (u16)frame.height);
        VDEC_ReleaseFrame(&frame);
    }
    else if (priv->sample_idx >= priv->total_samples &&
             now - priv->last_output_time > 1000000ULL) {
        /* everything submitted, decoder holds back its last reorder frames */
        ctx->is_finished = 1;
        return -1;
    }

    if (priv->pics_shown >= priv->total_samples) {
        ctx->is_finished = 1;
        return -1;
    }

    return fetched ? 1 : 0;
}

void MP4Player_Rewind(MP4Player *ctx)
{
    MP4PlayerPriv *priv;
    int n;
    if (!ctx || !ctx->_priv) return;
    priv = (MP4PlayerPriv*)ctx->_priv;

    VDEC_Flush();
    priv->sample_idx   = 0;
    priv->decode_ahead = 0;
    ctx->is_finished   = 0;
    ctx->is_playing    = 1;

    VDEC_SetSPSPPS(priv->demux.track[priv->video_track_idx].dsi,
                   priv->demux.track[priv->video_track_idx].dsi_bytes);

    for (n = 0; n < priv->max_ahead && priv->sample_idx < priv->total_samples; n++)
        submit_next_sample(ctx, priv);

    priv->started          = 0;      /* clock restarts on the next Tick */
    priv->pics_shown       = 0;
    priv->last_output_time = get_time_us();
}

void MP4Player_Close(MP4Player *ctx)
{
    MP4PlayerPriv *priv;
    if (!ctx) return;
    priv = (MP4PlayerPriv*)ctx->_priv;
    if (!priv) return;

    VDEC_Close();   /* EndSequence -> drain -> wait SEQDONE -> vdecClose */
    RSX_UnloadTextureSafe(ctx->tex_id);   /* waits for the GPU first */
    MP4D_close(&priv->demux);

    if (priv->fp)         { fclose(priv->fp); priv->fp = NULL; }
    if (priv->argb_buf)   { free(priv->argb_buf); priv->argb_buf = NULL; }
    if (priv->sample_buf) { free(priv->sample_buf); priv->sample_buf = NULL; }

    free(priv);
    ctx->_priv      = NULL;
    ctx->is_playing = 0;
}
