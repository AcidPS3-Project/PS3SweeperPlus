#include <codec/vdec.h>
#include <ppu-asm.h>
#include <rsx/rsx.h>
#include <rsx/gcm_sys.h>
#include <ppu-types.h>
#include <sys/process.h>
#include <sys/memory.h>
#include <tiny3d.h>

#include "libraries.h"
#include "video.h"

volatile int vdec_picout_pending = 0;

static u32 PS3Video_Callback(u32 handle, u32 msgtype, u32 msgdata, u32 arg)
{
	switch (msgtype)
	{
		case VDEC_CALLBACK_AUDONE: break;
		case VDEC_CALLBACK_PICOUT:
			vdec_picout_pending++;
			printf("[VDEC] PICOUT pending=%d\n", vdec_picout_pending);
			break;
		case VDEC_CALLBACK_SEQDONE:
			printf("[VDEC] SEQDONE msgdata=%08X\n", msgdata);
			break;
		case VDEC_CALLBACK_ERROR:
			printf("[VDEC] ERROR msgdata=%08X\n", msgdata);
			break;
		default:
			printf("[VDEC] UNKNOWN CALLBACK");
			break;
	}
	
	fflush(stdout);

    return 0;
}

static int FindStartCode(const u8 *data, u32 size, u32 offset)
{
    u32 i;

    for (i = offset; i + 3 < size; i++)
    {
        if (data[i] == 0x00 && data[i + 1] == 0x00 && data[i + 2] == 0x01) return (int)i;
        if (i + 4 < size && data[i] == 0x00 && data[i + 1] == 0x00 && data[i + 2] == 0x00 && data[i + 3] == 0x01) return (int)i;
    }

    return -1;
}

static int GetNALType(const u8 *data, u32 size, u32 start)
{
    u32 header;

    if (start + 3 >= size) return -1;

    if (data[start] == 0x00 && data[start + 1] == 0x00 && data[start + 2] == 0x01)
    {
		header = start + 3;
    }
    else if (start + 4 < size && data[start] == 0x00 && data[start + 1] == 0x00 && data[start + 2] == 0x00 && data[start + 3] == 0x01)
    {
        header = start + 4;
    }
    else return -1;

    return data[header] & 0x1F;
}

int PS3Video_Init(PS3Video *video, u32 width, u32 height)
{
    vdecType type;
    vdecAttr attr;
    vdecConfig config;
    vdecClosure closure;

    static opd32 callback_opd;

    s32 ret;

    if (!video) return -1;

    memset(video, 0, sizeof(PS3Video));

    video->width = width;
    video->height = height;

    memset(&type, 0, sizeof(type));

    type.codec_type = VDEC_CODEC_TYPE_H264;
    type.profile_level = 41;

    memset(&attr, 0, sizeof(attr));

    ret = vdecQueryAttr(&type, &attr);

    if (ret != 0)
    {
        printf("[VDEC] vdecQueryAttr failed: %08X\n", ret);
        return -2;
    }

    printf("[VDEC] version %u.%u\n", attr.ver_major, attr.ver_minor);
    printf("[VDEC] memory required: %u\n", attr.mem_size);

    #define VDEC_MEM_SIZE (64 * 1024 * 1024)

    video->decoder_memory = memalign(1024 * 1024, VDEC_MEM_SIZE);

    if (!video->decoder_memory)
    {
        printf("[VDEC] Could not allocate VDEC memory\n");
        return -3;
    }

    memset(video->decoder_memory, 0, VDEC_MEM_SIZE);
    memset(&config, 0, sizeof(config));

    config.mem_addr = (u32)(uintptr_t)video->decoder_memory;
    config.mem_size = VDEC_MEM_SIZE;

    config.ppu_thread_prio = 500;
    config.ppu_thread_stack_size = 0x10000;
    config.spu_thread_prio = 250;
    config.num_spus = 2;

    memset(&closure, 0, sizeof(closure));

    closure.fn = __build_opd32(PS3Video_Callback, &callback_opd);

    closure.arg = 0;

    printf("[VDEC] CONFIG addr=%08X size=%u ppu=%u stack=%u spu=%u spus=%u\n",
        config.mem_addr, config.mem_size, config.ppu_thread_prio,
        config.ppu_thread_stack_size, config.spu_thread_prio, config.num_spus);

    printf("[VDEC] closure fn=%08X arg=%08X\n", closure.fn, closure.arg);

    ret = vdecOpen(&type, &config, &closure, &video->vdec_handle);

    if (ret != 0)
    {
        printf("[VDEC] vdecOpen failed: %08X\n", ret);

        free(video->decoder_memory);
        video->decoder_memory = NULL;

        return -4;
    }

    printf("[VDEC] vdecOpen OK, handle=%u\n", video->vdec_handle);
	
	u32 map_size;
	int i;

	video->framebuffer_size = width * height * 4;

	map_size = (video->framebuffer_size + 0xFFFFF) & ~0xFFFFF;

	for (i = 0; i < 2; i++)
	{
		video->framebuffer[i] = memalign(1024 * 1024, map_size);

		if (!video->framebuffer[i])
		{
			printf("[VDEC] Could not allocate framebuffer %d\n", i);

			if(i > 0) free(video->framebuffer[0]);

			video->framebuffer[0] = NULL;
			video->framebuffer[1] = NULL;

			return -5;
		}

		memset(video->framebuffer[i], 0, map_size);

		ret = gcmMapMainMemory(video->framebuffer[i], map_size, &video->framebuffer_rsx_offset[i]);

		if (ret != 0)
		{
			printf("[VDEC] gcmMapMainMemory failed for framebuffer %d: %08X\n", i, ret);

			free(video->framebuffer[i]);
			video->framebuffer[i] = NULL;

			if (i > 0)
			{
				free(video->framebuffer[0]);
				video->framebuffer[0] = NULL;
			}

			video->framebuffer[1] = NULL;

			return -6;
		}

		printf("[VDEC] framebuffer[%d] mapped: %08X\n", i, video->framebuffer_rsx_offset[i]);
	}

	video->framebuffer_display = 0;
	video->framebuffer_decode = 1;
	
    printf("[VDEC] Initialized successfully\n");

    return 0;
}

