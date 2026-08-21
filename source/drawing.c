#include "drawing.h"
#include "bosses.h"
#include "text.h"
#include "save.h"
#include "video.h"

GameScreen currentScreen = LOGO;

void DrawBackground2D(u32 rgba, int l)
{
    tiny3d_SetPolygon(TINY3D_QUADS);

    tiny3d_VertexPos(-100, 0, l);
    tiny3d_VertexColor(rgba);
    tiny3d_VertexPos(1000, 0, l);
    tiny3d_VertexPos(1000, 1000, l);
    tiny3d_VertexPos(-100, 1000, l);
    tiny3d_End();
}

u32 color_from_value(int value)
{
    if (value < 0)  value = 0;
    if (value > 10) value = 10;
    int r, g, b;

    if (value >= 5)
    {
        float t = (float)(value - 5) / 5.0f;
        r = (int)(255.0f - (127.0f * t));
        g = 255;
        b = (int)(64.0f * t);
    }
    else
    {
        float t = (float)value / 5.0f;
        r = 255;
        g = (int)(255.0f * t);
        b = 0;
    }

    return (r << 24) | (g << 16) | (b << 8) | 0xff;
}

int getCaptainState(int anim_z, int anim1_z)
{
	int the_state = 0;
	switch(LEVEL)
	{
		case 0: case 1:
		{
			if(gamewin) the_state = 0+anim_z;
			else if(gamelose) the_state = 74;
			else the_state = 24;
			break;
		}
		case 2: case 3:
		{
			if(gamewin) the_state = 4+anim_z;
			else if(gamelose) the_state = 78;
			else the_state = 28;
			break;
		}
		case 4: case 5:
		{
			if(gamewin) the_state = 8+anim_z;
			else if(gamelose) the_state = 82+anim_z;
			else the_state = 32+anim_z;
			break;
		}
		case 6: case 7:
		{
			if(gamewin) the_state = 16+anim_z;
			else if(gamelose) the_state = 90;
			else the_state = 40;
			break;
		}
		case 8:
		{
			if(!boss9_cutscene) the_state = 240;
			else
			{
				if(gamewin) the_state = 96+anim_z;
				else if(gamelose) the_state = 218;
				else the_state = 241;
				//else the_state = 120+anim1_z;
			}
			break;
			/*
			switch (boss9_cutscene)
			{
				case 0: the_state = 240; break;
				case 1: the_state = 241; break;
				case 2: the_state = 242; break;
				case 3: the_state = 264; break;
				case 4: the_state = 265; break;
				case 5: the_state = 266; break;
				case 6: the_state = 216; break;
				case 7: the_state = 217; break;
				case 8: the_state = 288; break;
				case 9: the_state = 289; break;
				case 10: the_state = 290; break;
				case -1:
				{
					if(gamewin) the_state = 96+anim_z;
					else if(gamelose) the_state = 218;
					else the_state = 120+anim1_z;
					break;
				}
			}
			break;
			*/
		}
		case 9:
		{
			if(!boss10_cutscene) the_state = 224;
			else
			{
				if(gamewin) the_state = 102+anim_z;
				else if(gamelose) the_state = 176;
				else the_state = 126;
			}
			break;
		}
		case 10:
		{
			if(!boss11_cutscene) the_state = 110;
			else
			{
				if(gamewin) the_state = 106+anim_z;
				else if(gamelose) the_state = 180;
				else the_state = 130;
			}
			break;
		}
	}
	return the_state;
}

int getHatState(int hat, int anim_z)
{
	int the_state = 0;
	
	if(gamewin) the_state = 0+(!anim_z*7);
	else if(gamelose) the_state = 5;
	else the_state = 0;
	
	the_state = the_state+(hat*24);
	
	return the_state;
}

void DrawSprite2D(int id, float x, float y, int wid2, int hei2, int wid1, int hei1, int wid0, int hei0, float layer, u32 color)
{
	int cols = wid0 / wid1;
    int col = id % cols;
    int row = id / cols;
    float u0 = (float)(col * wid1) / wid0;
    float v0 = (float)(row * hei1) / hei0;
    float u1 = (float)((col + 1) * wid1) / wid0;
    float v1 = (float)((row + 1) * hei1) / hei0;
	
    tiny3d_SetPolygon(TINY3D_QUADS);

    tiny3d_VertexPos(x, y, layer);
    tiny3d_VertexColor(color);
    tiny3d_VertexTexture(u0, v0);
    tiny3d_VertexPos(x+wid2, y, layer);
    tiny3d_VertexTexture(u1, v0);
    tiny3d_VertexPos(x+wid2, y+hei2, layer);
    tiny3d_VertexTexture(u1, v1);
    tiny3d_VertexPos(x, y+hei2, layer);
    tiny3d_VertexTexture(u0, v1);

    tiny3d_End();
}

