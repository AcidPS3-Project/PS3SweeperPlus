#include <ppu-lv2.h>
#include <math.h>
#include <time.h>
#include <sysutil/sysutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <io/pad.h>
#include <pngdec/pngdec.h>
#include <sysmodule/sysmodule.h>
#include <assert.h>
#include <unistd.h>

#include <tiny3d.h>
#include <libfont.h>

#include <ft2build.h>
#include <freetype/freetype.h> 
#include <freetype/ftglyph.h>

#include "asimov_ttf_bin.h"

#include "spu_soundlib.h"
#include "audioplayer.h"

#include "bgm1_mp3_bin.h"
#include "bg00_png_bin.h"
#include "bg01_png_bin.h"

int ttf_inited = 0;

FT_Library freetype;
FT_Face face;

#include "spu_soundmodule.bin.h"

pngData tex_tile [2];
u32 tex_tile_offset[2];

void Initialize()
{
	u32 spu;
	sysSpuImage spu_image;
	sysSpuInitialize(6, 5);
	sysSpuRawCreate(&spu, NULL);
	sysSpuImageImport(&spu_image, spu_soundmodule_bin, 0);
	sysSpuRawImageLoad(spu, &spu_image);
	SND_Init(spu);
	
	#define GRID_W 9
	#define GRID_H 9
	#define MINES 10

	struct
	{
		u8 mine;
		u8 revealed;
		u8 flagged;
		u8 neighbors;
	} Cell;
	int cursorX = 0;
	int cursorY = 0;
}

void DrawSprites2D(float x, float y, float layer, float dx, float dy)
{
    tiny3d_SetPolygon(TINY3D_QUADS);

    tiny3d_VertexPos(x     , y     , layer);
    tiny3d_VertexColor(0xffffffff);
    tiny3d_VertexTexture(0.0f, 0.0f);

    tiny3d_VertexPos(x + dx, y     , layer);
    tiny3d_VertexTexture(0.99f, 0.0f);

    tiny3d_VertexPos(x + dx, y + dy, layer);
    tiny3d_VertexTexture(0.99f, 0.99f);

    tiny3d_VertexPos(x     , y + dy, layer);
    tiny3d_VertexTexture(0.0f, 0.99f);

    tiny3d_End();
}
/*
void GenerateBoard()
{
    memset(board, 0, sizeof(board));
    int placed = 0;
    while(placed < MINES)
    {
        int x = rand() % GRID_W;
        int y = rand() % GRID_H;

        if(!board[x][y].mine)
        {
            board[x][y].mine = 1;
            placed++;
        }
    }
}

void CalculateNumbers()
{
    for(int x=0;x<GRID_W;x++)
    {
        for(int y=0;y<GRID_H;y++)
        {
            if(board[x][y].mine)
                continue;

            int count = 0;

            for(int ox=-1;ox<=1;ox++)
            {
                for(int oy=-1;oy<=1;oy++)
                {
                    int nx = x + ox;
                    int ny = y + oy;

                    if(nx < 0 || ny < 0 ||
                       nx >= GRID_W ||
                       ny >= GRID_H)
                        continue;

                    if(board[nx][ny].mine)
                        count++;
                }
            }
            board[x][y].neighbors = count;
        }
    }
}

void Reveal(int x,int y)
{
    if(x < 0 || y < 0 ||
       x >= GRID_W || y >= GRID_H)
        return;

    if(board[x][y].revealed)
        return;

    if(board[x][y].flagged)
        return;

    board[x][y].revealed = 1;

    if(board[x][y].neighbors != 0)
        return;

    for(int ox=-1;ox<=1;ox++)
    {
        for(int oy=-1;oy<=1;oy++)
        {
            if(ox==0 && oy==0)
                continue;

            Reveal(x+ox,y+oy);
        }
    }
}

for(int x=0;x<GRID_W;x++)
{
    for(int y=0;y<GRID_H;y++)
    {
        int px = x * 32;
        int py = y * 32;

        if(board[x][y].revealed)
        {
            DrawTileRevealed(px, py);

            if(board[x][y].mine)
                DrawMine(px, py);
            else
            if(board[x][y].neighbors)
                DrawNumber(px, py,
                           board[x][y].neighbors);
        }
        else
        {
            DrawTileHidden(px, py);

            if(board[x][y].flagged)
                DrawFlag(px, py);
        }
    }
}
*/

int TTFLoadFont(char * path, void * from_memory, int size_from_memory)
{
    if(!ttf_inited)
        FT_Init_FreeType(&freetype);
    ttf_inited = 1;

    if(path) {
        if(FT_New_Face(freetype, path, 0, &face)) return -1;
    } else {
        if(FT_New_Memory_Face(freetype, from_memory, size_from_memory, 0, &face)) return -1;
        }

    return 0;
}

