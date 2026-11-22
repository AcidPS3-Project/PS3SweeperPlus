/*
 * video_dec.c — PS3 hardware H.264 decoder (PSL1GHT)
 *
 * Notes:
 *  - vdecDecodeAu() is ASYNCHRONOUS: the decoder reads the packet later,
 *    so every AU needs its own buffer that stays untouched until the
 *    AUDONE callback fires. We use a small ring of buffers.
 *  - The vdec callback runs on libvdec's own thread. It does NO system
 *    calls: it only bumps counters. Each counter has exactly one writer
 *    thread, so no locking is needed.
 *  - Shutdown: vdecEndSequence -> wait SEQDONE -> vdecClose -> free.
 */

#include "video_dec.h"
#include "importer.h"

#include <ppu-lv2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <sys/systime.h>
#include <codec/vdec.h>
#include <sysmodule/sysmodule.h>

/* Callback message types (Sony libvdec: AUDONE 0, PICOUT 1, SEQDONE 2, ERROR 3).
 * Verify against your codec/vdec.h if behaviour looks wrong. */
#define CB_AUDONE   0
#define CB_PICOUT   1
#define CB_SEQDONE  2
#define CB_ERROR    3

/* ------------------------------------------------------------------ */
/* State                                                                */
/* ------------------------------------------------------------------ */

static u32   s_vdec_handle = 0;
static int   s_vdec_open   = 0;
static void *s_vdec_mem    = NULL;

/* written ONLY by the callback thread */
static volatile u32 s_cnt_audone = 0;   /* AUs the decoder finished reading */
static volatile u32 s_cnt_picout = 0;   /* pictures ready                   */
static volatile u32 s_seqdone    = 0;
static volatile u32 s_cnt_error  = 0;
static volatile u32 s_last_err   = 0;
static volatile u32 s_cnt_cb     = 0;   /* total callbacks                  */
static volatile u32 s_last_msg   = 0xFFFF;
static volatile u32 s_cnt_other  = 0;   /* unknown message types            */

/* written ONLY by the main thread */
static volatile u32 s_cnt_fed    = 0;   /* AUs submitted                    */
static volatile u32 s_cnt_got    = 0;   /* pictures fetched                 */

static int s_width  = 0;
static int s_height = 0;

static uint32_t *s_argb_buf      = NULL;
static uint32_t  s_argb_buf_size = 0;

/* AU ring: AU number N lives in slot N % AU_SLOTS and is free again
 * once s_cnt_audone > N (AUs complete in order). */
#define AU_SLOTS    8
#define AU_BUF_SIZE (512 * 1024)
static uint8_t *s_au_buf[AU_SLOTS];

/* SPS+PPS in Annex-B form. NEVER sent as an AU of their own (ffmpeg-based
 * decoders reject a packet with no slice); they are prepended to the first
 * keyframe AU and to every later IDR. */
#define PS_BUF_SIZE 2048
static uint8_t  s_ps_buf[PS_BUF_SIZE];
static uint32_t s_ps_len     = 0;
static int      s_ps_pending = 0;

/* ------------------------------------------------------------------ */
/* Annex-B conversion                                                   */
/* ------------------------------------------------------------------ */

static void write_sc(uint8_t *d)
{ d[0]=0; d[1]=0; d[2]=0; d[3]=1; }

static uint32_t avcc_to_annexb(const uint8_t *avcc, uint32_t sz,
                                uint8_t *dst, uint32_t dmax)
{
    uint32_t out = 0, i;
    int sps_n, pps_n;
    const uint8_t *p = avcc;
    if (sz < 7) return 0;
    p += 5;
    sps_n = (*p++) & 0x1F;
    for (i = 0; i < (uint32_t)sps_n; i++) {
        uint16_t len = ((uint16_t)p[0]<<8)|p[1]; p += 2;
        if ((size_t)(p-avcc)+len > sz || out+4+len > dmax) return 0;
        write_sc(dst+out); out += 4;
        memcpy(dst+out, p, len); out += len; p += len;
    }
    pps_n = *p++;
    for (i = 0; i < (uint32_t)pps_n; i++) {
        uint16_t len = ((uint16_t)p[0]<<8)|p[1]; p += 2;
        if ((size_t)(p-avcc)+len > sz || out+4+len > dmax) return 0;
        write_sc(dst+out); out += 4;
        memcpy(dst+out, p, len); out += len; p += len;
    }
    return out;
}