void DrawSpriteUI(float x, float y, int wid1, int hei1, int wid2, int hei2, int wid0, int hei0, int wid3, int hei3, float layer, u32 color)
{
    float u0 = (float)wid1 / (float)wid0;
    float v0 = (float)hei1 / (float)hei0;
    float u1 = (float)wid2 / (float)wid0;
    float v1 = (float)hei2 / (float)hei0;
	
    tiny3d_SetPolygon(TINY3D_QUADS);

    tiny3d_VertexPos(x, y, layer);
    tiny3d_VertexColor(color);
    tiny3d_VertexTexture(u0, v0);
    tiny3d_VertexPos(x+wid3, y, layer);
    tiny3d_VertexTexture(u1, v0);
    tiny3d_VertexPos(x+wid3, y+hei3, layer);
    tiny3d_VertexTexture(u1, v1);
    tiny3d_VertexPos(x, y+hei3, layer);
    tiny3d_VertexTexture(u0, v1);

    tiny3d_End();
}

void DrawSprite2DR(int id, float x, float y, int wid2, int hei2, int wid1, int hei1, int wid0, int hei0, float angle, float layer,  u32 color)
{
    int cols = wid0 / wid1;
    int col = id % cols;
    int row = id / cols;

    float u0 = (float)(col * wid1) / wid0;
    float v0 = (float)(row * hei1) / hei0;
    float u1 = (float)((col + 1) * wid1) / wid0;
    float v1 = (float)((row + 1) * hei1) / hei0;

    float cx = x + wid2 * 0.5f;
    float cy = y + hei2 * 0.5f;
    float hw = wid2 * 0.5f;
    float hh = hei2 * 0.5f;

    float rad = angle * (M_PI / 180.0f);
    float c = cosf(rad);
    float s = sinf(rad);

    float x0 = (-hw * c) - (-hh * s);
    float y0 = (-hw * s) + (-hh * c);
    float x1 = ( hw * c) - (-hh * s);
    float y1 = ( hw * s) + (-hh * c);
    float x2 = ( hw * c) - ( hh * s);
    float y2 = ( hw * s) + ( hh * c);
    float x3 = (-hw * c) - ( hh * s);
    float y3 = (-hw * s) + ( hh * c);

    tiny3d_SetPolygon(TINY3D_QUADS);

    tiny3d_VertexPos(cx + x0, cy + y0, layer);
    tiny3d_VertexColor(color);
    tiny3d_VertexTexture(u0, v0);
    tiny3d_VertexPos(cx + x1, cy + y1, layer);
    tiny3d_VertexTexture(u1, v0);
    tiny3d_VertexPos(cx + x2, cy + y2, layer);
    tiny3d_VertexTexture(u1, v1);
    tiny3d_VertexPos(cx + x3, cy + y3, layer);
    tiny3d_VertexTexture(u0, v1);

    tiny3d_End();
}

void SetTexture(int id)
{
	tiny3d_SetTexture(0, sprite_off[id], spritesheet[id].width, spritesheet[id].height,
		spritesheet[id].pitch, TINY3D_TEX_FORMAT_A8R8G8B8, TEXTURE_NEAREST);
}