void TTFUnloadFont()
{
   FT_Done_FreeType(freetype);
   ttf_inited = 0;
}

void TTF_to_Bitmap(u8 chr, u8 * bitmap, short *w, short *h, short *y_correction)
{
    FT_Set_Pixel_Sizes(face, (*w), (*h));
    FT_GlyphSlot slot = face->glyph;
    memset(bitmap, 0, (*w) * (*h));
    if(FT_Load_Char(face, (char) chr, FT_LOAD_RENDER )) {(*w) = 0; return;}
    int n, m, ww;

    *y_correction = (*h) - 1 - slot->bitmap_top;
    
    ww = 0;

    for(n = 0; n < slot->bitmap.rows; n++) {
        for (m = 0; m < slot->bitmap.width; m++) {

            if(m >= (*w) || n >= (*h)) continue;
            
            bitmap[m] = (u8) slot->bitmap.buffer[ww + m];
        }
    
    bitmap += *w;

    ww += slot->bitmap.width;
    }

    *w = ((slot->advance.x + 31) >> 6) + ((slot->bitmap_left < 0) ? -slot->bitmap_left : 0);
    *h = slot->bitmap.rows;
}

void startMusic()
{
	int size_samples = 48000 * 133 * 4;
	short *samples = malloc(size_samples);

	int freq;
	int is_stereo;

	DecodeAudio((void *)bgm1_mp3_bin,bgm1_mp3_bin_size,samples,&size_samples,&freq,&is_stereo);
	SND_Pause(0);
	SND_SetVoice(2,is_stereo ? VOICE_STEREO_16BIT : VOICE_MONO_16BIT,freq,0,samples,size_samples,255,255,NULL);
}

void Load_PNG()
{
    pngLoadFromBuffer(bg00_png_bin, bg00_png_bin_size, &tex_tile[0]);
    pngLoadFromBuffer(bg01_png_bin, bg01_png_bin_size, &tex_tile[1]);
}

void drawScene()
{
	float x, y;
	int i, j;
    static int count_frames =0;
    tiny3d_Project2D();

    SetFontSize(28, 32);
    
    x= 0.0; y = 0.0;
	
	count_frames++;
	
    for(i = 0; i < 9; i++) {
		for(j = 0; j < 9; j++) {
			int cur_text = count_frames;

			tiny3d_SetTexture(0, tex_tile_offset[cur_text], tex_tile[cur_text].width,
				tex_tile[cur_text].height, tex_tile[cur_text].pitch,  
				TINY3D_TEX_FORMAT_A8R8G8B8, TEXTURE_LINEAR);

			DrawSprites2D(x+(16*i), y+(16*j), (float) 0, 16, 16);
		}
    }
    if(count_frames > 1) count_frames = 0;
	
    SetCurrentFont(0);
    SetFontColor(0xffffffff, 0x0);
    x = DrawString(x,y, "Minesweeper Plus: PS3 Edition\nGame test");
}

void LoadTexture()
{
	int g;
	
    u32 * texture_mem = tiny3d_AllocTexture(64*1024*1024);
    u32 * texture_pointer;
    if(!texture_mem) return;
    texture_pointer = texture_mem;
	
    Load_PNG();
	
	ResetFont();
    TTFLoadFont(NULL, (void *) asimov_ttf_bin, asimov_ttf_bin_size);
    texture_pointer = (u32 *) AddFontFromTTF((u8 *) texture_pointer, 32, 255, 32, 32, TTF_to_Bitmap);
    TTFUnloadFont();
	
    for(g = 0; g < 2; g++) {
        tex_tile_offset[g]   = 0;
        if(tex_tile[g].bmp_out) {
            memcpy(texture_pointer, tex_tile[g].bmp_out, tex_tile[g].pitch * tex_tile[g].height);
            free(tex_tile[g].bmp_out);
            tex_tile_offset[g] = tiny3d_TextureOffset(texture_pointer);
            texture_pointer += ((tex_tile[g].pitch * tex_tile[g].height + 15) & ~15) / 4;
         }
    }
}

int main(int argc, const char* argv[])
{
	Initialize();
	tiny3d_Init(1024*1024);
	
	sysModuleLoad(SYSMODULE_PNGDEC);
	
	ioPadInit(7);
    LoadTexture();
	startMusic();
	
    while(1)
    {
		sysUtilCheckCallback();
		
        tiny3d_Clear(0xff000000, TINY3D_CLEAR_ALL);
		drawScene();
        tiny3d_Flip();
    }

    return 0;
}