/* 1 if the length-prefixed sample contains an IDR slice (NAL type 5) */
static int sample_has_idr(const uint8_t *s, uint32_t ssz)
{
    uint32_t pos = 0;
    while (pos + 5 <= ssz) {
        uint32_t nlen = ((uint32_t)s[pos]<<24)|((uint32_t)s[pos+1]<<16)
                       |((uint32_t)s[pos+2]<<8)|s[pos+3];
        if (nlen == 0 || pos + 4 + nlen > ssz) break;
        if ((s[pos+4] & 0x1F) == 5) return 1;
        pos += 4 + nlen;
    }
    return 0;
}

/* Length-prefixed (AVCC) sample -> Annex-B, optionally preceded by `pre`. */
static uint32_t sample_to_annexb(const uint8_t *s, uint32_t ssz,
                                  uint8_t *dst, uint32_t dmax,
                                  const uint8_t *pre, uint32_t pre_len)
{
    uint32_t out = 0, pos = 0;
    if (pre && pre_len) {
        if (pre_len > dmax) return 0;
        memcpy(dst, pre, pre_len);
        out = pre_len;
    }
    while (pos + 4 <= ssz) {
        uint32_t nlen = ((uint32_t)s[pos]<<24)|((uint32_t)s[pos+1]<<16)
                       |((uint32_t)s[pos+2]<<8)|s[pos+3];
        pos += 4;
        if (pos + nlen > ssz || out + 4 + nlen > dmax) break;
        write_sc(dst+out); out += 4;
        memcpy(dst+out, s+pos, nlen); out += nlen; pos += nlen;
    }
    return out;
}

/* ------------------------------------------------------------------ */
/* vdec callback — counters only, no system calls                       */
/* ------------------------------------------------------------------ */

static u32 vdec_cb(u32 handle, u32 msgtype, u32 msgdata, u32 arg)
{
    (void)handle; (void)arg;

    s_cnt_cb   = s_cnt_cb + 1;
    s_last_msg = msgtype;

    switch (msgtype)
    {
    case CB_AUDONE:  s_cnt_audone = s_cnt_audone + 1; break;
    case CB_PICOUT:  s_cnt_picout = s_cnt_picout + 1; break;
    case CB_SEQDONE: s_seqdone = 1;                   break;
    case CB_ERROR:   s_cnt_error = s_cnt_error + 1; s_last_err = msgdata; break;
    default:         s_cnt_other = s_cnt_other + 1;   break;
    }
    return 0;
}

/* ------------------------------------------------------------------ */
/* AU submission                                                        */
/* ------------------------------------------------------------------ */

/* Returns 1 ok, -1 busy (retry later), 0 fatal. `src` is one AVCC sample. */
static int submit_au(const uint8_t *src, uint32_t src_size)
{
    vdecAU   au;
    uint32_t n, pre_len = 0;
    const uint8_t *pre = NULL;
    u32      fed, done;
    int      slot, ret;

    fed  = s_cnt_fed;
    done = s_cnt_audone;
    if (fed - done >= AU_SLOTS) return -1;     /* every buffer still in flight */
    __sync_synchronize();

    slot = (int)(fed % AU_SLOTS);

    if (s_ps_len && (s_ps_pending || sample_has_idr(src, src_size))) {
        pre = s_ps_buf; pre_len = s_ps_len;
    }
    n = sample_to_annexb(src, src_size, s_au_buf[slot], AU_BUF_SIZE, pre, pre_len);

    if (!n) {
        DebugLog("VDEC: AU convert failed (src %u bytes, max %u)\n",
                 src_size, (unsigned)AU_BUF_SIZE);
        return 0;
    }

    memset(&au, 0, sizeof(au));
    au.packet_addr = (u32)(uintptr_t)s_au_buf[slot];
    au.packet_size = n;
    au.pts.low = VDEC_TS_INVALID; au.pts.hi = VDEC_TS_INVALID;
    au.dts.low = VDEC_TS_INVALID; au.dts.hi = VDEC_TS_INVALID;

    __sync_synchronize();
    s_cnt_fed = fed + 1;          /* count it BEFORE the decoder can finish it */

    if (fed < 12) DebugLog("VDEC: DecodeAu #%u slot=%d size=%u%s\n", (unsigned)fed, slot,
                           (unsigned)n, pre ? " (with SPS/PPS)" : "");
    ret = vdecDecodeAu(s_vdec_handle, VDEC_DECODER_MODE_NORMAL, &au);
    if (fed < 12) DebugLog("VDEC: DecodeAu #%u returned 0x%08X\n", (unsigned)fed, ret);

    if ((u32)ret == (u32)VDEC_ERROR_BUSY) { s_cnt_fed = fed; return -1; }
    if (ret != 0) {
        s_cnt_fed = fed;
        DebugLog("VDEC: DecodeAu err 0x%08X\n", ret);
        return 0;
    }
    if (pre) s_ps_pending = 0;
    return 1;
}

