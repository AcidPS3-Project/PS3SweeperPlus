#include <ppu-lv2.h>
#include <math.h>
#include <time.h>
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

// IMPORT IMAGES
#include "n1_png_bin.h"
#include "n2_png_bin.h"
#include "n3_png_bin.h"
#include "n4_png_bin.h"
#include "n5_png_bin.h"
#include "n6_png_bin.h"
#include "n7_png_bin.h"
#include "n8_png_bin.h"
#include "n9_png_bin.h"
#include "n10_png_bin.h"
#include "n11_png_bin.h"
#include "n12_png_bin.h"
#include "n13_png_bin.h" //NaN
#include "n14_png_bin.h" //ep 6 boss
#include "n15_png_bin.h" //ep 7 boss (final)
#include "mine0_png_bin.h"
#include "mine1_png_bin.h" //duplicate
#include "tile00_png_bin.h"
#include "tile01_png_bin.h"
#include "bg00_png_bin.h"
#include "bg01_png_bin.h"
#include "flag0_png_bin.h"
#include "flag1_png_bin.h"
#include "grid_png_bin.h"

//IMPORT MUSIC
#include "bgm1_mp3_bin.h"
#include "bgm2_mp3_bin.h"
#include "bgm3_mp3_bin.h"
#include "bgm4_mp3_bin.h"

#include "spu_soundmodule.bin.h"

int ttf_inited = 0;

FT_Library freetype;
FT_Face face;

const unsigned char *bgm[11];
int bgm_size[11];
int current_bgm = 0;
int desired_bgm = 0;

void init_bgm()
{
	bgm[0] = bgm1_mp3_bin;
	bgm_size[0] = bgm1_mp3_bin_size;
	
	bgm[1] = bgm2_mp3_bin;
	bgm_size[1] = bgm2_mp3_bin_size;
	
	bgm[2] = bgm3_mp3_bin;
	bgm_size[2] = bgm3_mp3_bin_size;
	
	bgm[3] = bgm4_mp3_bin;
	bgm_size[3] = bgm4_mp3_bin_size;
}

#define NUM_TILES 24

pngData tile_png[NUM_TILES];
u32 tile_offset[NUM_TILES];

#define MAX_W 48
#define MAX_H 36

typedef struct
{
	u8 mine;
	u8 revealed;
	u8 flagged;
	u8 neighbors;
} Cell;

Cell board[MAX_W][MAX_H];

int GRID_W = 9;
int GRID_H = 9;
int MINES  = 10;
int RENDER_SIZE = 32;
int FIRST_CLICK = 1;

int cursorX;
int cursorY;
int gamewin = 0;
int gamelose = 0;

int screen_w = 848 / 2;
int screen_h = 512 / 2;

int offset_x;
int offset_y;

int revealed_total = 0;
int valid_total = 0;

static int fps = 0;
static int frames = 0;
static u64 last_time = 0;

void UpdateFPS()
{
    u64 now = sysGetSystemTime();

    frames++;

    if(now - last_time >= 1000000ULL)
    {
        fps = frames;
        frames = 0;
        last_time = now;
    }
}

void Initialize()
{
	u32 spu;
	sysSpuImage spu_image;
	sysSpuInitialize(6, 5);
	sysSpuRawCreate(&spu, NULL);
	sysSpuImageImport(&spu_image, spu_soundmodule_bin, 0);
	sysSpuRawImageLoad(spu, &spu_image);
	SND_Init(spu);
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
	
	offset_x = screen_w - (RENDER_SIZE * GRID_W / 2);
	offset_y = screen_h - (RENDER_SIZE * GRID_H / 2);
	valid_total = (GRID_W * GRID_H) - MINES;
}