void draw_game()
{
	SetTexture(1);
	int x1, y1 = 0;
	int x0, y0 = 0;
	int zp = the_bg/5;
	int mp = the_bg%5;
	
	int tile_bg = (4*zp)+(mp * 20);
	int grid_bg = (20*zp)+mp;
	
	u32 color = 0xffffffff;
	
    static int count_frames = 0;
    static int anim = 0;
	
	count_frames++;
	anim = (count_frames / 30) % 2;
	
	for(x0 = -99; x0 < (screen_w * 2) + 99; x0 += 64)
	{
		for(y0 = -99; y0 < (screen_h * 2) + 99; y0 += 64)
		{
			DrawSprite2D(tile_bg + anim + 2, x0, y0, 64, 64, 16, 16, 320, 320, 5, 0xffffff88);
		}
	}
	
	for(x1=0;x1<GRID_W;x1++)
	{
		for(y1=0;y1<GRID_H;y1++)
		{
			if (!board[x1][y1].active) continue;
			
			int tile = 0;
			
			if(board[x1][y1].revealed)
			{
				if(board[x1][y1].mine)
				{
					DrawSprite2D(163, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
						RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 3, color);
				}
				else
				{
					DrawSprite2D(tile_bg + anim + 2, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
						RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 4, color);
				}
				
				switch(board[x1][y1].neighbors)
				{
					case -30: tile = 79; break;
					case -28: tile = 78; break;
					case -26: tile = 77; break;
					case -24: tile = 76; break;
					case -22: tile = 59; break;
					case -20: tile = 58; break;
					case -18: tile = 57; break;
					case -16: tile = 56; break;
					case -14: tile = 39; break;
					case -12: tile = 38; break;
					case -10: tile = 37; break;
					case -8: tile = 36; break;
					case -6: tile = 19; break;
					case -4: tile = 18; break;
					case -2: tile = 17; break;
					case 0: tile = 16; break;
					case 1: tile = 152; break;
					case 2: tile = 105; break;
					case 3: tile = 153; break;
					case 4: tile = 106; break;
					case 5: tile = 154; break;
					case 6: tile = 107; break;
					case 7: tile = 155; break;
					case 8: tile = 124; break;
					case 9: tile = 172; break;
					case 10: tile = 125; break;
					case 11: tile = 173; break;
					case 12: tile = 126; break;
					case 13: tile = 174; break;
					case 14: tile = 127; break;
					case 15: tile = 175; break;
					case 16: tile = 144; break;
					case 18: tile = 145; break;
					case 20: tile = 146; break;
					case 22: tile = 147; break;
					case 24: tile = 164; break;
					case 26: tile = 165; break;
					case 28: tile = 166; break;
					case 30: tile = 167; break;
					case 32: tile = 184; break;
					case 34: tile = 185; break;
					case 36: tile = 186; break;
					case 38: tile = 187; break;
					case 40: tile = 204; break;
					case 42: tile = 205; break;
					case 44: tile = 206; break;
					case 46: tile = 207; break;
					default: tile = 104; break;
				}
				
				if(board[x1][y1].mine) tile = 114+anim;
				if(board[x1][y1].doublemine) tile = 12+anim;
				if(board[x1][y1].antimine) tile = 32+anim;
				
				int zpz = RENDER_SIZE;
				if(board[x1][y1].halfmine) zpz = RENDER_SIZE/2;
				
				if(board[x1][y1].hasNeighborMines || board[x1][y1].mine)
				{
					DrawSprite2D(tile, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
						RENDER_SIZE, zpz, 16, 16, 320, 320, 3, color);
				}
			}
			else
			{
				DrawSprite2D(tile_bg + anim, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
					RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 4, color);
				
				if(board[x1][y1].mine && gamelose == 1)
				{
					if(board[x1][y1].mine) tile = 114+anim;
					if(board[x1][y1].doublemine) tile = 12+anim;
					if(board[x1][y1].antimine) tile = 32+anim;
					
					int zpz = RENDER_SIZE;
					if(board[x1][y1].halfmine) zpz = RENDER_SIZE/2;
					
					DrawSprite2D(tile, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
						RENDER_SIZE, zpz, 16, 16, 320, 320, 3, color);
				}
				
				if(board[x1][y1].flagged>0)
				{
					if(board[x1][y1].mine == 0 && gamelose)
						DrawSprite2D(132, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
							RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 3, color);
					else if(!gamelose)
					{
						int ze_flag = 0;
						switch(board[x1][y1].flagged)
						{
							case 1: ze_flag = 72; break;
							case 2: ze_flag = 112; break;
							case 3: ze_flag = 92; break;
							case 4: ze_flag = 74; break;
						}
						
						DrawSprite2D(ze_flag + anim, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
							RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 3, color);
					}
				}
			}
			
			// if(board[x1][y1].wall_left)
				// DrawSprite2D(195 + grid_bg, offset_x + (x1 * RENDER_SIZE), offset_y + (y1 * RENDER_SIZE),
					// RENDER_SIZE / 16, RENDER_SIZE, 16, 16, 320, 320, 1, color);

			// if(board[x1][y1].wall_right)
				// DrawSprite2D(195 + grid_bg, offset_x + (x1 * RENDER_SIZE) + RENDER_SIZE, offset_y + (y1 * RENDER_SIZE),
					// RENDER_SIZE / 16, RENDER_SIZE, 16, 16, 320, 320, 1, color);

			// if(board[x1][y1].wall_up)
				// DrawSprite2D(195 + grid_bg, offset_x + (x1 * RENDER_SIZE), offset_y + (y1 * RENDER_SIZE),
					// RENDER_SIZE, RENDER_SIZE / 16, 16, 16, 320, 320, 1, color);

			// if(board[x1][y1].wall_down)
				// DrawSprite2D(195 + grid_bg, offset_x + (x1 * RENDER_SIZE), offset_y + (y1 * RENDER_SIZE) + RENDER_SIZE,
					// RENDER_SIZE, RENDER_SIZE / 16, 16, 16, 320, 320, 1, color);
			
			if(draw_grid_outlines)
			{
				DrawSprite2D(241 + grid_bg, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
					RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 2, 0xffffff88);
			}
		}
	}
	
	DrawSprite2D(240, offset_x+(cursorX*RENDER_SIZE), offset_y+(cursorY*RENDER_SIZE),
		RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, -4, color);
	
	if(LEVEL>=8 && (!boss9_cutscene && !boss10_cutscene && !boss11_cutscene && !boss12_cutscene))
	{
		int center_x = GRID_W / 2;
		int center_y = GRID_H / 2;
		
		DrawSprite2D(300, offset_x+(center_x*RENDER_SIZE), offset_y+(center_y*RENDER_SIZE),
			RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, -4, color);
	}
}