static struct { u32 entry; u32 toc; } s_cb_opd __attribute__((aligned(8)));

static u32 make_opd32(void *fn)
{
    const u64 *d = (const u64*)fn;      /* PSL1GHT's 64-bit descriptor */
    s_cb_opd.entry = (u32)d[0];
    s_cb_opd.toc   = (u32)d[1];
    return (u32)(uintptr_t)&s_cb_opd;
}

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */

int VDEC_Init(int frame_width, int frame_height, uint32_t *argb_buf)
{
    vdecType    vtype;
    vdecConfig  vconfig;
    vdecClosure vclosure;
    vdecAttr    vattr;
    static const u32 levels[]  = { 31, 40, 42 };
    static const u32 spu_try[] = { 1, 4, 2, 3 };
    int li, si, i, ret;

    s_width         = frame_width;
    s_height        = frame_height;
    s_argb_buf      = argb_buf;
    s_argb_buf_size = (uint32_t)(frame_width * frame_height * 4);

    s_seqdone = 0;
    s_cnt_audone = 0; s_cnt_picout = 0; s_cnt_error = 0; s_last_err = 0;
    s_cnt_cb = 0; s_last_msg = 0xFFFF; s_cnt_other = 0;
    s_cnt_fed = 0; s_cnt_got = 0;
    s_ps_len = 0; s_ps_pending = 0;

    sysModuleLoad(SYSMODULE_VDEC);

    /* Lowest level whose decoder memory we can allocate */
    s_vdec_mem = NULL;
    for (li = 0; li < 3; li++) {
        memset(&vtype, 0, sizeof(vtype));
        vtype.codec_type    = VDEC_CODEC_TYPE_H264;
        vtype.profile_level = levels[li];

        memset(&vattr, 0, sizeof(vattr));
        ret = vdecQueryAttr(&vtype, &vattr);
        if (ret != 0) { DebugLog("VDEC: level %u query 0x%08X\n", levels[li], ret); continue; }

        DebugLog("VDEC: level %u needs %u bytes\n", levels[li], (unsigned)vattr.mem_size);
        s_vdec_mem = memalign(128, vattr.mem_size);
        if (s_vdec_mem) break;
    }
    if (!s_vdec_mem) { DebugLog("VDEC: no level fit in memory\n"); return 0; }

    memset(&vconfig, 0, sizeof(vconfig));
    vconfig.mem_addr              = (u32)(uintptr_t)s_vdec_mem;
    vconfig.mem_size              = vattr.mem_size;
    vconfig.ppu_thread_prio       = 1001;
    vconfig.ppu_thread_stack_size = 0x10000;
    vconfig.spu_thread_prio       = 250;
    vconfig.num_spus              = 1;

    /* AU ring buffers */
    for (i = 0; i < AU_SLOTS; i++) {
        s_au_buf[i] = (uint8_t*)memalign(128, AU_BUF_SIZE);
        if (!s_au_buf[i]) {
            DebugLog("VDEC: AU buf %d alloc fail\n", i);
            goto fail_mem;
        }
    }

    memset(&vclosure, 0, sizeof(vclosure));
    vclosure.fn  = make_opd32((void*)vdec_cb);
    vclosure.arg = 0;

    ret = -1;
    for (si = 0; si < 4; si++) {
        vconfig.num_spus = spu_try[si];
        DebugLog("VDEC: open try spus=%u mem=%p size=%u lvl=%u\n",
                 (unsigned)vconfig.num_spus, s_vdec_mem,
                 (unsigned)vconfig.mem_size, (unsigned)vtype.profile_level);
        ret = vdecOpen(&vtype, &vconfig, &vclosure, &s_vdec_handle);
        DebugLog("VDEC: vdecOpen -> 0x%08X\n", ret);
        if (ret == 0) break;
    }
    if (ret != 0) goto fail_mem;

    ret = vdecStartSequence(s_vdec_handle);
    if (ret != 0) {
        DebugLog("VDEC: vdecStartSequence failed: 0x%08X\n", ret);
        vdecClose(s_vdec_handle);
        goto fail_mem;
    }

    s_vdec_open = 1;
    DebugLog("VDEC: init OK %dx%d\n", frame_width, frame_height);
    return 1;

fail_mem:
    for (i = 0; i < AU_SLOTS; i++) { free(s_au_buf[i]); s_au_buf[i] = NULL; }
    free(s_vdec_mem); s_vdec_mem = NULL;
    return 0;
}

/* Pop every picture the decoder has ready (it can't finish a sequence
 * while its output queue is full of pictures nobody collected). */