int PS3Video_Start(PS3Video *video)
{
    s32 ret;

    if (!video) return -1;
	
    ret = vdecStartSequence(video->vdec_handle);
    if (ret != 0) return -2;

    video->picture_ready = 0;
    video->error = 0;
    video->frame_number = 0;
    video->eof = 0;
	
    video->framebuffer_display = 0;
    video->framebuffer_decode = 1;

    return 0;
}

int PS3Video_OpenFile(PS3Video *video, const char *filename)
{
    FILE *fp;
    long size;

    if (!video || !filename) return -1;

    fp = fopen(filename, "rb");

    if (!fp)
    {
        printf("[VDEC] Could not open %s\n", filename);
        return -2;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);

    fseek(fp, 0, SEEK_SET);

    if (size <= 0)
    {
        fclose(fp);
        return -3;
    }

    video->file_data = memalign(128, (u32)size);

    if (!video->file_data)
    {
        fclose(fp);
        printf("[VDEC] Could not allocate video file\n");

        return -4;
    }

    if (fread(video->file_data, 1, (u32)size, fp) != (u32)size)
    {
        fclose(fp);

        free(video->file_data);
        video->file_data = NULL;

        return -5;
    }

    fclose(fp);
	
	video->file_size = (u32)size;
    video->file_pos = 0;

    video->au_start = 0;
    video->au_end = 0;

    video->file_open = 1;
    video->eof = 0;
	
    printf("[VDEC] Opened %s (%u bytes)\n", filename, video->file_size);

    return 0;
}

int PS3Video_Update(PS3Video *video)
{
    int start;
    int next;
    int nal_type;

    u32 au_start;
    u32 au_end;

    if (!video) return -1;
    if (!video->file_open) return -2;
    if (video->eof) return 0;

    //if (video->picture_ready) return 0;

    start = FindStartCode(video->file_data, video->file_size, video->file_pos);

    if (start < 0)
    {
        video->eof = 1;
        return 0;
    }

    nal_type = GetNALType(video->file_data, video->file_size, start);

    if (nal_type < 0)
    {
        video->eof = 1;
        return 0;
    }

    au_start = start;

    next = FindStartCode(video->file_data, video->file_size, start + 3);

    while (next >= 0)
    {
        nal_type = GetNALType(video->file_data, video->file_size, next);
        if (nal_type == 9)
        {
            au_end = next;
            if (au_end > au_start)
            {
                if(PS3Video_DecodeAU(video, video->file_data + au_start, au_end - au_start, video->frame_number, video->frame_number)  > 0)
				{
					video->error = 1;
					return -3;
				}
				
                video->file_pos = au_end;
                return 1;
            }
        }

        next = FindStartCode(video->file_data, video->file_size, next + 3);
    }

    au_end = video->file_size;

    if (au_end > au_start)
    {
        PS3Video_DecodeAU(video, video->file_data + au_start, au_end - au_start, video->frame_number, video->frame_number);
    }

    video->file_pos = video->file_size;
    video->eof = 1;
	
    return 1;
}

int PS3Video_DecodeAU(PS3Video *video, void *data, u32 size, u64 pts, u64 dts)
{
    vdecAU au;
    s32 ret;

    if (!video || !data || size == 0) return -1;

    memset(&au, 0, sizeof(au));

    au.packet_addr = (u32)(uintptr_t)data;
    au.packet_size = size;
    au.pts.low = (u32)(pts & 0xffffffff);
    au.pts.hi = (u32)(pts >> 32);
    au.dts.low = (u32)(dts & 0xffffffff);
    au.dts.hi = (u32)(dts >> 32);
    au.userdata = video->frame_number;
	
    ret = vdecDecodeAu(video->vdec_handle, VDEC_DECODER_MODE_NORMAL, &au);
	
    if (ret != 0)
    {
        printf("[VDEC] DecodeAU failed: %08X\n", ret);
        return -2;
    }

    video->frame_number++;

    return 0;
}

