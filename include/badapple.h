#ifndef BADAPPLE_H
#define BADAPPLE_H

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <io/pad.h>

typedef struct __attribute__((packed))
{
    char magic[4];
    u16 width;
    u16 height;
    u16 fps;
    u32 frames;
} VideoHeader;

extern VideoHeader video_header;
extern FILE *video_fp;
extern unsigned char frame_buffer[64 * 48];

int Video_Open(const char *filename);
void Video_Close(void);
int Video_ReadFrame(u32 frame);

#endif