void DrawButton(int x, int y, int button, int z)
{
	u32 color = 0xffffffff;
	SetTexture(11);
	DrawSprite2D(button, x, y+4, 32, 32, 20, 20, 120, 20, z, color);
}

void drawScene()
{
	u32 color = 0xffffffff;
    tiny3d_Project2D();

    static int count_frames = 0;
    static int anim = 0;
    static int anim1 = 0;
    static int anim2 = 0;
    static int anim3 = 0;
	static int animc = 0;
	SetFontSize(28, 32);
	
	int ui_x = 4;
	
    SetFontColor(0xffffffff, 0x00000000);
    SetCurrentFont(0);
	
	if(game_loaded)
	{
		count_frames++;
		
		anim = (count_frames / 30) % 2;
		anim1 = (count_frames / 15) % 2;
		anim2 = (count_frames / 15) % 3;
		anim3 = (count_frames / 15) % 4;
		
		switch(currentScreen)
		{
			case LOGO:
			{
				static int video_frame = 0;
				PS3Video_Update(&the_video);
				
				if (vdec_picout_pending)
				{
					fflush(stdout);
					vdec_picout_pending = 0;
					PS3Video_GetFrame(&the_video, the_video.framebuffer);
					fflush(stdout);
				}
				video_frame++;
				
				if(video_frame<150) PS3Video_Draw(&the_video, 0);
				else
				{
					PS3Video_Stop(&the_video);
					currentScreen = TITLE;
					start_music = 1;
				}
				usleep(30000);
				break;
			}
			case TITLE:
			{
				SetTexture(7);
				DrawSprite2D(anim3, -40, -22, 916, 582, 1152, 960, 2304, 1920, 1, color);
				DrawFString(0, ui_x,4, 0xffffffff, 0, "%s (%s)", global_name, global_version);
				DrawFString(0, ui_x,484, 0xffffffff, 0, "%s", splash);
				DrawButton(ui_x, 128, 4, 0);
				DrawFString(0, ui_x+36,128, 0xffffffff, 0, langStr[13]);
				DrawButton(ui_x, 224, 0, 0);
				DrawFString(0, ui_x+36,224, 0xffffffff, 0, langStr[14]);
				break;
			}
			case MENU:
			{
				SetTexture(1);
				int x0, y0 = 0;
				int zp = fp_back/5;
				int mp = fp_back%5;
				animc = (count_frames / 9) % 8;
				
				int tile_bg = (4*zp)+(mp * 20);
				
				for(x0 = -99; x0 < (screen_w * 2) + 99; x0 += 32)
				{
					for(y0 = -99; y0 < (screen_h * 2) + 99; y0 += 32)
					{
						DrawSprite2D(tile_bg+anim+2, x0, y0, 32, 32, 16, 16, 320, 320, 5, 0xffffffff);
					}
				}
				
				int zfp_x = 1184;
				int zas_x = 1152;
				
				int zcs_y = 720;
				
				if(adventure_select) ze_x = (float)lerp(ze_x, -zas_x, 0.1f);
				else if(free_play) ze_x = (float)lerp(ze_x, zfp_x, 0.1f);
				else ze_x = (float)lerp(ze_x, 0, 0.1f);
				
				if(challenge_select) ze_y = (float)lerp(ze_y, -zcs_y, 0.1f);
				else if(loadout_select) ze_y = (float)lerp(ze_y, zcs_y, 0.1f);
				else ze_y = (float)lerp(ze_y, 0, 0.1f);
				
				DrawFString(0, ze_x+ui_x, ze_y+4, 0xffffffff, 0, langStr[16]);
				
				SetTexture(9);
				DrawSpriteUI(ui_x+ze_x+menu_x, ze_y+48, 512, 0, 960, 512, 1536, 1024, 224, 256, 4, color);
				DrawSpriteUI(ui_x+ze_x+menu_x+262, ze_y+48, 512, 512, 960, 1024, 1536, 1024, 224, 256, 4, color);
				DrawSpriteUI(ui_x+ze_x+menu_x+(262*2), ze_y+48, 960, 0, 960+448, 512, 1536, 1024, 224, 256, 4, color);
				DrawSpriteUI(ui_x+ze_x+menu_x+(262*3), ze_y+48, 960, 512, 960+448, 1024, 1536, 1024, 224, 256, 4, color);
				DrawFString(0, ze_x+ui_x, ze_y+320, 0xffffffff, 0, "        ^^^^^^^^");
				
				if(LEVEL>0)
				{
					DrawButton(ze_x+ui_x, ze_y+354, 0, 0);
					DrawFString(0, ze_x+ui_x+36, ze_y+354, 0xffffffff, 0, "%s   (%d-%d)", langStr[15], EPISODE, LEVEL+1);
				}
				
				DrawFString(0, ze_x+ui_x, ze_y+386, 0xffffffff, 0, "%s", menu_title[menu_pos]);
				DrawFString(52, ze_x+ui_x, ze_y+420, 0xffffffff, 0, "%s", menu_desc[menu_pos]);
				
				u32 color1 = 0xffffffff;
				u32 color2 = 0xffffffff;
				u32 color3 = 0xffffffff;
				u32 color4 = 0xffffffff;
				
				if(DIFFICULTYs==0) color1 = 0xffffffff; else color1 = 0xaaaaaaff;
				if(DIFFICULTYs==1) color2 = 0xffffffff; else color2 = 0xaaaaaaff;
				if(DIFFICULTYs==2) color3 = 0xffffffff; else color3 = 0xaaaaaaff;
				if(DIFFICULTYs==3) color4 = 0xffffffff; else color4 = 0xaaaaaaff;
				
				DrawFString(0, ze_x+zas_x, ze_y+4, 0xffffffff, 0, langStr[17]);
				SetTexture(9);
				DrawSpriteUI(ze_x+zas_x, ze_y+48, 0, 0, 512, 128, 1536, 1024, 256, 64, 4, color1);
				DrawSpriteUI(ze_x+zas_x, ze_y+116, 0, 128, 512, 256, 1536, 1024, 256, 64, 4, color2);
				DrawSpriteUI(ze_x+zas_x, ze_y+184, 0, 256, 512, 384, 1536, 1024, 256, 64, 4, color3);
				DrawSpriteUI(ze_x+zas_x, ze_y+252, 0, 384, 512, 512, 1536, 1024, 256, 64, 4, color4);
				DrawFString(0, ze_x+zas_x, ze_y+322, 0xffffffff, 0, "%s: %d", langStr[24], EPISODEs);
				
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+4, 0xffffffff, 0, langStr[27]);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+64, 0xffffffff, 0, "%s: %d", langStr[18], fp_width);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+96, 0xffffffff, 0, "%s: %d", langStr[19], fp_height);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+128, 0xffffffff, 0, "%s: %d", langStr[20], fp_mines);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+160, 0xffffffff, 0, "%s: %d", langStr[21], fp_doubles);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+192, 0xffffffff, 0, "%s: %d", langStr[22], fp_halfes);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+224, 0xffffffff, 0, "%s: %d", langStr[23], fp_antis);
				
				if(fp_music==99) DrawFString(0, ze_x+ui_x-zfp_x, ze_y+256, 0xffffffff, 0, "%s: %s", langStr[25], langStr[26]);
				else DrawFString(0, ze_x+ui_x-zfp_x, ze_y+256, 0xffffffff, 0, "%s: %d", langStr[25], fp_music);
				
				if(fp_menu==6)
				{
					DrawButton(ze_x+ui_x-zfp_x+320, ze_y+256, 3, 0);
					if(fp_music==99) DrawFString(0, ze_x+ui_x-zfp_x+320+36, ze_y+256, 0xffffffff, 0, langStr[28]);
					else DrawFString(0, ze_x+ui_x-zfp_x+320+36, ze_y+256, 0xffffffff, 0, langStr[29]);
				}
				
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+288, 0xffffffff, 0, "%s: %d", langStr[30], (fp_back+1));
				
				DrawButton(ze_x+ui_x-zfp_x, ze_y+386, 4, 0);
				DrawButton(ze_x+ui_x-zfp_x, ze_y+418, 1, 0);
				DrawButton(ze_x+ui_x-zfp_x, ze_y+450, 0, 0);
				
				DrawFString(0, ze_x+ui_x-zfp_x+36, ze_y+386, 0xffffffff, 0, langStr[31]);
				DrawFString(0, ze_x+ui_x-zfp_x+36, ze_y+418, 0xffffffff, 0, langStr[32]);
				DrawFString(0, ze_x+ui_x-zfp_x+36, ze_y+450, 0xffffffff, 0, langStr[34]);
				
				DrawFString(0, ze_x+ui_x-zfp_x+300, ze_y+64+(fp_menu*32), 0xffffffff, 0, "<");
				
				DrawFString(0, ze_x+ui_x, ze_y+zcs_y+4, 0xffffffff, 0, langStr[35]);
				DrawFString(0, ze_x+ui_x, ze_y+zcs_y+64, 0xffffffff, 0, langStr[36]);

				SetTexture(10);
				DrawSprite2D(animc, ui_x+818, 4, 32, 32, 64, 64, 512, 64, 0, 0xffffffff);
				DrawTextAligned(0, ui_x+600, ui_x+800, 4, 2, 0xffffffff, 0, 14, "%d", global_minecoins);
				break;
			}
			case MAP:
			{
				break;
			}
			case GAME:
			{
				draw_game();
				int the_timer = (int)timer_seconds;
				if(the_timer>998) the_timer=999;
				
				SetTexture(2);
				SetFontSize(84, 96);
				DrawSpriteUI(-34, -22, 0, 0, 384, 192, 1024, 1024, 302, 112, 1, color);
				DrawSpriteUI(576, -22, 0, 0, 384, 192, 1024, 1024, 302, 112, 1, color);
				DrawSpriteUI(268, -22, 384, 0, 768, 192, 1024, 1024, 308, 112, 1, color);
				DrawSpriteUI(-20, -10, 0+(anim*264), 192, 264+(anim*264), 456, 1024, 1024, 94, 94, 1, color);
				DrawSpriteUI(586, -10, 0+(anim*264), 456, 264+(anim*264), 720, 1024, 1024, 94, 94, 1, color);
				
				if(LEVEL==8)
				{
					int random_mine = RandRange(-999, 999);
					if(!boss9_cutscene) DrawFString(0, 94, -26, 0xff0000ff, 1, "???");
					else DrawFString(0, 94, -26, 0xff0000ff, 1, "%03d", random_mine);
				}
				else DrawFString(0, 94, -26, 0xff0000ff, 1, "%03d", mines_count);
				DrawFString(0, 714,-26, 0xff0000ff, 1, "%03d", the_timer);
				
				SetFontSize(28, 32);
				
				SetTexture(0);
				int capt_state = getCaptainState(anim1, anim2);
				int capt_y = -9; if(the_hat==-1) capt_y = -14;
				DrawSprite2D(capt_state, 375, capt_y, 94, 94, 40, 40, 960, 560, 0, color);
				
				if(the_hat!=-1)
				{
					SetTexture(12);
					int hat_state = getHatState(the_hat, anim1);
					DrawSprite2D(hat_state, 375, -14, 94, 94, 40, 40, 960, 320, 0, color);
				}
				//DrawFString(0, -20,32, 0xffffffff, 0, "TOTAL MINECOINS: %d", global_minecoins);
				//DrawFString(0, -20,64, 0xffffffff, 0, "MINECOINS: %d", current_minecoins);
				DrawFString(0, -20,88, 0xffffffff, 0, "FPS: %d", fps);
				
				if(global_route==1) SetTexture(4); else SetTexture(3);
				
				if(boss9_count>0 && boss9_cutscene)
				{
					int i;

					for (i = 0; i < boss9_count; i++)
					{
						if(boss9_count>0 && i>0) SetTexture(5);
						
						DrawSprite2DR(0, offset_x+(boss9_list[i].x*RENDER_SIZE)-RENDER_SIZE, offset_y+(boss9_list[i].y*RENDER_SIZE)-RENDER_SIZE,
							RENDER_SIZE*3, RENDER_SIZE*3, 384, 384, 1536, 1536, boss9_list[i].rotation, -1, color);
						DrawSprite2D(1, offset_x+(boss9_list[i].x*RENDER_SIZE)-RENDER_SIZE, offset_y+(boss9_list[i].y*RENDER_SIZE)-RENDER_SIZE,
							RENDER_SIZE*3, RENDER_SIZE*3, 384, 384, 1536, 1536, -1, color);
					}
				}
			
				if(boss10_count>0 && boss10_cutscene)
				{
					int jl = 0;
					int num = 0;
					float scale = 0.0f;
					float TMP_SIZ = 3.0f * RENDERING_PIXELS;
					int i;

					for (i = 0; i < boss10_count; i++)
					{
						boss10_list[i].pulse += 0.4f / (float)boss10_list[i].time;
						if (boss10_list[i].pulse > 6.283185f) boss10_list[i].pulse -= 6.283185f; // keep it sane mkay?
						
						if(boss10_count>0 && i>0) SetTexture(5);
						
						switch(boss10_list[i].time)
						{
							case 10: num = 222; break;
							case 9: num = 221; break;
							case 8: num = 220; break;
							case 7: num = 203; break;
							case 6: num = 202; break;
							case 5: num = 201; break;
							case 4: num = 200; break;
							case 3: num = 183; break;
							case 2: num = 182; break;
							case 1: num = 181; break;
							case 0: num = 180; break;
						}
						for(jl = 0; jl < 5; jl++)
						{
							scale = 1.0f + sinf(boss10_list[i].pulse + ((float)jl-0.67f)) * 0.1f;
							float TMP_SIZE = TMP_SIZ * 3.0f * scale;
							
							float draw_x = (boss10_list[i].x - TMP_SIZ) - (TMP_SIZE / 2.0f) + (TMP_SIZ*1.5f);
							float draw_y = (boss10_list[i].y - TMP_SIZ) - (TMP_SIZE / 2.0f) + (TMP_SIZ*1.5f);
							
							DrawSprite2DR(4, draw_x, draw_y, TMP_SIZE, TMP_SIZE, 384, 384, 1536, 1536, 36*jl, -1, color_from_value(boss10_list[i].time));
						}
						DrawSprite2D(5, boss10_list[i].x - TMP_SIZ, boss10_list[i].y - TMP_SIZ, TMP_SIZ*3, TMP_SIZ*3, 384, 384, 1536, 1536, -2, color_from_value(boss10_list[i].time));
						
						SetTexture(1);
						DrawSprite2D(num, boss10_list[i].x, boss10_list[i].y, TMP_SIZ, TMP_SIZ, 16, 16, 320, 320, -3, color);
					}
				}
				
				if(boss11_count>0 && boss11_cutscene)
				{
					int i;

					for (i = 0; i < boss11_count; i++)
					{
						u32 colorz = (0xFFFFFF00 | boss11_list[i].alpha);
						//if(boss11_count>0 && i>0) SetTexture(5);
						DrawSprite2D(4, offset_x+(boss11_list[i].x*RENDER_SIZE)-(RENDER_SIZE*0.5)-boss11_list[i].vis_x1+(RENDER_SIZE*0.022),
							offset_y+(boss11_list[i].y*RENDER_SIZE)-(RENDER_SIZE*0.5),
							RENDER_SIZE, RENDER_SIZE*2, 192, 384, 1536, 1536, -1, colorz);
						
						DrawSprite2D(5, offset_x+(boss11_list[i].x*RENDER_SIZE)-(RENDER_SIZE*0.5)+RENDER_SIZE+boss11_list[i].vis_x2-(RENDER_SIZE*0.022),
							offset_y+(boss11_list[i].y*RENDER_SIZE)-(RENDER_SIZE*0.5),
							RENDER_SIZE, RENDER_SIZE*2, 192, 384, 1536, 1536, -1, colorz);
					}
				}
				
				if(boss12_count>0 && boss12_cutscene)
				{
					float TMP_SIZ = 3.0f * RENDERING_PIXELS;
					
					if(boss12.tempo>(495-boss12.mistake) && boss12.tempo<(500-boss12.mistake))
					{
						DrawBackground2D(0xffffffff, -2);
					}
					
					DrawSprite2DR(3, boss12.x - TMP_SIZ, boss12.y - TMP_SIZ, TMP_SIZ*3, TMP_SIZ*3,
						384, 384, 1536, 1536, -boss12.rotation, -3, color);
					DrawSprite2D(7, boss12.x - TMP_SIZ, boss12.y - TMP_SIZ, TMP_SIZ*3, TMP_SIZ*3,
						384, 384, 1536, 1536, -3, color);
				}
				
				SetFontSize(28, 32);
				
				if(gamelose)
				{
					DrawButton(-20, 192, 4, -1);
					DrawFString(0, -20,160, 0xffffffff, 0, langStr[37]);
					DrawFString(0, -20+36,192, 0xffffffff, 0, langStr[15]);
				}
				else if(gamewin)
				{
					DrawFString(0, -20,160, 0xffffffff, 0, langStr[38]);
					DrawFString(0, -20,320, 0xffffffff, 0, "+ %d %s!", level_minecoins, langStr[39]);
					DrawFString(0, -20,356, 0xffffffff, 0, "%.2f %s", timer_seconds, langStr[40]);
					DrawButton(-20, 192, 4, -1);
					DrawButton(-20, 224, 5, -1);
					if(!free_play)
					{
						DrawFString(0, -20+36,192, 0xffffffff, 0, langStr[41]);
						DrawFString(0, -20+36,224, 0xffffffff, 0, langStr[42]);
					}
					else
					{
						DrawFString(0, -20+36,192, 0xffffffff, 0, langStr[15]);
						DrawFString(0, -20+36,224, 0xffffffff, 0, langStr[43]);
					}
				}
				
				break;
			}
			case ENDING:
			{
				break;
			}
			default: break;
		}
	}
	else
	{
		SetFontSize(24, 28);
		if(game_init)
		{
			DrawFString(0, 4,96,0xffffffff,0,langStr[3]);
			DrawFString(0, 4,156,0xffffffff,0,langStr[4]);
			DrawFString(0, 4,188,0xffffffff,0,langStr[5]);
			DrawFString(0, 4,252,0xff0000ff,0,langStr[6]);
			DrawFString(0, 4,284,0xff0000ff,0,langStr[7]);
			DrawFString(0, 40,348,0x00aa00ff,0,langStr[8]);
			DrawButton(4, 348, 2, 0);
		}
		else
		{
			DrawFString(0, 4,36,0xffffffff,0,langStr[0]);
			DrawFString(0, 4,96,0xffffffff,0,langStr[9]);
			DrawFString(0, 4,188,0xffffffff,0,langStr[10]);
			DrawFString(0, 4,220,0xffffffff,0,langStr[11]);
		}
	}
	
	if(save_running)
	{
		SetFontSize(28, 32);
		DrawTextAligned(0, 600, 848, 512, 2, 0xffffffff, 0, 14, langStr[1]);
	}
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
    texture_pointer = (u32 *) AddFontFromTTF((u8 *) texture_pointer, 32, 255, 64, 64, TTF_to_Bitmap);
    TTFUnloadFont();
	
    for(g = 0; g < TOTAL_SHEETS; g++)
	{
		memcpy(texture_pointer, spritesheet[g].bmp_out, spritesheet[g].pitch * spritesheet[g].height);
		sprite_off[g] = tiny3d_TextureOffset(texture_pointer);
		free(spritesheet[g].bmp_out);
		spritesheet[g].bmp_out = NULL;
		
		texture_pointer += ((spritesheet[g].pitch * spritesheet[g].height + 15) & ~15) / 4;
    }
}