void CalculateNumbers()
{
	int x, y;
    for(x=0;x<GRID_W;x++)
    {
        for(y=0;y<GRID_H;y++)
        {
            if(board[x][y].mine)
                continue;
			
			int ox, oy;
            int count = 0;

            for(ox=-1;ox<=1;ox++)
            {
                for(oy=-1;oy<=1;oy++)
                {
					int nx, ny;
					
                    nx = x + ox;
                    ny = y + oy;

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

void WinCheck()
{
	revealed_total = 0;
	int wx, wy;

	for (wx = 0; wx < GRID_W; wx++)
	{
		for (wy = 0; wy < GRID_H; wy++)
		{
			if (board[wx][wy].revealed) revealed_total++;
			/*
			if (!board[wx][wy].mine)
			{
				valid_total++;
				if (board[wx][wy].revealed) revealed_total++;
			}*/
		}
	}
	
	
	if(revealed_total >= valid_total)
	{
		gamewin = 1;
		StopAudio();
		int fx, fy;
		
		for (fx = 0; fx < GRID_W; fx++)
		{
			for (fy = 0; fy < GRID_H; fy++)
			{
				if (board[fx][fy].mine) board[fx][fy].flagged = 1;
			}
		}
	}
}

void Reveal(int x,int y)
{
    if(x < 0 || y < 0 || x >= GRID_W || y >= GRID_H) return;
    if(board[x][y].revealed || board[x][y].flagged || board[x][y].mine) return;

    board[x][y].revealed = 1;

    if(board[x][y].neighbors != 0) return;
	
	int ox, oy;
    for(ox=-1;ox<=1;ox++)
    {
        for(oy=-1;oy<=1;oy++)
        {
            if(ox==0 && oy==0) continue;
            Reveal(x+ox,y+oy);
        }
    }
}

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

void PlayBGM(int id)
{
    StopAudio();

    FILE *fp = (FILE *)mem_open((char *)bgm[id], bgm_size[id]);
    PlayAudiofd(fp, 0, AUDIO_INFINITE_TIME);

    current_bgm = id;
}

void Load_PNG()
{
	pngLoadFromBuffer(n1_png_bin, n1_png_bin_size, &tile_png[0]);
	pngLoadFromBuffer(n2_png_bin, n2_png_bin_size, &tile_png[1]);
	pngLoadFromBuffer(n3_png_bin, n3_png_bin_size, &tile_png[2]);
	pngLoadFromBuffer(n4_png_bin, n4_png_bin_size, &tile_png[3]);
	pngLoadFromBuffer(n5_png_bin, n5_png_bin_size, &tile_png[4]);
	pngLoadFromBuffer(n6_png_bin, n6_png_bin_size, &tile_png[5]);
	pngLoadFromBuffer(n7_png_bin, n7_png_bin_size, &tile_png[6]);
	pngLoadFromBuffer(n8_png_bin, n8_png_bin_size, &tile_png[7]);
	pngLoadFromBuffer(n9_png_bin, n9_png_bin_size, &tile_png[8]);
	pngLoadFromBuffer(n10_png_bin, n10_png_bin_size, &tile_png[9]);
	pngLoadFromBuffer(n11_png_bin, n11_png_bin_size, &tile_png[10]);
	pngLoadFromBuffer(n12_png_bin, n12_png_bin_size, &tile_png[11]);
	pngLoadFromBuffer(n13_png_bin, n13_png_bin_size, &tile_png[12]);
	pngLoadFromBuffer(n14_png_bin, n14_png_bin_size, &tile_png[13]);
	pngLoadFromBuffer(n15_png_bin, n15_png_bin_size, &tile_png[14]);
	pngLoadFromBuffer(mine0_png_bin, mine0_png_bin_size, &tile_png[15]);
	pngLoadFromBuffer(mine1_png_bin, mine1_png_bin_size, &tile_png[16]);
	pngLoadFromBuffer(tile00_png_bin, tile00_png_bin_size, &tile_png[17]);
	pngLoadFromBuffer(tile01_png_bin, tile01_png_bin_size, &tile_png[18]);
	pngLoadFromBuffer(bg00_png_bin, bg00_png_bin_size, &tile_png[19]);
	pngLoadFromBuffer(bg01_png_bin, bg01_png_bin_size, &tile_png[20]);
	pngLoadFromBuffer(flag0_png_bin, flag0_png_bin_size, &tile_png[21]);
	pngLoadFromBuffer(flag1_png_bin, flag1_png_bin_size, &tile_png[22]);
	pngLoadFromBuffer(grid_png_bin, grid_png_bin_size, &tile_png[23]);
}

void DrawRect(float x, float y, float w, float h, u32 color)
{
    tiny3d_SetPolygon(TINY3D_QUADS);

    tiny3d_VertexPos(x, y, 0);
    tiny3d_VertexColor(color);

    tiny3d_VertexPos(x+w, y, 0);
    tiny3d_VertexPos(x+w, y+h, 0);
    tiny3d_VertexPos(x, y+h, 0);

    tiny3d_End();
}

void SetTexture(int tile_id)
{
	tiny3d_SetTexture(0, tile_offset[tile_id], tile_png[tile_id].width, tile_png[tile_id].height,
		tile_png[tile_id].pitch, TINY3D_TEX_FORMAT_A8R8G8B8, TEXTURE_NEAREST);
}

void drawScene()
{
	float x, y;
	int x1, y1;
	int x0, y0;
	
	int temp = RENDER_SIZE/2;
	int temp2 = 99;
	int temp3 = 10;
	
    static int count_frames = 0;
    static int anim = 0;
    tiny3d_Project2D();

    SetFontSize(28, 32);
    
    x= 0.0; y = 0.0;
	count_frames++;
	
	anim = (count_frames / 30) % 2;
	
	for(x0 = -temp - temp2; x0 < (screen_w * 2) + temp2; x0 += temp)
	{
		for(y0 = -temp - temp2; y0 < (screen_h * 2) + temp2; y0 += temp)
		{
			SetTexture(17+anim);
			DrawSprites2D(x0, y0, (float) 0, RENDER_SIZE/2, RENDER_SIZE/2);
		}
	}
	
	for(x1=0;x1<GRID_W;x1++)
	{
		for(y1=0;y1<GRID_H;y1++)
		{
			int tile = 0;
			
			if(board[x1][y1].revealed)
			{
				SetTexture(19+anim);
				DrawSprites2D(offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE), (float) 0, RENDER_SIZE, RENDER_SIZE);
				
				switch(board[x1][y1].neighbors)
				{
					default:
					case 0: tile = 0; break;
					
					case 1: tile = 0; break;
					case 2: tile = 1; break;
					case 3: tile = 2; break;
					case 4: tile = 3; break;
					case 5: tile = 4; break;
					case 6: tile = 5; break;
					case 7: tile = 6; break;
					case 8: tile = 7; break;
				}
				
				if(board[x1][y1].mine) tile = 15+anim;
				
				if(board[x1][y1].neighbors > 0 || board[x1][y1].mine)
				{
					SetTexture(tile);
					DrawSprites2D(offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE), (float) 0, RENDER_SIZE, RENDER_SIZE);
				}
			}
			else
			{
				SetTexture(17+anim);
				DrawSprites2D(offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE), (float) 0, RENDER_SIZE, RENDER_SIZE);
				
				if(board[x1][y1].flagged)
				{
					SetTexture(21+anim);
					DrawSprites2D(offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE), (float) 0, RENDER_SIZE, RENDER_SIZE);
				}
			}
			
			if (x1 == cursorX && y1 == cursorY)
			{
				SetTexture(23);
				DrawSprites2D(offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE), (float) 0, RENDER_SIZE, RENDER_SIZE);
			}
		}
	}
	
    SetFontColor(0xffffffff, 0x00000000);
    SetCurrentFont(0);
	
	int info_x = -temp - temp3 - 3;
	int info_y = -temp - temp3 + 3;
	
    DrawFormatString(info_x,info_y, "Minesweeper Plus: PS3 Edition");
    DrawFormatString(info_x,info_y+32, "[MADE BY ACIDNT3.1]");
    DrawFormatString(info_x,info_y+64, "FRAME: %d", count_frames);
    DrawFormatString(info_x,info_y+96, "FPS: %d", fps);
    DrawFormatString(info_x,info_y+128, "TILES: %d/%d", revealed_total, valid_total);
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
	
    for(g = 0; g < NUM_TILES; g++) {
		memcpy(texture_pointer, tile_png[g].bmp_out, tile_png[g].pitch * tile_png[g].height);
		tile_offset[g] = tiny3d_TextureOffset(texture_pointer);
		
		free(tile_png[g].bmp_out);
		tile_png[g].bmp_out = NULL;
		
		texture_pointer += ((tile_png[g].pitch * tile_png[g].height + 15) & ~15) / 4;
    }
}

