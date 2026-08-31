#ifndef VIDEO_H
#define VIDEO_H

typedef struct
{
    u32 vdec_handle;

    void *decoder_memory;
    u32 decoder_memory_size;

    void *framebuffer;
    u32 framebuffer_size;
    u32 framebuffer_rsx_offset;

    u32 width;
    u32 height;

    int picture_ready;
    int error;
    int eof;
    int file_open;

    u8 *file_data;
    u32 file_size;
    u32 file_pos;

    u32 au_start;
    u32 au_end;
    u64 frame_number;
} PS3Video;

extern int vdec_picout_pending;

PS3Video the_video;

int PS3Video_Init(PS3Video *video, u32 width, u32 height);
int PS3Video_Start(PS3Video *video);
int PS3Video_OpenFile(PS3Video *video, const char *filename);
int PS3Video_Update(PS3Video *video);

int PS3Video_DecodeAU(PS3Video *video, void *data, u32 size, u64 pts, u64 dts);

int PS3Video_GetFrame(PS3Video *video, void *destination);
int PS3Video_HasFrame(PS3Video *video);
void PS3Video_Draw(PS3Video *video, int layer);

void PS3Video_Stop(PS3Video *video);
void PS3Video_Destroy(PS3Video *video);

#endif