int PS3Video_HasFrame(PS3Video *video)
{
    if (!video) return 0;
    return video->picture_ready;
}

int PS3Video_GetFrame(PS3Video *video)
{
    vdecPictureFormat fmt;
    s32 ret;
    int decode;

    if (!video) return -1;
    decode = video->framebuffer_decode;
    if (!video->framebuffer[decode]) return -2;

    memset(&fmt, 0, sizeof(fmt));

    fmt.format_type = 0;
    fmt.color_matrix = VDEC_COLOR_MATRIX_BT709;
    fmt.alpha = 255;

    ret = vdecGetPicture(video->vdec_handle, &fmt, video->framebuffer[decode]);

    if (ret != 0) return ret;
    video->framebuffer_display = decode;
    video->framebuffer_decode ^= 1;

    return 0;
}

void PS3Video_Draw(PS3Video *video, int layer)
{
    gcmContextData *ctx;
    gcmTexture tex;

    if (!video) return;
    if (!video->framebuffer[video->framebuffer_display]) return;

    ctx = (gcmContextData *)tiny3d_Get_GCM_Context();
    if (!ctx) return;

    memset(&tex, 0, sizeof(tex));

    tex.format = GCM_TEXTURE_FORMAT_LIN | GCM_TEXTURE_FORMAT_A8R8G8B8;

    tex.mipmap = 1;
    tex.dimension = GCM_TEXTURE_DIMS_2D;
    tex.cubemap = 0;

    tex.width = video->width;
    tex.height = video->height;
    tex.depth = 1;
	
    tex.location = GCM_LOCATION_CELL;
    tex.pitch = video->width * 4;
    tex.offset = video->framebuffer_rsx_offset[video->framebuffer_display];
	
    tex.remap = (2 << 14) | (2 << 12) | (2 << 10) | (2 << 8) | (3 << 6) | (2 << 4) | (1 << 2) |(0 << 0);

    tiny3d_DoCmd_Space(64);
	rsxInvalidateTextureCache(ctx, GCM_INVALIDATE_TEXTURE);
    rsxLoadTexture(ctx, 0, &tex);
    rsxTextureControl(ctx, 0, GCM_TRUE, 0, 12 << 8, 1);
    rsxTextureFilter(ctx, 0, 0, GCM_TEXTURE_LINEAR, GCM_TEXTURE_LINEAR, GCM_TEXTURE_CONVOLUTION_QUINCUNX);
    rsxTextureWrapMode(ctx, 0, GCM_TEXTURE_CLAMP_TO_EDGE, GCM_TEXTURE_CLAMP_TO_EDGE,
        GCM_TEXTURE_CLAMP_TO_EDGE, 0, GCM_TEXTURE_ZFUNC_LESS, 0);
		
	int x0 = 0;
	int y0 = 0;
	
	int w0 = 916;
	int h0 = 582;
	
    tiny3d_SetPolygon(TINY3D_TRIANGLE_STRIP);
    tiny3d_VertexPos(x0, y0, layer);
    tiny3d_VertexTexture(0.0f, 0.0f);
    tiny3d_VertexPos(w0, y0, layer);
    tiny3d_VertexTexture(1.0f, 0.0f);
    tiny3d_VertexPos(x0, h0, layer);
    tiny3d_VertexTexture(0.0f, 1.0f);
    tiny3d_VertexPos(w0, h0, layer);
    tiny3d_VertexTexture(1.0f, 1.0f);

    tiny3d_End();
}

void PS3Video_Stop(PS3Video *video)
{
    if (!video) return;
    if (video->vdec_handle) vdecEndSequence(video->vdec_handle);
    video->picture_ready = 0;
	video->eof = 0;
}

void PS3Video_Destroy(PS3Video *video)
{
    if (!video) return;

    if (video->vdec_handle)
    {
        vdecClose(video->vdec_handle);
        video->vdec_handle = 0;
    }

    if (video->decoder_memory)
	{
		free(video->decoder_memory);
		video->decoder_memory = NULL;
	}
	
    if (video->framebuffer[0])
	{
		free(video->framebuffer[0]);
		video->framebuffer[0] = NULL;
	}

	if (video->framebuffer[1])
	{
		free(video->framebuffer[1]);
		video->framebuffer[1] = NULL;
	}
	
	if (video->file_data)
	{
		free(video->file_data);
		video->file_data = NULL;
	}

    memset(video, 0, sizeof(PS3Video));
}