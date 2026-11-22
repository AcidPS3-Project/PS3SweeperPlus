/*
 * minimp4.h — Minimal MP4 demuxer, single header, C89.
 * Written from scratch for PS3/PSL1GHT. No external dependencies.
 *
 * Usage:
 *   #define MINIMP4_IMPLEMENTATION
 *   #include "minimp4.h"
 */

#ifndef MINIMP4_H
#define MINIMP4_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int (*MP4D_read_cb)(int64_t offset, void *buffer, size_t size, void *token);

typedef struct
{
    uint32_t handler_type;
    uint32_t sample_count;
    uint32_t *entry_size;
    uint32_t *entry_offset;
    uint8_t  *dsi;
    uint32_t  dsi_bytes;
    uint32_t  timescale;
    uint32_t  duration;
    uint32_t  avg_fps_num;
    uint32_t  avg_fps_den;
    uint16_t  SampleDescription_width;
    uint16_t  SampleDescription_height;
} MP4D_track_t;

typedef struct
{
    MP4D_read_cb read_cb;
    void        *token;
    MP4D_track_t track[2];
    int          track_count;
} MP4D_demux_t;

int  MP4D_open(MP4D_demux_t *mp4, MP4D_read_cb read_cb, void *token, int64_t file_size);
void MP4D_close(MP4D_demux_t *mp4);
int  MP4D_get_sample(MP4D_demux_t *mp4, int track_idx, uint32_t sample_idx,
                     int64_t *data_offset, uint32_t *data_size);

#ifdef MINIMP4_IMPLEMENTATION

#define MP4D_FOURCC(a,b,c,d) \
    (((uint32_t)(a)<<24)|((uint32_t)(b)<<16)|((uint32_t)(c)<<8)|(uint32_t)(d))

static uint32_t mp4d_rb32(const uint8_t *p)
{
    return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];
}
static uint64_t mp4d_rb64(const uint8_t *p)
{
    return ((uint64_t)mp4d_rb32(p)<<32)|(uint64_t)mp4d_rb32(p+4);
}
static int mp4d_read(MP4D_demux_t *mp4, int64_t off, void *buf, size_t n)
{
    return mp4->read_cb(off, buf, n, mp4->token);
}

/*
 * Parse boxes in [off, end).
 * track_idx: which track slot we are currently filling (-1 = not inside a trak yet).
 * Returns the track_idx to use after this call (may be updated by tkhd).
 */
static void mp4d_parse(MP4D_demux_t *mp4, int64_t off, int64_t end, int track_idx);