void MyDrawFormatString(float x, float y, const char* format, ...) {
    char utf8_buffer[512];
    char cp1251_buffer[512];
	
    va_list args;
    va_start(args, format);
    vsnprintf(utf8_buffer, sizeof(utf8_buffer), format, args);
    va_end(args);
	
    utf8_to_cp1251(utf8_buffer, cp1251_buffer, sizeof(cp1251_buffer));
    DrawString(x, y, cp1251_buffer); 
}

void DrawFString(int wrap, float x, float y, u32 textColor, int BW, const char *fmt, ...)
{
    char buffer[1024];
	u32 shadowColor = 0x000000ff;
	
	if(BW) shadowColor = 0xffffffff;
	
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    SetFontColor(shadowColor, 0x00000000);
	
	if(wrap==0) MyDrawFormatString(x + 2, y + 2, buffer);
	else DrawWrappedText(x + 2, y + 2, 32, wrap, buffer);
	
    SetFontColor(textColor, 0x00000000);
	
	if(wrap==0) MyDrawFormatString(x, y, buffer);
	else DrawWrappedText(x, y, 32, wrap, buffer);
}

int GetFontWidth(const char *text, int fz)
{
    return strlen(text) * fz;
}

void DrawTextAligned(int wrap, float x1, float x2, float y, int align, u32 tc, int bw, int fsize, const char *text, ...)
{
    char bufferz[1024];
    va_list args;
    va_start(args, text);
    vsnprintf(bufferz, sizeof(bufferz), text, args);
    va_end(args);
	
    int width = GetFontWidth(bufferz, fsize);
    float x;

    if(align == 0) x = x1;
    else if(align == 1) x = x1 + ((x2 - x1) - width) / 2.0f;
    else x = x2 - width;

    //DrawFormatString(x, y, "%s", text);
	DrawFString(wrap, x, y, tc, bw, bufferz);
}

void zoomCheck(int zoom)
{
	if(zoom==1)
	{
		RENDERING_SIZE+=0.05f;
		if(RENDERING_SIZE>3.0f) RENDERING_SIZE=3.0f;
	}
	else
	{
		RENDERING_SIZE-=0.05f;
		if(RENDERING_SIZE<=0.25f) RENDERING_SIZE=0.25f;
	}
	
	RENDER_SIZE = RENDERING_SIZE * RENDERING_PIXELS;
	offset_x = screen_w - (RENDER_SIZE * GRID_W / 2);
	offset_y = screen_h - (RENDER_SIZE * GRID_H / 2) + ui_offset_y;
}