int main(int argc, const char* argv[])
{
	padInfo padinfo;
	int i;
	
	static padData oldpad;
	padData paddata;
	
	srand(time(NULL));
	tiny3d_Init(1024*1024);
	
	sysModuleLoad(SYSMODULE_PNGDEC);
	
	Initialize();
	init_bgm();
	
	ioPadInit(7);
    LoadTexture();
	PlayBGM(desired_bgm);
	
	GenerateBoard();
	CalculateNumbers();
	
    while(1)
    {
		sysUtilCheckCallback();
		
		UpdateFPS();
		
        tiny3d_Clear(0xff000000, TINY3D_CLEAR_ALL);
		
		tiny3d_AlphaTest(1, 0x10, TINY3D_ALPHA_FUNC_GEQUAL);

		tiny3d_BlendFunc(1, TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,
            TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | TINY3D_BLEND_FUNC_DST_ALPHA_ZERO,
            TINY3D_BLEND_RGB_FUNC_ADD | TINY3D_BLEND_ALPHA_FUNC_ADD);
		
		ioPadGetInfo(&padinfo);

		for(i = 0; i < MAX_PADS; i++){

			if(padinfo.status[i]){
				ioPadGetData(i, &paddata);
				
				if (!gamewin && !gamelose)
				{
					// remove afterwards
					if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE){
						desired_bgm++;
						if (desired_bgm>3) desired_bgm = 0;
						
						PlayBGM(desired_bgm);
					}
					
					// basic
					if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
					{
						if(board[cursorX][cursorY].mine && FIRST_CLICK)
						{
							do
							{
								GenerateBoard();
								CalculateNumbers();
							}
							while(board[cursorX][cursorY].mine);
						}
						FIRST_CLICK = 0;
						Reveal(cursorX, cursorY);
						WinCheck();
					}
					if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
					{
						board[cursorX][cursorY].flagged = !board[cursorX][cursorY].flagged;
					}
					
					// movement
					if(paddata.BTN_LEFT && !oldpad.BTN_LEFT){
						cursorX--;
						if (cursorX<0) cursorX = 0;
					}
					if(paddata.BTN_RIGHT && !oldpad.BTN_RIGHT){
						cursorX++;
						if (cursorX>GRID_W-1) cursorX = GRID_W-1;
					}
					if(paddata.BTN_UP && !oldpad.BTN_UP){
						cursorY--;
						if (cursorY<0) cursorY = 0;
					}
					if(paddata.BTN_DOWN && !oldpad.BTN_DOWN){
						cursorY++;
						if (cursorY>GRID_H-1) cursorY = GRID_H-1;
					}
				}
				oldpad = paddata;
			}
		}
		
		drawScene();
        tiny3d_Flip();
    }

    return 0;
}