static void mp4d_parse(MP4D_demux_t *mp4, int64_t off, int64_t end, int track_idx)
{
    uint8_t hdr[16];

    while (off < end)
    {
        uint64_t box_size;
        uint32_t box_type;
        int64_t  payload_off, payload_end;

        if (!mp4d_read(mp4, off, hdr, 8)) return;
        box_size = mp4d_rb32(hdr);
        box_type = mp4d_rb32(hdr + 4);

        if (box_size == 1)
        {
            if (!mp4d_read(mp4, off + 8, hdr + 8, 8)) return;
            box_size    = mp4d_rb64(hdr + 8);
            payload_off = off + 16;
        }
        else
        {
            payload_off = off + 8;
        }

        if (box_size == 0) box_size = (uint64_t)(end - off);
        if (box_size < 8)  { off++; continue; } /* corrupt — skip one byte */
        payload_end = off + (int64_t)box_size;

        switch (box_type)
        {
        /* ---- moov: top-level container, recurse with no track yet ---- */
        case MP4D_FOURCC('m','o','o','v'):
            mp4d_parse(mp4, payload_off, payload_end, -1);
            break;

        /* ---- trak: allocate a new track slot, recurse into it ---- */
        case MP4D_FOURCC('t','r','a','k'):
        {
            int new_idx = -1;
            if (mp4->track_count < 2)
            {
                new_idx = mp4->track_count;
                mp4->track_count++;
            }
            mp4d_parse(mp4, payload_off, payload_end, new_idx);
            break;
        }

        /* ---- simple container boxes: recurse, keeping track_idx ---- */
        case MP4D_FOURCC('m','d','i','a'):
        case MP4D_FOURCC('m','i','n','f'):
        case MP4D_FOURCC('s','t','b','l'):
        case MP4D_FOURCC('d','i','n','f'):
            mp4d_parse(mp4, payload_off, payload_end, track_idx);
            break;

        /* ---- stsd: sample description — recurse to find avc1 etc. ---- */
        case MP4D_FOURCC('s','t','s','d'):
            /* skip 8 bytes of FullBox header (version+flags+entry_count) */
            mp4d_parse(mp4, payload_off + 8, payload_end, track_idx);
            break;

        /* ---- hdlr: handler type ('vide' / 'soun') ---- */
        case MP4D_FOURCC('h','d','l','r'):
        {
            uint8_t buf[12];
            if (track_idx < 0 || track_idx >= 2) break;
            if (!mp4d_read(mp4, payload_off, buf, 12)) break;
            /* bytes 8-11 are handler_type fourCC */
            mp4->track[track_idx].handler_type = mp4d_rb32(buf + 8);
            break;
        }

        /* ---- mdhd: media timescale ---- */
        case MP4D_FOURCC('m','d','h','d'):
        {
            uint8_t buf[28];
            uint32_t ver;
            if (track_idx < 0 || track_idx >= 2) break;
            if (!mp4d_read(mp4, payload_off, buf, 28)) break;
            ver = buf[0];
            if (ver == 1)
            {
                mp4->track[track_idx].timescale = mp4d_rb32(buf + 20);
                mp4->track[track_idx].duration  = (uint32_t)mp4d_rb64(buf + 12);
            }
            else
            {
                mp4->track[track_idx].timescale = mp4d_rb32(buf + 12);
                mp4->track[track_idx].duration  = mp4d_rb32(buf + 16);
            }
            break;
        }

        /* ---- stts: sample-to-time (used for FPS) ---- */
        case MP4D_FOURCC('s','t','t','s'):
        {
            uint8_t buf[4];
            uint32_t entry_count, i;
            uint64_t total_dur = 0, total_samp = 0;
            if (track_idx < 0 || track_idx >= 2) break;
            if (!mp4d_read(mp4, payload_off + 4, buf, 4)) break;
            entry_count = mp4d_rb32(buf);
            for (i = 0; i < entry_count; i++)
            {
                uint8_t e[8];
                if (!mp4d_read(mp4, payload_off + 8 + (int64_t)i * 8, e, 8)) break;
                total_samp += mp4d_rb32(e);
                total_dur  += (uint64_t)mp4d_rb32(e) * mp4d_rb32(e + 4);
            }
            if (total_samp && total_dur)
            {
                uint32_t ts = mp4->track[track_idx].timescale;
                if (!ts) ts = 1;
                mp4->track[track_idx].avg_fps_num = (uint32_t)(total_samp * ts);
                mp4->track[track_idx].avg_fps_den = (uint32_t)total_dur;
            }
            break;
        }

        /* ---- stsz: sample sizes ---- */
        case MP4D_FOURCC('s','t','s','z'):
        {
            uint8_t buf[8];
            uint32_t fixed_size, count, i;
            if (track_idx < 0 || track_idx >= 2) break;
            if (!mp4d_read(mp4, payload_off + 4, buf, 8)) break;
            fixed_size = mp4d_rb32(buf);
            count      = mp4d_rb32(buf + 4);
            if (!count) break;
            mp4->track[track_idx].sample_count = count;
            free(mp4->track[track_idx].entry_size);
            mp4->track[track_idx].entry_size = (uint32_t*)malloc(count * 4);
            if (!mp4->track[track_idx].entry_size) break;
            if (fixed_size)
            {
                for (i = 0; i < count; i++)
                    mp4->track[track_idx].entry_size[i] = fixed_size;
            }
            else
            {
                for (i = 0; i < count; i++)
                {
                    uint8_t s[4];
                    if (!mp4d_read(mp4, payload_off + 12 + (int64_t)i * 4, s, 4)) break;
                    mp4->track[track_idx].entry_size[i] = mp4d_rb32(s);
                }
            }
            break;
        }

        /* ---- stco: chunk offsets (32-bit) ---- */
        case MP4D_FOURCC('s','t','c','o'):
        {
            uint8_t buf[4];
            uint32_t count, i;
            if (track_idx < 0 || track_idx >= 2) break;
            if (!mp4d_read(mp4, payload_off + 4, buf, 4)) break;
            count = mp4d_rb32(buf);
            if (!count) break;
            if (!mp4->track[track_idx].entry_offset)
            {
                uint32_t cap = mp4->track[track_idx].sample_count;
                if (!cap) cap = count;
                mp4->track[track_idx].entry_offset = (uint32_t*)calloc(cap, 4);
            }
            if (!mp4->track[track_idx].entry_offset) break;
            for (i = 0; i < count; i++)
            {
                uint8_t o[4];
                if (!mp4d_read(mp4, payload_off + 8 + (int64_t)i * 4, o, 4)) break;
                if (i < mp4->track[track_idx].sample_count)
                    mp4->track[track_idx].entry_offset[i] = mp4d_rb32(o);
            }
            break;
        }

        /* ---- co64: chunk offsets (64-bit) ---- */
        case MP4D_FOURCC('c','o','6','4'):
        {
            uint8_t buf[4];
            uint32_t count, i;
            if (track_idx < 0 || track_idx >= 2) break;
            if (!mp4d_read(mp4, payload_off + 4, buf, 4)) break;
            count = mp4d_rb32(buf);
            if (!count) break;
            if (!mp4->track[track_idx].entry_offset)
            {
                uint32_t cap = mp4->track[track_idx].sample_count;
                if (!cap) cap = count;
                mp4->track[track_idx].entry_offset = (uint32_t*)calloc(cap, 4);
            }
            if (!mp4->track[track_idx].entry_offset) break;
            for (i = 0; i < count; i++)
            {
                uint8_t o[8];
                if (!mp4d_read(mp4, payload_off + 8 + (int64_t)i * 8, o, 8)) break;
                if (i < mp4->track[track_idx].sample_count)
                    mp4->track[track_idx].entry_offset[i] = mp4d_rb32(o + 4);
            }
            break;
        }

        /*
         * ---- stsc: sample-to-chunk mapping ----
         *
         * MP4 stores samples in chunks. stco gives chunk offsets; stsc maps
         * chunks to samples. Without stsc, we assumed 1 sample/chunk which
         * breaks most real files. This is the most common reason for
         * "no samples" or corrupt playback.
         *
         * We expand stsc + stco into a flat per-sample offset table.
         * We do this after both stsc and stco are parsed (see MP4D_open).
         * Here we just store the raw stsc entries in a temporary buffer
         * reusing the entry_offset slot (overwritten in MP4D_open later).
         *
         * Actually, simplest correct approach: defer offset expansion to
         * MP4D_open() after full parse. Store stsc as raw bytes in dsi
         * temporarily if needed — but that's messy.
         *
         * Instead: we do the expansion inline here IF stco is already parsed,
         * otherwise store stsc entries and expand at the end of MP4D_open.
         * For simplicity, we store stsc raw in a side buffer.
         */

        /* ---- avc1 / mp4v: video sample entry ---- */
        case MP4D_FOURCC('a','v','c','1'):
        case MP4D_FOURCC('a','v','c','3'):
        case MP4D_FOURCC('m','p','4','v'):
        {
            uint8_t ve[28];
            if (track_idx < 0 || track_idx >= 2) break;
            if (!mp4d_read(mp4, payload_off, ve, 28)) break;
            /* VisualSampleEntry layout (offsets from payload start):
             *   0  reserved[6]
             *   6  data_reference_index (2)
             *   8  pre_defined/reserved (16)
             *  24  width  (2)
             *  26  height (2)
             *  28  horizresolution, vertresolution, reserved, frame_count,
             *      compressorname[32], depth, pre_defined  (=50 bytes)
             *  78  child boxes (avcC ...)
             */
            mp4->track[track_idx].SampleDescription_width  =
                (uint16_t)((ve[24] << 8) | ve[25]);
            mp4->track[track_idx].SampleDescription_height =
                (uint16_t)((ve[26] << 8) | ve[27]);
            /* Recurse into children (avcC lives here) */
            mp4d_parse(mp4, payload_off + 78, payload_end, track_idx);
            break;
        }

        /* ---- avcC: SPS+PPS AVCC blob ---- */
        case MP4D_FOURCC('a','v','c','C'):
        {
            int64_t dsz = payload_end - payload_off;
            if (track_idx < 0 || track_idx >= 2) break;
            if (dsz <= 0 || dsz > 4096) break;
            free(mp4->track[track_idx].dsi);
            mp4->track[track_idx].dsi = (uint8_t*)malloc((size_t)dsz);
            if (!mp4->track[track_idx].dsi) break;
            mp4->track[track_idx].dsi_bytes = (uint32_t)dsz;
            mp4d_read(mp4, payload_off, mp4->track[track_idx].dsi, (size_t)dsz);
            break;
        }

        default: break;
        }

        off = payload_end;
    }
}

