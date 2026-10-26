#include "badapple.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>

VideoHeader video_header;

FILE *video_fp = NULL;

unsigned char frame_buffer[64 * 48];

int Video_Open(const char *filename)
{
    video_fp = fopen(filename, "rb");

    if(video_fp == NULL) return 0;
    fread(&video_header, sizeof(VideoHeader), 1, video_fp);
	
	printf("magic: %.4s\n", video_header.magic);
	printf("width: %u\n", video_header.width);
	printf("height: %u\n", video_header.height);
	printf("fps: %u\n", video_header.fps);
	printf("frames: %u\n", video_header.frames);
	
	printf("sizeof(VideoHeader) = %u\n", (unsigned)sizeof(VideoHeader));

    return 1;
}

void Video_Close(void)
{
    if(video_fp)
    {
        fclose(video_fp);
        video_fp = NULL;
    }
}

int Video_ReadFrame(u32 frame)
{
    if(video_fp == NULL) return 0;
    if(frame >= video_header.frames) return 0;
    fseek(video_fp, sizeof(VideoHeader) + frame * (video_header.width * video_header.height), SEEK_SET);
    fread(frame_buffer, 1, video_header.width * video_header.height, video_fp);
    return 1;
}