static void drain_pictures(void)
{
    vdecPictureFormat fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.format_type  = VDEC_PICFMT_ARGB32;
    fmt.color_matrix = VDEC_COLOR_MATRIX_BT601;
    fmt.alpha        = 0xFF;

    while ((int)(s_cnt_picout - s_cnt_got) > 0) {
        if (vdecGetPicture(s_vdec_handle, &fmt, s_argb_buf) != 0) {
            s_cnt_got = s_cnt_picout;      /* give up on the rest */
            break;
        }
        s_cnt_got = s_cnt_got + 1;
    }
}

/* Wait (up to ~3s) for SEQDONE after vdecEndSequence, draining output */
static void wait_seqdone(void)
{
    int i;
    for (i = 0; i < 300 && !s_seqdone; i++) {
        drain_pictures();
        sysUsleep(10000);
    }
    drain_pictures();
    if (!s_seqdone) DebugLog("VDEC: SEQDONE timeout\n");
}

void VDEC_Close(void)
{
    int i;
    if (!s_vdec_open) return;

    s_seqdone = 0;
    vdecEndSequence(s_vdec_handle);
    wait_seqdone();

    vdecClose(s_vdec_handle);
    s_vdec_open = 0;

    sysModuleUnload(SYSMODULE_VDEC);

    for (i = 0; i < AU_SLOTS; i++) { free(s_au_buf[i]); s_au_buf[i] = NULL; }
    free(s_vdec_mem); s_vdec_mem = NULL;
}

int VDEC_SetSPSPPS(const uint8_t *avcc_data, uint32_t avcc_size)
{
    uint32_t n;
    if (!s_vdec_open)                  { DebugLog("VDEC: SPS/PPS: not open\n"); return 0; }
    if (!avcc_data || avcc_size < 7)   { DebugLog("VDEC: SPS/PPS: bad avcC (%u)\n", avcc_size); return 0; }
    n = avcc_to_annexb(avcc_data, avcc_size, s_ps_buf, PS_BUF_SIZE);
    if (!n)                            { DebugLog("VDEC: SPS/PPS: convert failed\n"); return 0; }
    s_ps_len     = n;
    s_ps_pending = 1;
    DebugLog("VDEC: SPS/PPS stored (%u bytes), sent with the first keyframe\n", (unsigned)n);
    return 1;
}

int VDEC_DecodeNAL(const uint8_t *data, uint32_t size)
{
    if (!s_vdec_open || !data || !size) return 0;
    return submit_au(data, size);
}

int VDEC_GetFrame(VDEC_Frame *frame)
{
    vdecPictureFormat fmt;
    int ret;

    if ((int)(s_cnt_picout - s_cnt_got) <= 0) return 0;

    memset(&fmt, 0, sizeof(fmt));
    fmt.format_type  = VDEC_PICFMT_ARGB32;
    fmt.color_matrix = VDEC_COLOR_MATRIX_BT601;
    fmt.alpha        = 0xFF;

    ret = vdecGetPicture(s_vdec_handle, &fmt, s_argb_buf);
    if (ret != 0) {
        DebugLog("VDEC: vdecGetPicture err 0x%08X\n", ret);
        return 0;
    }

    s_cnt_got = s_cnt_got + 1;

    frame->argb     = s_argb_buf;
    frame->width    = s_width;
    frame->height   = s_height;
    frame->frame_id = 0;
    return 1;
}

void VDEC_ReleaseFrame(VDEC_Frame *frame)
{
    (void)frame;
}

void VDEC_Flush(void)
{
    vdecPictureFormat fmt;
    int i;
    if (!s_vdec_open) return;

    s_seqdone = 0;
    vdecEndSequence(s_vdec_handle);
    wait_seqdone();

    memset(&fmt, 0, sizeof(fmt));
    fmt.format_type  = VDEC_PICFMT_ARGB32;
    fmt.color_matrix = VDEC_COLOR_MATRIX_BT601;
    fmt.alpha        = 0xFF;
    for (i = 0; i < 32; i++) {
        if (vdecGetPicture(s_vdec_handle, &fmt, s_argb_buf) != 0) break;
    }

    s_cnt_fed = 0; s_cnt_audone = 0; s_cnt_picout = 0; s_cnt_got = 0;
    s_ps_pending = (s_ps_len != 0);

    vdecStartSequence(s_vdec_handle);
}

void VDEC_LogStats(void)
{
    DebugLog("VDEC: fed=%u audone=%u picout=%u got=%u cb=%u last_msg=%u other=%u err=%u(0x%X)\n",
             (unsigned)s_cnt_fed, (unsigned)s_cnt_audone, (unsigned)s_cnt_picout,
             (unsigned)s_cnt_got, (unsigned)s_cnt_cb, (unsigned)s_last_msg,
             (unsigned)s_cnt_other, (unsigned)s_cnt_error, (unsigned)s_last_err);
}