/*
 * stsc expansion: after full parse, convert chunk offsets + stsc into
 * per-sample offsets. We re-read stsc from the file for this.
 *
 * Stored temporarily: we re-scan the file for stsc by doing a second
 * targeted parse. Actually, the cleanest fix without a large rewrite is
 * to store stsc data in a separate side-channel during parse.
 *
 * Since adding a side-channel is complex, we instead handle the common
 * case: if entry_offset was filled per-chunk by stco, expand using stsc.
 * We store raw stsc in track[i].entry_offset as a temporary and rebuild.
 *
 * SIMPLEST CORRECT APPROACH for typical MP4 files:
 * Most ffmpeg-produced MP4s use 1 sample per chunk (stsc has one entry:
 * first_chunk=1, samples_per_chunk=1, sample_desc=1). We detect this
 * and handle it. For multi-sample chunks we do full expansion.
 */

typedef struct { uint32_t first_chunk, spc, desc; } StscEntry;

static void mp4d_expand_offsets(MP4D_demux_t *mp4, int ti,
                                 StscEntry *stsc, uint32_t stsc_count,
                                 uint32_t *chunk_off, uint32_t chunk_count)
{
    MP4D_track_t *tr = &mp4->track[ti];
    uint32_t *flat;
    uint32_t si = 0, ci, ei;

    if (!tr->sample_count || !stsc_count || !chunk_count) return;

    flat = (uint32_t*)malloc(tr->sample_count * 4);
    if (!flat) return;

    for (ei = 0; ei < stsc_count && si < tr->sample_count; ei++)
    {
        uint32_t next_first = (ei + 1 < stsc_count) ? stsc[ei+1].first_chunk : chunk_count + 1;
        uint32_t spc = stsc[ei].spc;
        for (ci = stsc[ei].first_chunk - 1;
             ci < chunk_count && ci < next_first - 1 && si < tr->sample_count;
             ci++)
        {
            uint32_t s, cur_off = chunk_off[ci];
            for (s = 0; s < spc && si < tr->sample_count; s++, si++)
            {
                flat[si] = cur_off;
                cur_off += tr->entry_size ? tr->entry_size[si] : 0;
            }
        }
    }

    free(tr->entry_offset);
    tr->entry_offset = flat;
}

/* Second-pass stsc reader: re-reads the file to find stsc for track ti */
typedef struct { MP4D_demux_t *mp4; int ti; StscEntry *out; uint32_t cap; uint32_t count; } StscCtx;

/*
static void mp4d_read_stsc(MP4D_demux_t *mp4, int64_t off, int64_t end,
                            int track_idx, int target_ti,
                            StscEntry **out, uint32_t *out_count)
{
    uint8_t hdr[16];
    while (off < end)
    {
        uint64_t box_size;
        uint32_t box_type;
        int64_t  payload_off, payload_end;

        if (!mp4d_read(mp4, off, hdr, 8)) return;
        box_size = mp4d_rb32(hdr);
        box_type = mp4d_rb32(hdr + 4);
        if (box_size == 1) {
            if (!mp4d_read(mp4, off+8, hdr+8, 8)) return;
            box_size = mp4d_rb64(hdr+8);
            payload_off = off + 16;
        } else payload_off = off + 8;
        if (box_size == 0) box_size = (uint64_t)(end - off);
        if (box_size < 8) { off++; continue; }
        payload_end = off + (int64_t)box_size;

        switch (box_type) {
        case MP4D_FOURCC('m','o','o','v'):
            mp4d_read_stsc(mp4, payload_off, payload_end, -1, target_ti, out, out_count);
            break;
        case MP4D_FOURCC('t','r','a','k'):
        {
            int new_ti = -1;
            / count traks to find the right one /
            static int trak_counter = 0; / reset before call /
            new_ti = trak_counter++;
            mp4d_read_stsc(mp4, payload_off, payload_end, new_ti, target_ti, out, out_count);
            break;
        }
        case MP4D_FOURCC('m','d','i','a'):
        case MP4D_FOURCC('m','i','n','f'):
        case MP4D_FOURCC('s','t','b','l'):
            mp4d_read_stsc(mp4, payload_off, payload_end, track_idx, target_ti, out, out_count);
            break;
        case MP4D_FOURCC('s','t','s','d'):
            mp4d_read_stsc(mp4, payload_off+8, payload_end, track_idx, target_ti, out, out_count);
            break;
        case MP4D_FOURCC('s','t','s','c'):
            if (track_idx == target_ti) {
                uint8_t buf[4];
                uint32_t i, count;
                if (!mp4d_read(mp4, payload_off+4, buf, 4)) break;
                count = mp4d_rb32(buf);
                *out = (StscEntry*)malloc(count * sizeof(StscEntry));
                if (!*out) break;
                *out_count = count;
                for (i = 0; i < count; i++) {
                    uint8_t e[12];
                    if (!mp4d_read(mp4, payload_off+8+(int64_t)i*12, e, 12)) break;
                    (*out)[i].first_chunk = mp4d_rb32(e);
                    (*out)[i].spc         = mp4d_rb32(e+4);
                    (*out)[i].desc        = mp4d_rb32(e+8);
                }
            }
            break;
        default: break;
        }
        off = payload_end;
    }
}
*/

int MP4D_open(MP4D_demux_t *mp4, MP4D_read_cb read_cb, void *token, int64_t file_size)
{
    int ti;
    memset(mp4, 0, sizeof(*mp4));
    mp4->read_cb = read_cb;
    mp4->token   = token;

    /* First pass: parse everything except stsc expansion */
    mp4d_parse(mp4, 0, file_size, -1);

    /*
     * Second pass: read stsc for each track and expand chunk offsets
     * into per-sample offsets.
     *
     * stsc second-pass uses a simple re-read approach: we scan the file
     * again looking only for stsc boxes in the right trak.
     */
    for (ti = 0; ti < mp4->track_count; ti++)
    {
        MP4D_track_t *tr = &mp4->track[ti];
        StscEntry *stsc = NULL;
        uint32_t   stsc_count = 0;
        uint32_t  *chunk_offs = NULL;
        uint32_t   chunk_count = 0;
        //uint8_t    hdr2[8];
        int64_t    off2 = 0;

        if (!tr->sample_count) continue;

        /*
         * Read stsc by re-scanning file.
         * We track which trak we're in with a simple counter.
         * This is a targeted mini-scan, not a full parse.
         */
        {
            /* Simple targeted stsc scan for trak #ti */
            int64_t moov_off = -1, moov_end = -1;
            int64_t trak_off = -1, trak_end_arr[2] = {-1,-1};
            int     trak_n = 0;
            //int64_t stsc_off = -1;
            int64_t stbl_off = -1, stbl_end = -1;

            /* Find moov */
            off2 = 0;
            while (off2 < file_size) {
                uint8_t h[8]; uint64_t bs; uint32_t bt;
                if (!mp4d_read(mp4, off2, h, 8)) break;
                bs = mp4d_rb32(h); bt = mp4d_rb32(h+4);
                if (bs == 1) { uint8_t x[8]; mp4d_read(mp4, off2+8, x, 8); bs = mp4d_rb64(x); }
                if (bs == 0) break;
                if (bt == MP4D_FOURCC('m','o','o','v')) { moov_off = off2+8; moov_end = off2+(int64_t)bs; break; }
                off2 += (int64_t)bs;
            }
            if (moov_off < 0) continue;

            /* Find trak #ti inside moov */
            off2 = moov_off;
            while (off2 < moov_end) {
                uint8_t h[8]; uint64_t bs; uint32_t bt;
                if (!mp4d_read(mp4, off2, h, 8)) break;
                bs = mp4d_rb32(h); bt = mp4d_rb32(h+4);
                if (bs == 1) { uint8_t x[8]; mp4d_read(mp4, off2+8, x, 8); bs = mp4d_rb64(x); }
                if (bs < 8) break;
                if (bt == MP4D_FOURCC('t','r','a','k')) {
                    if (trak_n == ti) { trak_off = off2+8; trak_end_arr[0] = off2+(int64_t)bs; break; }
                    trak_n++;
                }
                off2 += (int64_t)bs;
            }
            if (trak_off < 0) continue;

            /* Find stbl inside trak */
            {
                /* Walk mdia → minf → stbl */
                const uint32_t containers[] = {
                    MP4D_FOURCC('m','d','i','a'),
                    MP4D_FOURCC('m','i','n','f'),
                    MP4D_FOURCC('s','t','b','l')
                };
                int64_t cur = trak_off, cur_end = trak_end_arr[0];
                int depth;
                for (depth = 0; depth < 3; depth++) {
                    int found = 0;
                    while (cur < cur_end) {
                        uint8_t h[8]; uint64_t bs; uint32_t bt;
                        if (!mp4d_read(mp4, cur, h, 8)) break;
                        bs = mp4d_rb32(h); bt = mp4d_rb32(h+4);
                        if (bs == 1) { uint8_t x[8]; mp4d_read(mp4, cur+8, x, 8); bs = mp4d_rb64(x); }
                        if (bs < 8) break;
                        if (bt == containers[depth]) {
                            cur_end = cur + (int64_t)bs;
                            cur = cur + 8;
                            if (depth == 2) { stbl_off = cur; stbl_end = cur_end; }
                            found = 1; break;
                        }
                        cur += (int64_t)bs;
                    }
                    if (!found) break;
                }
            }
            if (stbl_off < 0) continue;

            /* Find stsc and stco inside stbl */
            {
                int64_t cur = stbl_off;
                uint32_t chunk_cap = 0;
                while (cur < stbl_end) {
                    uint8_t h[8]; uint64_t bs; uint32_t bt;
                    if (!mp4d_read(mp4, cur, h, 8)) break;
                    bs = mp4d_rb32(h); bt = mp4d_rb32(h+4);
                    if (bs == 1) { uint8_t x[8]; mp4d_read(mp4, cur+8, x, 8); bs = mp4d_rb64(x); }
                    if (bs < 8) break;

                    if (bt == MP4D_FOURCC('s','t','s','c')) {
                        uint8_t buf[4]; uint32_t i, cnt;
                        mp4d_read(mp4, cur+12, buf, 4);
                        cnt = mp4d_rb32(buf);
                        stsc = (StscEntry*)malloc(cnt * sizeof(StscEntry));
                        stsc_count = cnt;
                        for (i = 0; i < cnt && stsc; i++) {
                            uint8_t e[12];
                            mp4d_read(mp4, cur+16+(int64_t)i*12, e, 12);
                            stsc[i].first_chunk = mp4d_rb32(e);
                            stsc[i].spc         = mp4d_rb32(e+4);
                            stsc[i].desc        = mp4d_rb32(e+8);
                        }
                    }
                    else if (bt == MP4D_FOURCC('s','t','c','o')) {
                        uint8_t buf[4]; uint32_t i, cnt;
                        mp4d_read(mp4, cur+12, buf, 4);
                        cnt = mp4d_rb32(buf);
                        chunk_offs = (uint32_t*)malloc(cnt * 4);
                        chunk_count = cnt;
                        chunk_cap = cnt;
                        for (i = 0; i < cnt && chunk_offs; i++) {
                            uint8_t o[4];
                            mp4d_read(mp4, cur+16+(int64_t)i*4, o, 4);
                            chunk_offs[i] = mp4d_rb32(o);
                        }
                    }
                    else if (bt == MP4D_FOURCC('c','o','6','4')) {
                        uint8_t buf[4]; uint32_t i, cnt;
                        mp4d_read(mp4, cur+12, buf, 4);
                        cnt = mp4d_rb32(buf);
                        chunk_offs = (uint32_t*)malloc(cnt * 4);
                        chunk_count = cnt;
                        chunk_cap = cnt;
                        for (i = 0; i < cnt && chunk_offs; i++) {
                            uint8_t o[8];
                            mp4d_read(mp4, cur+16+(int64_t)i*8, o, 8);
                            chunk_offs[i] = mp4d_rb32(o+4);
                        }
                    }
                    cur += (int64_t)bs;
                }
                (void)chunk_cap;
            }
        }

        /* Expand stsc + chunk_offs into per-sample offsets */
        if (stsc && chunk_offs && stsc_count && chunk_count)
            mp4d_expand_offsets(mp4, ti, stsc, stsc_count, chunk_offs, chunk_count);

        free(stsc);
        free(chunk_offs);
    }

    return (mp4->track_count > 0) ? 1 : 0;
}

void MP4D_close(MP4D_demux_t *mp4)
{
    int i;
    for (i = 0; i < 2; i++) {
        free(mp4->track[i].entry_size);
        free(mp4->track[i].entry_offset);
        free(mp4->track[i].dsi);
        mp4->track[i].entry_size   = NULL;
        mp4->track[i].entry_offset = NULL;
        mp4->track[i].dsi          = NULL;
    }
}

int MP4D_get_sample(MP4D_demux_t *mp4, int track_idx, uint32_t sample_idx,
                    int64_t *data_offset, uint32_t *data_size)
{
    MP4D_track_t *tr;
    if (track_idx < 0 || track_idx >= mp4->track_count) return 0;
    tr = &mp4->track[track_idx];
    if (!tr->entry_offset || !tr->entry_size) return 0;
    if (sample_idx >= tr->sample_count) return 0;
    *data_offset = (int64_t)tr->entry_offset[sample_idx];
    *data_size   = tr->entry_size[sample_idx];
    return 1;
}

#endif /* MINIMP4_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif
#endif /* MINIMP4_H */
