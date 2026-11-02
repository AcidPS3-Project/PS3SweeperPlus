#include "drawing.h"
#include "bosses.h"
#include "text.h"
#include "save.h"
#include "lang.h"
#include "asimov_ttf_bin.h"

#include "level_data.h"

#include "rsx_renderer.h"

int end_time = 0;

BossTriangle boss_triangles[3] =
{
	{0.0f, 40.0f, 0xff0000cc},
	{120.0f, -20.0f, 0xff0000aa},
	{240.0f, 60.0f, 0xff0000ff}
};

#define FADE_NONE 0
#define FADE_IN   1
#define FADE_OUT  2

static const LevelSelectPos level_positions[9] = 
{
	{88 + (64 * 4), -22 + (32 * 8)},  // 1
	{88 + (64 * 5), -22 + (32 * 8)},  // 2
	{88 + (64 * 6), -22 + (32 * 8)},  // 3
	{88 + (64 * 6), -22 + (32 * 10)}, // 4
	{88 + (64 * 6), -22 + (32 * 12)}, // 5
	{88 + (64 * 5), -22 + (32 * 12)}, // 6
	{88 + (64 * 4), -22 + (32 * 12)}, // 7
	{88 + (64 * 4), -22 + (32 * 10)}, // 8
	{88 + (64 * 5), -22 + (32 * 10)}  // 9
};

static const LevelSelectPos level_positions2[10] = 
{
	{120 + (64 * 4), -22 + (32 * 14)}, // 1
	{120 + (64 * 3), -22 + (32 * 12)}, // 2
	{120 + (64 * 3), -22 + (32 * 10)}, // 3
	{120 + (64 * 4), -22 + (32 * 8)},  // 4
	{120 + (64 * 5), -22 + (32 * 8)},  // 5
	{120 + (64 * 6), -22 + (32 * 10)}, // 6
	{120 + (64 * 6), -22 + (32 * 12)}, // 7
	{120 + (64 * 5), -22 + (32 * 14)}, // 8
	{120 + (64 * 4), -22 + (32 * 12)}, // 9
	{120 + (64 * 5), -22 + (32 * 10)}  // 10
};

static const LevelSelectPos level_positions3[3] = 
{
	{88 + (64 * 5), -22 + (32 * 8)},  // 1
	{88 + (64 * 6), -22 + (32 * 12)}, // 2
	{88 + (64 * 4), -22 + (32 * 12)}  // 3
};

static int fade_mode = FADE_NONE;
static float fade_alpha = 0.0f;

static GameScreen *fade_variable = NULL;
static GameScreen fade_value;

GameScreen currentScreen = LOGO;

void DrawProjectiles()
{
    int i;

    for(i = 0; i < MAX_PROJECTILES; i++)
    {
        Projectile *p = &projectiles[i];
        if(!p->active) continue;
		
		switch(p->type)
		{
			case 0:
			{
				RSX_DrawSprite(8, p->anim, p->x - 34, p->y - 34, 96, 96, 48, 48, 0xFFFFFFFF);
				break;
			}
			case 1:
			{
				float S = RENDERING_PIXELS / 2;
				RSX_DrawSpriteUI(2, p->x - S, p->y - S, 768, 0, 858, 90, 1024, 1024, S*3, S*3, 0xFFFFFFFF);
			}
			default: break;
		}
	}
}

bool ProjectileOnCursor(float px, float py)
{
    float cx = offset_x + cursorX * RENDER_SIZE;
    float cy = offset_y + cursorY * RENDER_SIZE;

    return px >= cx && px <  cx + RENDER_SIZE && py >= cy && py <  cy + RENDER_SIZE;
}

void UpdateBossTriangles()
{
    int i;

    for(i = 0; i < 3; i++)
    {
		int rndom = rand() % 2763000;
		if(rndom == 2763) boss_triangles[i].speed = -boss_triangles[i].speed;
        boss_triangles[i].angle += boss_triangles[i].speed / 10;

        if(boss_triangles[i].angle >= 360.0f) boss_triangles[i].angle -= 360.0f;
        if(boss_triangles[i].angle < 0.0f) boss_triangles[i].angle += 360.0f;
    }
}

void DrawBossTriangles(float x, float y, float size)
{
    int i;
	int sz = size / 3;

    for(i = 0; i < 3; i++)
    {
        RSX_DrawRotatingTriangle(x, y, sz*(4-i), boss_triangles[i].angle, boss_triangles[i].color);
    }
}

void DrawFade(void)
{
    if(fade_alpha <= 0.0f) return;

    u32 alpha = (u32)fade_alpha;
    u32 color = alpha;
    color = 0x00000000 | alpha;
	
	RSX_DrawBackground2D(color);
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

int getShipRot(int rot)
{
	switch(rot)
	{
		case 0: return 156; break;
		case 1: return 158; break;
		case 2: return 176; break;
		case 3: return 178; break;
		default: return 0; break;
	}
}

int getCaptainState(int anim_z, int anim1_z, int anim2_z)
{
	int left = 318;
	int right = 518;
	int cursor = offset_x+(cursorX*RENDER_SIZE);
	
	int the_state = 0;
	if(use_custom_sweeper)
	{
		if(currentScreen != GAME) the_state = 4;
		else if(gamewin) the_state = 0+anim_z;
		else if(gamelose) the_state = 14;
		else if(paused)
		{
			if(button_start_hold) the_state=8;
			else the_state = 4;
		}
		else
		{
			if(!can_move_xd) the_state = 12+anim_z;
			else if(disable_control)
			{
				if(cursor<left) the_state = 3;
				else if(cursor>right) the_state = 2;
				else the_state = 15;
			}
			else
			{
				if(cursor<left) the_state = 5;
				else if(cursor>right) the_state = 7;
				else the_state = 6;
				
				if(button_x_hold) the_state+=4;
				else if(button_start_hold) the_state=8;
			}
		}
	}
	else switch(smiley_face)
	{
		case 0:
		{
			if(currentScreen != GAME) the_state = 24;
			else if(gamewin) the_state = 0+anim_z;
			else if(gamelose) the_state = 74;
			else if(paused)
			{
				if(button_start_hold) the_state=48;
				else the_state = 24;
			}
			else
			{
				if(!can_move_xd) the_state = 72+anim_z;
				else if(disable_control)
				{
					if(cursor<left) the_state = 3;
					else if(cursor>right) the_state = 2;
					else the_state = 75;
				}
				else
				{
					if(cursor<left) the_state = 27;
					else if(cursor>right) the_state = 25;
					else the_state = 26;
					
					if(button_x_hold) the_state+=24;
					else if(button_start_hold) the_state=48;
				}
			}
			break;
		}
		case 1:
		{
			if(currentScreen != GAME) the_state = 28;
			else if(gamewin) the_state = 4+anim_z;
			else if(gamelose) the_state = 78;
			else if(paused)
			{
				if(button_start_hold) the_state=52;
				else the_state = 28;
			}
			else
			{
				if(!can_move_xd) the_state = 76+anim_z;
				else if(disable_control)
				{
					if(cursor<left) the_state = 7;
					else if(cursor>right) the_state = 6;
					else the_state = 79;
				}
				else
				{
					if(cursor<left) the_state = 31;
					else if(cursor>right) the_state = 29;
					else the_state = 30;
					
					if(button_x_hold) the_state+=24;
					else if(button_start_hold) the_state=52;
				}
			}
			break;
		}
		case 2:
		{
			if(currentScreen != GAME) the_state = 32+anim_z;
			else if(gamewin) the_state = 8+anim_z;
			else if(gamelose) the_state = 82+anim_z;
			else if(paused)
			{
				if(button_start_hold) the_state=56+anim_z;
				else the_state = 32+anim_z;
			}
			else
			{
				if(!can_move_xd) the_state = 80+anim_z;
				else if(disable_control)
				{
					if(cursor<left) the_state = 14+anim_z;
					else if(cursor>right) the_state = 12+anim_z;
					else the_state = 86+anim_z;
				}
				else
				{
					if(cursor<left) the_state = 38+anim_z;
					else if(cursor>right) the_state = 34+anim_z;
					else the_state = 36+anim_z;
					
					if(button_x_hold) the_state+=24;
					else if(button_start_hold) the_state=56+anim_z;
				}
			}
			break;
		}
		case 3:
		{
			if(currentScreen != GAME) the_state = 40;
			else if(gamewin) the_state = 16+anim_z;
			else if(gamelose) the_state = 90;
			else if(paused)
			{
				if(button_start_hold) the_state=64;
				else the_state = 40;
			}
			else
			{
				if(!can_move_xd) the_state = 88+anim_z;
				else if(disable_control)
				{
					if(cursor<left) the_state = 19;
					else if(cursor>right) the_state = 18;
					else the_state = 91;
				}
				else
				{
					if(cursor<left) the_state = 43;
					else if(cursor>right) the_state = 41;
					else the_state = 42;
					
					if(button_x_hold) the_state+=24;
					else if(button_start_hold) the_state=64;
				}
			}
			break;
		}
		case 4:
		{
			if(currentScreen != GAME) the_state = 120+anim1_z;
			else if(enable_9 && boss9_cutscene==0) the_state = 240;
			else
			{
				if(gamewin) the_state = 96+anim_z;
				else if(gamelose) the_state = 218;
				else if(paused)
				{
					if(button_start_hold) the_state=168+anim1_z;
					else the_state = 120+anim1_z;
				}
				else
				{
					if(!can_move_xd) the_state = 216+anim_z;
					else if(disable_control)
					{
						if(cursor<left) the_state = 147+anim1_z;
						else if(cursor>right) the_state = 123+anim1_z;
						else the_state = 144+anim1_z;
						
						the_state+=48;
					}
					else
					{
						if(cursor<left) the_state = 147+anim1_z;
						else if(cursor>right) the_state = 123+anim1_z;
						else the_state = 144+anim1_z;
						
						if(button_x_hold) the_state+=48;
						else if(button_start_hold) the_state=168+anim1_z;
					}
				}
				//else the_state = 120+anim1_z;
			}
			break;
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
		}
		case 5:
		{
			if(currentScreen != GAME) the_state = 126;
			else if(enable_10 && boss10_cutscene==0) the_state = 224;
			else
			{
				if(gamewin) the_state = 102+anim_z;
				else if(gamelose) the_state = 176;
				else if(paused)
				{
					if(button_start_hold) the_state=150;
					else the_state = 126;
				}
				else
				{
					if(!can_move_xd) the_state = 174+anim_z;
					else if(disable_control)
					{
						if(cursor<left) the_state = 105;
						else if(cursor>right) the_state = 104;
						else the_state = 177;
					}
					else
					{
						if(cursor<left) the_state = 129;
						else if(cursor>right) the_state = 127;
						else the_state = 128;
						
						if(button_x_hold) the_state+=24;
						else if(button_start_hold) the_state=150;
					}
				}
			}
			break;
		}
		case 6:
		{
			if(currentScreen != GAME) the_state = 130;
			else if(enable_11 && boss11_cutscene==0) the_state = 110;
			else
			{
				if(gamewin) the_state = 106+anim_z;
				else if(gamelose) the_state = 180;
				else if(paused)
				{
					if(button_start_hold) the_state=154;
					else the_state = 130;
				}
				else
				{
					if(!can_move_xd)
					{
						if(anim2_z<2) the_state = 178+anim2_z;
						else the_state = 178+24+anim2_z;
					}
					else if(disable_control)
					{
						if(cursor<left) the_state = 109;
						else if(cursor>right) the_state = 107;
						else the_state = 181;
					}
					else
					{
						if(cursor<left) the_state = 133;
						else if(cursor>right) the_state = 131;
						else the_state = 132;
						
						if(button_x_hold) the_state+=24;
						else if(button_start_hold) the_state=154;
					}
				}
			}
			break;
		}
	}
	return the_state;
}

int getHatState(int hat, int anim_z)
{
	int the_state = 0;
	int left = 318;
	int right = 518;
	int cursor = offset_x+(cursorX*RENDER_SIZE);
	
	if(currentScreen != GAME) the_state = 0;
	else if(gamewin) the_state = 0+anim_z;
	else if(gamelose) the_state = 5;
	else if(paused) the_state = 0;
	else
	{
		if(!can_move_xd) the_state = 5+anim_z;
		else
		{
			if(smiley_face==4)
			{
				if(cursor<left) the_state = 2;
				else if(cursor>right) the_state = 4;
				else the_state = 3;
			}
			else
			{
				if(cursor<left) the_state = 4;
				else if(cursor>right) the_state = 2;
				else the_state = 3;
			}
			
			if(button_start_hold) the_state=0;
		}
	}
	
	the_state = the_state+(hat*24);
	
	return the_state;
}

void DrawLevelIcon(int level, int x, int y, int anim, int tile_bg)
{
    int tile = tile_map_num(level);

    if(LEVEL < level)
    {
        RSX_DrawSprite(1, tile_bg + anim + 2, x, y, 32, 32, 16, 16, 0xffffffff);
        RSX_DrawSprite(1, tile, x, y, 32, 32, 16, 16, 0xffffffff);
    }
    else RSX_DrawSprite(1, 112 + anim, x, y, 32, 32, 16, 16, 0xffffffff);
}

void draw_game()
{
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
	
	for(x0 = -40; x0 < (screen_w * 2) + 99; x0 += 64)
	{
		for(y0 = -22; y0 < (screen_h * 2) + 99; y0 += 64)
		{
			RSX_DrawSprite(1, tile_bg + anim + 2, x0, y0, 64, 64, 16, 16, 0xffffff88);
		}
	}
	
	for(x1=0;x1<GRID_W;x1++)
	{
		for(y1=0;y1<GRID_H;y1++)
		{
			if(board[x1][y1].ice_center)
			{
				int tz = 858;
				if(anim==1) tz = 858+54;
				RSX_DrawSpriteUI(2, offset_x+(x1*RENDER_SIZE)-RENDER_SIZE, offset_y+(y1*RENDER_SIZE)-RENDER_SIZE,
					tz, 0, tz+54, 54, 1024, 1024, RENDER_SIZE*3, RENDER_SIZE*3, color);
			}
			
			if (!board[x1][y1].active) continue;
			
			int tile = 0;
			
			if(board[x1][y1].revealed)
			{
				if(board[x1][y1].mine) RSX_DrawSprite(1, 163, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE), RENDER_SIZE, RENDER_SIZE, 16, 16, color);
				else RSX_DrawSprite(1, tile_bg + anim + 2, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE), RENDER_SIZE, RENDER_SIZE, 16, 16, color);
				
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
				if(board[x1][y1].hasNeighborMines || board[x1][y1].mine) RSX_DrawSprite(1, tile, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE), RENDER_SIZE, zpz, 16, 16, color);
			}
			else
			{
				RSX_DrawSprite(1, tile_bg + anim, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE), RENDER_SIZE, RENDER_SIZE, 16, 16, color);
				if(board[x1][y1].mine && gamelose == 1)
				{
					if(board[x1][y1].mine) tile = 114+anim;
					if(board[x1][y1].doublemine) tile = 12+anim;
					if(board[x1][y1].antimine) tile = 32+anim;
					
					int zpz = RENDER_SIZE;
					if(board[x1][y1].halfmine) zpz = RENDER_SIZE/2;
					
					RSX_DrawSprite(1, tile, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE), RENDER_SIZE, zpz, 16, 16, color);
				}
				
				if(board[x1][y1].flagged>0)
				{
					if(board[x1][y1].mine == 0 && gamelose) RSX_DrawSprite(1, 132, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE), RENDER_SIZE, RENDER_SIZE, 16, 16, color);
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
						
						RSX_DrawSprite(1, ze_flag + anim, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE), RENDER_SIZE, RENDER_SIZE, 16, 16, color);
					}
				}
			}
			
			if(draw_grid_outlines) RSX_DrawSprite(1, 241 + grid_bg, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE), RENDER_SIZE, RENDER_SIZE, 16, 16, 0xffffff88);
		}
	}
	
	RSX_DrawSprite(1, 240, offset_x+(cursorX*RENDER_SIZE), offset_y+(cursorY*RENDER_SIZE), RENDER_SIZE, RENDER_SIZE, 16, 16, color);
	
	if(LEVEL>=8 && (!boss9_cutscene && !boss10_cutscene && !boss11_cutscene && !boss12_cutscene))
	{
		int center_x = GRID_W / 2;
		int center_y = GRID_H / 2;
		
		RSX_DrawSprite(1, 300, offset_x+(center_x*RENDER_SIZE), offset_y+(center_y*RENDER_SIZE), RENDER_SIZE, RENDER_SIZE, 16, 16, color);
	}
}

void DrawButton(int x, int y, int button, int z)
{
	u32 color = 0xffffffff;
	RSX_DrawSprite(11, button, x, y+4, 32, 32, 20, 20, color);
}

int tile_map_num(int _t)
{
	int tile = 0;
	switch(_t)
	{
		case 1: tile = 105; break;
		case 2: tile = 106; break;
		case 3: tile = 107; break;
		case 4: tile = 124; break;
		case 5: tile = 125; break;
		case 6: tile = 126; break;
		case 7: tile = 127; break;
		case 8: tile = 144; break;
		case 9: tile = 145; break;
		case 10: tile = 146; break;
		case 11: tile = 147; break;
		case 12: tile = 164; break;
		case 13: tile = 165; break;
		case 14: tile = 166; break;
		case 15: tile = 167; break;
		case 16: tile = 184; break;
		case 17: tile = 185; break;
		case 18: tile = 186; break;
		case 19: tile = 187; break;
		case 20: tile = 204; break;
		case 21: tile = 205; break;
		case 22: tile = 206; break;
		case 23: tile = 207; break;
		default: tile = 104; break;
	}
	return tile;
}

int zez_y = -22;
int zex_y = -22;

void drawScene()
{
	u32 color = 0xffffffff;

    static int count_frames = 0;
    static int anim = 0;
    static int anim1 = 0;
    static int anim2 = 0;
    static int anim3 = 0;
	static int animc = 0;
	SetFontSize(28, 32);
	
	int ui_x = 4;
	
    SetFontColor(0xffffffff, 0x00000000);
    //SetCurrentFont(0);
	
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
				SetFontSize(28, 32);
				DrawFString(0, 0, 0, 0xffffffff, 0, 32, "AcidNT3.1 Productions LOGO TEST");
				break;
			}
			case TITLE:
			{
				RSX_DrawSprite(7, anim3, -40, -22, 916, 582, 1152, 960, color);
				if(settings_select)
				{
					DrawFString(0, ui_x,4, 0xffffffff, 0, 32, langStr[44]);
					DrawButton(ui_x, 64, PAD_L2, 0);
					switch(language)
					{
						case 0: DrawFString(0, ui_x+36, 64, 0xffffffff, 0, 32, "Language: English"); break;
						case 1: DrawFString(0, ui_x+36, 64, 0xffffffff, 0, 32, "Язык: Русский"); break;
						case 2: DrawFString(0, ui_x+36, 64, 0xffffffff, 0, 32, "Тілі: Қазақ"); break;
					}
					DrawButton(ui_x, 96, PAD_R2, 0);
					switch(setting_guarantee)
					{
						case 0:
							DrawFString(0, ui_x+36, 96, 0xffffffff, 0, 32, "%s: %s", langStr[2], langStr[3]);
							break;
						case 1:
							DrawFString(0, ui_x+36, 96, 0xffffffff, 0, 32, "%s: %s", langStr[2], langStr[45]);
							break;
						case 2:
							DrawFString(0, ui_x+36, 96, 0xffffffff, 0, 32, "%s: %s", langStr[2], langStr[46]);
							break;
					}
					DrawButton(ui_x, 484, PAD_CIRCLE, 0);
					DrawFString(0, ui_x+36, 484, 0xffffffff, 0, 32, langStr[32]);
				}
				else
				{
					DrawFString(0, ui_x,4, 0xffffffff, 0, 32, "%s (%s)", global_name, global_version);
					DrawFString(0, ui_x,484, 0xffffffff, 0, 32, "%s", splash);
					DrawButton(ui_x, 128, PAD_START, 0);
					DrawFString(0, ui_x+36,128, 0xffffffff, 0, 32, langStr[13]);
					DrawButton(ui_x, 160, PAD_SQUARE, 0);
					DrawFString(0, ui_x+36,160, 0xffffffff, 0, 32, langStr[44]);
					DrawButton(ui_x, 224, PAD_TRIANGLE, 0);
					DrawFString(0, ui_x+36,224, 0xffffffff, 0, 32, langStr[14]);
				}
				break;
			}
			case MENU:
			{
				int x0, y0 = 0;
				int zp = fp_back/5;
				int mp = fp_back%5;
				animc = (count_frames / 9) % 8;
				
				int tile_bg = (4*zp)+(mp * 20);
				
				for(x0 = -40; x0 < (screen_w * 2) + 99; x0 += 32)
				{
					for(y0 = -22; y0 < (screen_h * 2) + 99; y0 += 32)
					{
						RSX_DrawSprite(1, tile_bg+anim+2, x0, y0, 32, 32, 16, 16, 0xffffffff);
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
				else if(boss_options) ze_y = (float)lerp(ze_y, -zcs_y, 0.1f);
				else ze_y = (float)lerp(ze_y, 0, 0.1f);
				
				DrawFString(0, ze_x+ui_x, ze_y+4, 0xffffffff, 0, 32, langStr[16]);
				
				RSX_DrawSpriteUI(9, ui_x+ze_x+menu_x, ze_y+48, 512, 0, 960, 512, 1536, 1024, 224, 256, color);
				RSX_DrawSpriteUI(9, ui_x+ze_x+menu_x+262, ze_y+48, 512, 512, 960, 1024, 1536, 1024, 224, 256, color);
				RSX_DrawSpriteUI(9, ui_x+ze_x+menu_x+(262*2), ze_y+48, 960, 0, 960+448, 512, 1536, 1024, 224, 256, color);
				RSX_DrawSpriteUI(9, ui_x+ze_x+menu_x+(262*3), ze_y+48, 960, 512, 960+448, 1024, 1536, 1024, 224, 256, color);
				DrawFString(0, ze_x+ui_x, ze_y+320, 0xffffffff, 0, 32, "        ^^^^^^^^");
				
				if(LEVEL>0)
				{
					DrawButton(ze_x+ui_x, ze_y+354, PAD_TRIANGLE, 0);
					DrawFString(0, ze_x+ui_x+36, ze_y+354, 0xffffffff, 0, 32, "%s   (%d-%d)", langStr[15], EPISODE, LEVEL+1);
				}
				
				DrawFString(0, ze_x+ui_x, ze_y+386, 0xffffffff, 0, 32, "%s", menu_title[menu_pos]);
				DrawFString(52, ze_x+ui_x, ze_y+420, 0xffffffff, 0, 32, "%s", menu_desc[menu_pos]);
				
				u32 color1 = 0xffffffff;
				u32 color2 = 0xffffffff;
				u32 color3 = 0xffffffff;
				u32 color4 = 0xffffffff;
				
				if(DIFFICULTYs==0) color1 = 0xffffffff; else color1 = 0xaaaaaaff;
				if(DIFFICULTYs==1) color2 = 0xffffffff; else color2 = 0xaaaaaaff;
				if(DIFFICULTYs==2) color3 = 0xffffffff; else color3 = 0xaaaaaaff;
				if(DIFFICULTYs==3) color4 = 0xffffffff; else color4 = 0xaaaaaaff;
				
				DrawFString(0, ze_x+zas_x, ze_y+4, 0xffffffff, 0, 32, langStr[17]);
				RSX_DrawSpriteUI(9, ze_x+zas_x, ze_y+48, 0, 0, 512, 128, 1536, 1024, 256, 64, color1);
				RSX_DrawSpriteUI(9, ze_x+zas_x, ze_y+116, 0, 128, 512, 256, 1536, 1024, 256, 64, color2);
				RSX_DrawSpriteUI(9, ze_x+zas_x, ze_y+184, 0, 256, 512, 384, 1536, 1024, 256, 64, color3);
				RSX_DrawSpriteUI(9, ze_x+zas_x, ze_y+252, 0, 384, 512, 512, 1536, 1024, 256, 64, color4);
				DrawFString(0, ze_x+zas_x, ze_y+322, 0xffffffff, 0,  32,"%s: %d", langStr[24], EPISODEs);
				
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+4, 0xffffffff, 0, 32, langStr[27]);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+64, 0xffffffff, 0, 32, "%s: %d", langStr[18], fp_width);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+96, 0xffffffff, 0, 32, "%s: %d", langStr[19], fp_height);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+128, 0xffffffff, 0, 32, "%s: %d", langStr[20], fp_mines);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+160, 0xffffffff, 0, 32, "%s: %d", langStr[21], fp_doubles);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+192, 0xffffffff, 0, 32, "%s: %d", langStr[22], fp_halfes);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+224, 0xffffffff, 0, 32, "%s: %d", langStr[23], fp_antis);
				
				if(fp_music==99) DrawFString(0, ze_x+ui_x-zfp_x, ze_y+256, 0xffffffff, 0, 32, "%s: %s", langStr[25], langStr[26]);
				else DrawFString(0, ze_x+ui_x-zfp_x, ze_y+256, 0xffffffff, 0, 32, "%s: %d", langStr[25], fp_music);
				
				if(fp_menu==6)
				{
					DrawButton(ze_x+ui_x-zfp_x+320, ze_y+256, PAD_SQUARE, 0);
					if(fp_music==99) DrawFString(0, ze_x+ui_x-zfp_x+320+36, ze_y+256, 0xffffffff, 0, 32, langStr[28]);
					else DrawFString(0, ze_x+ui_x-zfp_x+320+36, ze_y+256, 0xffffffff, 0, 32, langStr[29]);
				}
				
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+288, 0xffffffff, 0, 32, "%s: %d", langStr[30], (fp_back+1));
				
				DrawButton(ze_x+ui_x-zfp_x, ze_y+386, PAD_START, 0);
				DrawButton(ze_x+ui_x-zfp_x, ze_y+418, PAD_CIRCLE, 0);
				DrawButton(ze_x+ui_x-zfp_x, ze_y+450, PAD_TRIANGLE, 0);
				DrawButton(ze_x+ui_x-zfp_x, ze_y+482, PAD_L1, 0);
				DrawButton(ze_x+ui_x-zfp_x, ze_y+514, PAD_SQUARE, 0);
				
				DrawFString(0, ze_x+ui_x-zfp_x+36, ze_y+386, 0xffffffff, 0, 32, langStr[31]);
				DrawFString(0, ze_x+ui_x-zfp_x+36, ze_y+418, 0xffffffff, 0, 32, langStr[32]);
				DrawFString(0, ze_x+ui_x-zfp_x+36, ze_y+450, 0xffffffff, 0, 32, langStr[34]);
				
				if(use_custom_sweeper) DrawFString(0, ze_x+ui_x-zfp_x+36, ze_y+482, 0xffffffff, 0, 32, "%s: V", langStr[54]);
				else DrawFString(0, ze_x+ui_x-zfp_x+36, ze_y+482, 0xffffffff, 0, 32, "%s: X", langStr[54]);
				
				DrawFString(0, ze_x+ui_x-zfp_x+36, ze_y+514, 0xffffffff, 0, 32, langStr[130]);
				
				DrawFString(0, ze_x+ui_x-zfp_x+300, ze_y+64+(fp_menu*32), 0xffffffff, 0, 32, "<");
				
				DrawFString(0, ze_x+ui_x-zfp_x+36, ze_y+zcs_y+4, 0xffffffff, 0, 32, langStr[130]);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+zcs_y+64, 0xffffffff, 0, 32, "%s 9: %s", langStr[131], enable_9 ? langStr[132] : langStr[133]);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+zcs_y+96, 0xffffffff, 0, 32, "%s 10: %s", langStr[131], enable_10 ? langStr[132] : langStr[133]);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+zcs_y+128, 0xffffffff, 0, 32, "%s 11: %s", langStr[131], enable_11 ? langStr[132] : langStr[133]);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+zcs_y+160, 0xffffffff, 0, 32, "%s XII: %s", langStr[131], enable_12 ? langStr[132] : langStr[133]);
				DrawFString(0, ze_x+ui_x-zfp_x, ze_y+zcs_y+192, 0xffffffff, 0, 32, "%s NaN: %s", langStr[131], enable_nan ? langStr[132] : langStr[133]);
				
				DrawFString(0, ze_x+ui_x-zfp_x+300, ze_y+zcs_y+64+(boss_menu*32), 0xffffffff, 0, 32, "<");
				
				int temperz = 0;
				if(global_route==1) temperz=8;
				
				RSX_DrawSprite(20, boss_menu+temperz, ze_x+ui_x-zfp_x+580, ze_y+zcs_y+256+boss_render_x, 256, 256, 256, 256, color);
				
				DrawFString(0, ze_x+ui_x, ze_y+zcs_y+4, 0xffffffff, 0, 32, langStr[35]);
				DrawFString(0, ze_x+ui_x, ze_y+zcs_y+64, 0xffffffff, 0, 32, langStr[36]);
				
				DrawButton(ze_x+ui_x, ze_y+zcs_y+450, PAD_L2, 0);
				DrawButton(ze_x+ui_x, ze_y+zcs_y+482, PAD_R2, 0);
				
				DrawFString(0, ze_x+ui_x+36, ze_y+zcs_y+450, 0xffffffff, 0, 32, langStr[33]);
				DrawFString(0, ze_x+ui_x+36, ze_y+zcs_y+482, 0xffffffff, 0, 32, langStr[58]);
				
				RSX_DrawSpriteUI(2, 262, -22+ze_y-zcs_y+256, 384, 0, 768, 192, 1024, 1024, 314, 112, color);
				
				int capt_state = getCaptainState(anim1, anim2, anim3);
				int capt_y = -9; if(the_hat==-1) capt_y = -14;
				
				int capt_p = 40;
				
				if(use_custom_sweeper)
				{
					capt_y = -14;
					capt_p = spritesheet[13].width / 4;
				}
				
				RSX_DrawSprite(0, capt_state, 372, capt_y+ze_y-zcs_y+256, 94, 94, capt_p, capt_p, color);
				
				int testpp = 24;
				if(master_modeS) testpp = 0;
				RSX_DrawSprite(0, testpp, -40, -22, 40, 40, 40, 40, color);
				DrawButton(0, -22, PAD_SELECT, -1);
				
				if(the_hat!=-1)
				{
					int hat_state = getHatState(the_hat, anim1);
					RSX_DrawSprite(12, hat_state, 372, -14+ze_y-zcs_y+256, 94, 94, 40, 40, color);
				}

				RSX_DrawSprite(10, animc, ui_x+818, 4, 32, 32, 64, 64, 0xffffffff);
				DrawTextAligned(0, ui_x+580, ui_x+780, 4, 2, 0xffffffff, 0, 14, 32, "%d", global_minecoins);
				break;
			}
			case MAP:
			{
				DifficultyData *lvl = &episodes[EPISODE-1].levels[LEVEL].difficulties[DIFFICULTY];
				int max_lvl = episodes[EPISODE-1].level_count;
				
				int bg_map = lvl->back;
				if(LEVEL!=(max_lvl-1))
				{
					switch(LEVEL)
					{
						case 8: bg_map = 4; break;
						case 9: bg_map = 9; break;
						default: break;
					}
				}
				
				if(EPISODE==7)
				{
					switch(LEVEL)
					{
						case 0: bg_map = 4; break;
						case 1: bg_map = 9; break;
						case 2: bg_map = 14; break;
						default: break;
					}
				}
				
				int zp = bg_map/5;
				int mp = bg_map%5;
				
				int tile_bg = (4*zp)+(mp * 20);
				int grid_bg = (20*zp)+mp;
				
				static int ep1_x = 0;
				
				int name_test = 0;
				
				u32 c_t = 0xff0000ff;
				
				SetFontSize(28, 32);
				
				if(EPISODE==1)
				{
					if(global_route==1) name_test = 111;
					else name_test = 61;
				}
				else if(EPISODE==2)
				{
					if(global_route==1) name_test = 120;
					else name_test = 80;
				}
				
				if(ten_arena<=160*4)
				{
					for(int x0 = -40; x0 < (screen_w * 2) + 99; x0 += 32)
					{
						for(int y0 = -22; y0 < (screen_h * 2) + 99; y0 += 32)
						{
							RSX_DrawSprite(1, tile_bg+anim, x0, y0, 32, 32, 16, 16, 0xffffffff);
						}
					}
					switch(EPISODE)
					{
						case 1:
						{
							if(ship_x > 876.0f)
							{
								c_t = 0xff0000ff;
								ep1_x = lerp(ep1_x, -956, 0.025f);
								RSX_DrawRectangle2D(876+ep1_x, -22, 2763, 1000, 0x000000ff);
								DrawBossTriangles((876/2)+ep1_x+876+12, 560/2, 200);
								UpdateBossTriangles();
							}
							else if(ship_x < -80.0f && LEVEL>0)
							{
								c_t = 0x0000ffff;
								ep1_x = lerp(ep1_x, 1132, 0.025f);
								RSX_DrawRectangle2D(-40, -22, ep1_x, 1000, 0x000000ff);
								static float ang = 0.0f;
								ang += 1.8f;
								RSX_DrawSpriteUIR(2, -1388+ep1_x+(876/2), (560/2) - 256, 768, 90, 768+208, 298, 1024, 1024, 512, 512, ang, 0xffffff67);
							}
							
							for(int _t = 1; _t < max_lvl; _t++)
							{
								if(LEVEL<_t)
								{
									int tile = tile_map_num(_t);
									RSX_DrawSprite(1, tile_bg+anim+2, 120+(64*_t)+ep1_x, -22+(32*11), 32, 32, 16, 16, 0xffffffff);
									RSX_DrawSprite(1, tile, 120+(64*_t), -22+(32*11)+ep1_x, 32, 32, 16, 16, 0xffffffff);
								}
								else RSX_DrawSprite(1, 112+anim, 120+(64*_t)+ep1_x, -22+(32*11), 32, 32, 16, 16, 0xffffffff);
								RSX_DrawSprite(1, 241 + grid_bg, 120+(64*_t)+ep1_x, -22+(32*11), 32, 32, 16, 16, 0xffffffff);
							}
							break;
						}
						case 2:
						{
							for(int b = 0; b < 9; b++)
							{
								DrawLevelIcon(b + 1, level_positions[b].x, level_positions[b].y, anim, tile_bg);
								
								int b1 = b % 3;
								int b2 = b / 3;
								RSX_DrawSprite(1, 241 + grid_bg, 88+(64*(4+b2)), -22+(64*(4+b1)), 32, 32, 16, 16, 0xffffffff);
							}
							
							break;
						}
						case 3:
						{
							for(int b = 0; b < 10; b++)
							{
								DrawLevelIcon(b + 1, level_positions2[b].x, level_positions2[b].y, anim, tile_bg);
								
								int b1 = 0;
								int b2 = 0;
								
								switch(b)
								{
									case 0: b1 = 0; b2 = 1; break;
									case 1: b1 = 0; b2 = 2; break;
									case 2: b1 = 1; b2 = 0; break;
									case 3: b1 = 1; b2 = 2; break;
									case 4: b1 = 1; b2 = 3; break;
									case 5: b1 = 2; b2 = 0; break;
									case 6: b1 = 2; b2 = 1; break;
									case 7: b1 = 2; b2 = 3; break;
									case 8: b1 = 3; b2 = 1; break;
									case 9: b1 = 3; b2 = 2; break;
								}
								
								RSX_DrawSprite(1, 241 + grid_bg, 120+(64*(3+b2)), -22+(64*(4+b1)), 32, 32, 16, 16, 0xffffffff);
							}
							
							break;
						}
						case 4:
						{
							for(int b = 0; b < 3; b++)
							{
								DrawLevelIcon(b + 1, level_positions3[b].x, level_positions3[b].y, anim, tile_bg);
								RSX_DrawSprite(1, 241 + grid_bg, level_positions3[b].x, level_positions3[b].y, 32, 32, 16, 16, 0xffffffff);
							}
							
							break;
						}
						case 7:
						{
							for(int _t = 1; _t < max_lvl+1; _t++)
							{
								if(LEVEL<_t)
								{
									int tile = tile_map_num(_t);
									RSX_DrawSprite(1, tile_bg+anim+2, 152+(64*_t)+ep1_x, -22+(32*11), 32, 32, 16, 16, 0xffffffff);
									RSX_DrawSprite(1, tile, 152+(64*_t), -22+(32*11)+ep1_x, 32, 32, 16, 16, 0xffffffff);
								}
								else RSX_DrawSprite(1, 112+anim, 152+(64*_t)+ep1_x, -22+(32*11), 32, 32, 16, 16, 0xffffffff);
								RSX_DrawSprite(1, 241 + grid_bg, 152+(64*_t)+ep1_x, -22+(32*11), 32, 32, 16, 16, 0xffffffff);
							}
							break;
						}
						default: break;
					}
				}
				
				RSX_DrawSprite(1, getShipRot(ship_r)+anim1, ship_x+ep1_x, ship_y, 32, 32, 16, 16, 0xffffffff);
				
				SetFontSize(28, 32);
				if(!ship_a)
				{
					if(EPISODE==1 && LEVEL==8) DrawTextAligned(0, 0, 876, -22+(32*4), 1, c_t, 1, 28, 32, langStr[name_test+LEVEL]);
					else DrawTextAligned(0, 0, 876, -22+(32*4), 1, 0xffffffff, 0, 28, 32, langStr[name_test+LEVEL]);
				}
				
				break;
			}
			case GAME:
			{
				draw_game();
				int the_timer = (int)timer_seconds;
				
				if(enable_9 && boss9_cutscene!=0) the_timer = (int)time_left;
				
				if(the_timer>998) the_timer=999;
				if(the_timer<0) the_timer=0;
				
				SetFontSize(84, 96);
				RSX_DrawSpriteUI(2, -40, -22, 0, 0, 384, 192, 1024, 1024, 302, 112, color);
				RSX_DrawSpriteUI(2, 576, -22, 0, 0, 384, 192, 1024, 1024, 302, 112, color);
				RSX_DrawSpriteUI(2, 262, -22, 384, 0, 768, 192, 1024, 1024, 314, 112, color);
				RSX_DrawSpriteUI(2, -20, -10, 0+(anim*264), 192, 264+(anim*264), 456, 1024, 1024, 94, 94, color);
				RSX_DrawSpriteUI(2, 586, -10, 0+(anim*264), 456, 264+(anim*264), 720, 1024, 1024, 94, 94, color);
				
				if(enable_9 || enable_12)
				{
					int random_mine = RandRange(-999, 999);
					if(!boss9_cutscene) DrawFString(0, 94, -26, 0xff0000ff, 1, 96, "???");
					else DrawFString(0, 94, -26, 0xff0000ff, 1, 96, "%03d", random_mine);
				}
				else DrawFString(0, 94, -26, 0xff0000ff, 1, 96, "%03d", mines_count);
				
				int livez = 0;
				if(free_play) livez = fp_lifes; else livez = LIFES;
				bool enable_bosses = enable_9 || enable_10 || enable_11 || enable_12;
				if(enable_bosses) livez = RandRange(-9, 9);
				DrawFString(0, 498, -26, 0xff0000ff, 1, 96, "%d", livez);
				
				if(enable_12 && !enable_9)
				{
					int random_time = RandRange(-999, 999);
					if(!boss12_cutscene) DrawFString(0, 714, -26, 0xff0000ff, 1, 96, "000");
					else DrawFString(0, 714, -26, 0xff0000ff, 1, 96, "%03d", random_time);
				}
				else DrawFString(0, 714,-26, 0xff0000ff, 1, 96, "%03d", the_timer);
				
				SetFontSize(28, 32);
				
				int capt_state = getCaptainState(anim1, anim2, anim3);
				int capt_y = -9; if(the_hat==-1) capt_y = -14;
				
				int capt_p = 40;
				if(use_custom_sweeper)
				{
					capt_y = -14;
					capt_p = spritesheet[13].width / 4;
					// SetTexture(13);
				}
				
				RSX_DrawSprite(0, capt_state, 372, capt_y, 94, 94, capt_p, capt_p, color);
				
				if(the_hat!=-1)
				{
					int hat_state = getHatState(the_hat, anim1);
					RSX_DrawSprite(12, hat_state, 372, -14, 94, 94, 40, 40, color);
				}
				// DrawFString(0, -20,32, 0xffffffff, 0, 32, "TOTAL MINECOINS: %d", global_minecoins);
				// DrawFString(0, -20,64, 0xffffffff, 0, 32, "MINECOINS: %d", current_minecoins);
				// DrawFString(0, -20,88, 0xffffffff, 0, 32, "FPS: %d", fps);
				
				int tex_boss = 0;
				if(global_route==1) tex_boss = 4; else tex_boss = 3;
				
				if(boss9_count>0 && boss9_cutscene && enable_9)
				{
					int i;

					for (i = 0; i < boss9_count; i++)
					{
						// if(boss9_count>0 && i>0) SetTexture(5);
						
						RSX_DrawSprite2DR(tex_boss, 0, offset_x+(boss9_list[i].x*RENDER_SIZE)-RENDER_SIZE, offset_y+(boss9_list[i].y*RENDER_SIZE)-RENDER_SIZE,
							RENDER_SIZE*3, RENDER_SIZE*3, 384, 384, 1536, 1536, boss9_list[i].rotation, color);
						RSX_DrawSprite(tex_boss, 1, offset_x+(boss9_list[i].x*RENDER_SIZE)-RENDER_SIZE, offset_y+(boss9_list[i].y*RENDER_SIZE)-RENDER_SIZE, RENDER_SIZE*3, RENDER_SIZE*3, 384, 384, color);
					}
				}
			
				if(boss10_count>0 && boss10_cutscene && enable_10)
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
						
						// if(boss10_count>0 && i>0) SetTexture(5);
						
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
							
							RSX_DrawSprite2DR(tex_boss, 4, draw_x, draw_y, TMP_SIZE, TMP_SIZE, 384, 384, 1536, 1536, 36*jl, color_from_value(boss10_list[i].time));
						}
						RSX_DrawSprite(tex_boss, 5, boss10_list[i].x - TMP_SIZ, boss10_list[i].y - TMP_SIZ, TMP_SIZ*3, TMP_SIZ*3, 384, 384, color_from_value(boss10_list[i].time));
						RSX_DrawSprite(1, num, boss10_list[i].x, boss10_list[i].y, TMP_SIZ, TMP_SIZ, 16, 16, color);
					}
				}
				
				if(boss11_count>0 && boss11_cutscene && enable_11)
				{
					int i;

					for (i = 0; i < boss11_count; i++)
					{
						u32 colorz = (0xFFFFFF00 | boss11_list[i].alpha);
						RSX_DrawSprite(tex_boss, 4, offset_x+(boss11_list[i].x*RENDER_SIZE)-(RENDER_SIZE*0.5)-boss11_list[i].vis_x1+(RENDER_SIZE*0.022),
							offset_y+(boss11_list[i].y*RENDER_SIZE)-(RENDER_SIZE*0.5), RENDER_SIZE, RENDER_SIZE*2, 192, 384, colorz);
						
						RSX_DrawSprite(tex_boss, 5, offset_x+(boss11_list[i].x*RENDER_SIZE)-(RENDER_SIZE*0.5)+RENDER_SIZE+boss11_list[i].vis_x2-(RENDER_SIZE*0.022),
							offset_y+(boss11_list[i].y*RENDER_SIZE)-(RENDER_SIZE*0.5), RENDER_SIZE, RENDER_SIZE*2, 192, 384, colorz);
					}
				}
				
				if(boss12_count>0 && boss12_cutscene && enable_12)
				{
					float TMP_SIZ = 3.0f * RENDERING_PIXELS;
					
					//if(boss12.tempo>(495-boss12.mistake) && boss12.tempo<(500-boss12.mistake)) DrawBackground2D(0xffffffff, -2);
					
					RSX_DrawSprite2DR(tex_boss, 3, boss12.x - TMP_SIZ, boss12.y - TMP_SIZ, TMP_SIZ*3, TMP_SIZ*3, 384, 384, 1536, 1536, -boss12.rotation, color);
					RSX_DrawSprite(tex_boss, 7, boss12.x - TMP_SIZ, boss12.y - TMP_SIZ, TMP_SIZ*3, TMP_SIZ*3, 384, 384, color);
				}
				
				if(bossnan_count>0 && bossnan_cutscene && enable_nan)
				{
					int i;

					for (i = 0; i < bossnan_count; i++)
					{
						//if(bossnan_count>0 && i>0) SetTexture(5);
						
						bossnan_list[i].pos_x = offset_x+(bossnan_list[i].x*RENDER_SIZE)-RENDER_SIZE;
						bossnan_list[i].pos_y = offset_y+(bossnan_list[i].y*RENDER_SIZE)-RENDER_SIZE;
						
						float t_x = 0.0f;
						float t_y = 0.0f;
						
						t_x = (float)lerp(t_x, bossnan_list[i].pos_x, 0.1);
						t_y = (float)lerp(t_y, bossnan_list[i].pos_y, 0.1);
						RSX_DrawSprite(tex_boss, 6, t_x, t_y, RENDER_SIZE*4, RENDER_SIZE*4, 384, 384, color);
					}
				}
				
				SetFontSize(28, 32);
				
				if(gamelose)
				{
					DrawButton(-20, 192, PAD_START, -1);
					DrawFString(0, -20,160, 0xffffffff, 0, 32, langStr[37]);
					DrawFString(0, -20+36,192, 0xffffffff, 0, 32, langStr[15]);
				}
				else if(gamewin)
				{
					DrawFString(0, -20,160, 0xffffffff, 0, 32, langStr[38]);
					DrawFString(0, -20,320, 0xffffffff, 0, 32, "+ %d %s!", level_minecoins, langStr[39]);
					if(!(LEVEL==11 || (free_play && fp_music == 14))) DrawFString(0, -20,356, 0xffffffff, 0, 32, "%.2f %s", timer_seconds, langStr[40]);
					DrawButton(-20, 192, PAD_START, -1);
					DrawButton(-20, 224, PAD_SELECT, -1);
					if(!free_play)
					{
						DrawFString(0, -20+36,192, 0xffffffff, 0, 32, langStr[41]);
						DrawFString(0, -20+36,224, 0xffffffff, 0, 32, langStr[42]);
					}
					else
					{
						DrawFString(0, -20+36,192, 0xffffffff, 0, 32, langStr[15]);
						DrawFString(0, -20+36,224, 0xffffffff, 0, 32, langStr[43]);
					}
				}
				else if(paused)
				{
					DrawFString(0, -20,160, 0xffffffff, 0, 32, langStr[57]);
					DrawButton(-20, 192, PAD_START, -1);
					DrawButton(-20, 224, PAD_SELECT, -1);
					DrawFString(0, -20+36,192, 0xffffffff, 0, 32, langStr[15]);
					DrawFString(0, -20+36,224, 0xffffffff, 0, 32, langStr[42]);
				}
				
				if(show_warning)
				{
					bool something = false;
					int temper = 0;
					int temper2 = 0;
					int tempz = 106;
					
					if(enable_9){temper = 74; temper2 = 103;}
					if(enable_10){temper = 75; temper2 = 104;}
					if(enable_11){temper = 76; temper2 = 105;}
					
					RSX_DrawRectangle2D(-40+100, -22+100, 876-100, 560-100, 0xaaaaaaff);
					
					SetFontSize(16, 18);
					if(global_route==1) DrawFString(tempz, -40+100, -22+100, 0x000000ff, 1, 18, "%s\n\n%s\n%s", langStr[temper], langStr[106], langStr[temper2]);
					else DrawFString(tempz, -40+100, -22+100, 0x000000ff, 1, 18, langStr[temper]);
					
					SetFontSize(28, 32);
					DrawTextAligned(0, 0, 876, 492, 1, 0x000000ff, 1, 28, 32, langStr[107]);
					
					if(enable_9 && !seen_nine) something = true;
					else if(enable_9 && global_route==1 && !seen_fnine) something = true;
					else if(enable_10 && !seen_ten) something = true;
					else if(enable_10 && global_route==1 && !seen_nten) something = true;
					else if(enable_11 && !seen_eleven) something = true;
					else if(enable_11 && global_route==1 && !seen_meleven) something = true;
					else if(enable_12 && !seen_twelve) something = true;
					else if(enable_nan && !seen_nan) something = true;
					else something = false;
					
					if(something) DrawTextAligned(0, 0, 876, 464, 1, 0x000000ff, 1, 28, 32, langStr[108]);
				}
				
				break;
			}
			case ENDING:
			{
				if(end_time>0)
				{
					if(end_time>1) zez_y = -22; else zez_y = 96;
					zex_y = lerp(zex_y, zez_y, 0.05f);
					
					RSX_DrawSprite(15+EPISODE, anim3, -40, -22, 916, 582, 1152, 960, color);
					
					RSX_DrawRectangle2D(-40, -22, 916, zex_y, 0x000000ff);
					RSX_DrawRectangle2D(-40, 562-zex_y, 916, 582, 0x000000ff);
					
					SetFontSize(28, 32);
					if(end_time<3)
					{
						DrawTextAligned(0, -40, 876, -84+zex_y, 1, 0xff0000ff, 1, 28, 32, langStr[109]);
						DrawTextAligned(0, -40, 876, 588-zex_y, 1, 0xff0000ff, 1, 28, 32, langStr[110]);
					}
					else
					{
						int c = end_time - 3;
						int zs_x = 20;
						int max_lvl = episodes[EPISODE-1].level_count;
						int name_test = 0;
						static int test_x = 1000;
						
						if(EPISODE==1) name_test = 60;
						else if(EPISODE==2) name_test = 79;
						
						for(int d = 0; d < c; d++)
						{
							int b = c-d;
							int a = b-max_lvl;
							
							int icon = tile_map_num(b);
							
							if(a==1) icon = 35; else if(a==2) icon = 133; else if(a==3) icon = 114;
							
							RSX_DrawSprite(1, icon, zs_x - 32, 512-(32*d)+4, 32, 32, 16, 16, color);
							
							if(b>max_lvl)
							{
								if(a == 1) DrawFString(0, zs_x, 512-(32*d), 0xffffffff, 0, 32, "%s: %.2f", langStr[77], time_total);
								if(a == 2) DrawFString(0, zs_x, 512-(32*d), 0xffffffff, 0, 32, "%s: %d", langStr[78], current_minecoins);
								if(a == 3) DrawFString(0, zs_x, 512-(32*d), 0xffffffff, 0, 32, "%s: %d", langStr[79], mistakes);
							}
							else DrawFString(0, zs_x, 512-(32*d), 0xffffffff, 0, 32, "%s: %.2f", langStr[name_test+b], level_time[b-1]);
						}
						
						if(show_shit)
						{
							DrawTextAligned(0, -40, 876, 32, 1, 0xff0000ff, 0, 28, 32, "%s:", langStr[9]);
							int rank_letter = 0;
							test_x = lerp(test_x, 512, 0.05f);
							
							switch(rank_result)
							{
								case 0: rank_letter = 307; break; // Failure
								case 1: rank_letter = 306; break; // Embarasing
								case 2: rank_letter = 287; break; // Dull
								case 3: rank_letter = 286; break; // Cool
								case 4: rank_letter = 267; break; // Bold
								case 5: rank_letter = 266; break; // Awesome
								case 6: rank_letter = 247; break; // Super
								case 7: rank_letter = 246; break; // Master
							}
							
							RSX_DrawSprite(1, rank_letter, 512+64, 20, 64, 64, 16, 16, color);
							
							DrawFString(0, test_x, 448, 0xff0000ff, 32, 1, langStr[10]);
							DrawFString(0, test_x, 480, 0xff0000ff, 32, 1, langStr[11]);
						}
					}
				}
				else
				{
					zex_y = 96;
					zez_y = 96;
				}
				
				break;
			}
			case GOTOSHOP:
			{
				for(int x0 = -40; x0 < (screen_w * 2) + 99; x0 += 32)
				{
					for(int y0 = -22; y0 < (screen_h * 2) + 99; y0 += 32)
					{
						RSX_DrawSprite(1, anim, x0, y0, 32, 32, 16, 16, color);
					}
				}
				RSX_DrawSprite(1, 192+anim2, -40+(32*16), -22+(32*6), 32, 32, 16, 16, color);
				RSX_DrawSprite(1, getShipRot(ship_r)+anim1, ship_x, ship_y, 32, 32, 16, 16, color);
				
				break;
			}
			case SHOP:
			{
				static int randomizer = 0;
				static int randomizer1 = 0;
				static int randomizer2 = 0;
				static int shopkeeper_updater = 0;
				static int shopkeeper_updater2 = 600;
				static int shopkeeper_quote = 90;
				
				if(fade_mode != FADE_OUT)
				{
					shopkeeper_updater++;
					if(shopkeeper_updater>=shopkeeper_updater2)
					{
						shopkeeper_updater2 = RandRange(500, 700);
						shopkeeper_updater = 0;
						int test = rand() % 10;
						shopkeeper_quote = 90+test;
					}
					
					if((rand() % 333) == 67 && randomizer != 4) randomizer = 4;
					
					if(randomizer==4 && randomizer1 == 0)
					{
						randomizer1 = 1;
						randomizer2 = RandRange(21, 43);
					}
					
					if(randomizer1>0)
					{
						randomizer1++;
						if(randomizer1>randomizer2)
						{
							randomizer1 = 0;
							randomizer = 0;
						}
					}
				}
				else shopkeeper_quote = 101;
				
				RSX_DrawSprite(19, 0, -40, -22, 916, 582, 640, 360, color);
				
				for(int p = 0; p < 7; p++)
				{
					RSX_DrawSprite(12, p*24, 32+(p*112), 64, 80, 80, 40, 40, color);
				}
				
				RSX_DrawSprite(15, 0+randomizer, 684, 432, 78*2, 49*2, 78, 49, color);
				
				DrawFString(52, 32, 420, 0xffffffff, 0, 32, langStr[shopkeeper_quote]);
				break;
			}
			case CREDITS:
			{
				int th = 24;
				if(specialthanks)
				{
					SetFontSize(28, 32);
					DrawFString(0, ui_x, 4, 0xffffffff, 0, 32, langStr[56]);
					SetFontSize(20, 24);
					
					char crediter[2048];
					sprintf(crediter, "%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",
						credits[2], credits[3], credits[10], credits[11], credits[12], credits[13], credits[14],
						credits[15], credits[16], credits[17], credits[18], credits[19], credits[20], credits[21],
						credits[22], credits[23]
					);
					DrawFString(75, ui_x, 40, 0xffffffff, 24, 0, crediter);
				}
				else
				{
					SetFontSize(28, 32);
					DrawFString(0, ui_x, 4, 0xffffffff, 0, 32, langStr[14]);
					SetFontSize(20, 24);
					DrawFString(0, ui_x, 40, 0xffffffff, 0, 24, "%s - %s", langStr[52], credits[0]);
					DrawFString(0, ui_x, 40+th, 0xffffffff, 0, 24, "%s - %s", langStr[55], credits[1]);
					DrawFString(0, ui_x, 40+(th*2), 0xffffffff, 0, 24, "%s - %s", langStr[59], credits[4]);
					DrawFString(0, ui_x, 40+(th*3), 0xffffffff, 0, 24, "%s - %s", langStr[60], credits[5]);
					DrawFString(0, ui_x, 40+(th*4), 0xffffffff, 0, 24, "%s - %s", langStr[70], credits[6]);
					DrawFString(0, ui_x, 40+(th*5), 0xffffffff, 0, 24, "%s - %s", langStr[71], credits[7]);
					DrawFString(0, ui_x, 40+(th*6), 0xffffffff, 0, 24, "%s - %s", langStr[72], credits[8]);
					DrawFString(0, ui_x, 40+(th*7), 0xffffffff, 0, 24, "%s - %s", langStr[73], credits[9]);
					//DrawFString(0, ui_x, 40+(th*8), 0xffffffff, 0, 24, "%s - %s", langStr[74], credits[10]);
				}
				
				SetFontSize(28, 32);
				DrawButton(ui_x, 484, PAD_CIRCLE, 0);
				DrawFString(0, ui_x + 36, 484, 0xffffffff, 0, 32, "%s         < - %s          > - %s", langStr[32], langStr[14], langStr[56]);
				
				if(easter_egg)
				{
					RSX_DrawSprite(15, 8+anim3, 684, 432, 78*2, 49*2, 78, 49, 0xffffffff);
				}
				break;
			}
			case ERROR_SCREEN:
			{
				SetFontSize(28, 32);
				DrawFString(0, 0, 0, 0xffffffff, 0, 32, "HALT! HALT! STOP EVERYTHING!!!!");
				DrawFString(52, 0, 156, 0xffffffff, 0, 32, "Report this error to AcidNT3.1 Productions at \"acidgunner@hotmail.com\" or \"acidnt31@hotmail.com\". Please, return to XMB if possible, or restart your PlayStation 3 system.");
				break;
			}
			case ENDING2:
			{
				if(end_time>0)
				{
					if(end_time>1) zez_y = -22; else zez_y = 96;
					zex_y = lerp(zex_y, zez_y, 0.05f);
					
					RSX_DrawSprite(15+EPISODE, 0, -40, -22, 916, 582, 1152, 960, color);
					
					RSX_DrawRectangle2D(-40, -22, 916, zex_y, 0x000000ff);
					RSX_DrawRectangle2D(-40, 562-zex_y, 916, 582, 0x000000ff);
					
					SetFontSize(28, 32);
					DrawTextAligned(0, -40, 876, -84+zex_y, 1, 0xff0000ff, 1, 28, 32, langStr[109]);
					DrawTextAligned(0, -40, 876, 588-zex_y, 1, 0xff0000ff, 1, 28, 32, langStr[110]);
				}
				else
				{
					zex_y = 96;
					zez_y = 96;
				}
				break;
			}
			case ROUTE_SELECT:
			{
				int x0, y0 = 0;
				int zp = fp_back/5;
				int mp = fp_back%5;
				animc = (count_frames / 9) % 8;
				
				int tile_bg = (4*zp)+(mp * 20);
				
				for(x0 = -40; x0 < (screen_w * 2) + 99; x0 += 32)
				{
					for(y0 = -22; y0 < (screen_h * 2) + 99; y0 += 32)
					{
						RSX_DrawSprite(1, tile_bg+anim+2, x0, y0, 32, 32, 16, 16, 0xffffffff);
					}
				}
				
				char *txt;
				
				SetFontSize(28, 32);
				if(route_selector==0) txt = "Main Route";
				else if(route_selector==1) txt = "Touhou Route";
				else if(route_selector==2) txt = "GameBoy Route";
				else if(route_selector==3) txt = "Weird Route";
				else txt = "UNKNOWN Route";
				
				DrawTextAligned(0, -40, 876, 12, 1, 0xff0000ff, 1, 28, 32, txt);
				
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
			DrawFString(0, 4,156,0xffffffff,0,28,langStr[4]);
			DrawFString(0, 4,188,0xffffffff,0,28,langStr[5]);
			DrawFString(0, 4,252,0xff0000ff,0,28,langStr[6]);
			DrawFString(0, 4,284,0xff0000ff,0,28,langStr[7]);
			DrawFString(0, 40,348,0x00aa00ff,0,28,langStr[8]);
			DrawButton(4, 348, PAD_CROSS, 0);
		}
		else DrawFString(0, 0,0,0xffffffff,0,28,langStr[0]);
	}
	
	SetFontSize(28, 32);
	if(save_running) DrawTextAligned(0, 550, 848, 512, 2, 0xffffffff, 0, 14, 32, langStr[1]);
	
	DrawProjectiles();
	
	DrawFade();
	
	if(show_ach)
	{
		SetFontSize(16, 18);
		RSX_DrawRectangle2D(-30, -2, 420, 40, 0xaaaaaaff);
		DrawFString(0, -20, -2, 0xffffffff, 0, 28, cur_ach_name);
	}
	
	// if(currentScreen == GAME)
	// {
		// if(enable_10 && global_route==1) DrawBackground2D(0x00ff0000 | (int)(tnt_alpha), -7);
		
		// if(enable_12 && !can_move_xd)
		// {
			// tiny3d_BlendFunc(1, TINY3D_BLEND_FUNC_SRC_RGB_ONE_MINUS_DST_COLOR | TINY3D_BLEND_FUNC_SRC_ALPHA_ONE,
				// TINY3D_BLEND_FUNC_DST_RGB_ZERO | TINY3D_BLEND_FUNC_DST_ALPHA_ZERO, TINY3D_BLEND_RGB_FUNC_ADD | TINY3D_BLEND_ALPHA_FUNC_ADD);
			
			// DrawBackground2D(0xffffffff, -7);
		// }
	// }
}

void MyDrawFormatString(float x, float y, const char* format, ...)
{
    char utf8_buffer[1024];
    char cp1251_buffer[1024];
	
    va_list args;
    va_start(args, format);
    vsnprintf(utf8_buffer, sizeof(utf8_buffer), format, args);
    va_end(args);
	
    utf8_to_cp1251(utf8_buffer, cp1251_buffer, sizeof(cp1251_buffer));
    DrawString(x, y, cp1251_buffer);
}

void DrawFString(int wrap, float x, float y, u32 textColor, int BW, int size, const char *fmt, ...)
{
    char buffer[1024];
	u32 shadowColor = 0x000000ff;
	
	if(BW) shadowColor = 0xffffffff;
	
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    SetFontColor(shadowColor, 0x00000000);
	
	if(wrap==0) MyDrawFormatString(x + 42, y + 24, buffer);
	else DrawWrappedText(x + 42, y + 24, size, wrap, buffer);
	
    SetFontColor(textColor, 0x00000000);
	
	if(wrap==0) MyDrawFormatString(x + 40, y + 22, buffer);
	else DrawWrappedText(x + 40, y + 22, size, wrap, buffer);
}

int GetFontWidth(const char *text, int fz)
{
    return (int)(strlen(text) * fz * 0.55f);
}

void DrawTextAligned(int wrap, float x1, float x2, float y, int align, u32 tc, int bw, int fsize, int size, const char *text, ...)
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
	DrawFString(wrap, x, y, tc, bw, size, bufferz);
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

void FadeInOut(GameScreen *variable, GameScreen value)
{
    fade_variable = variable;
    fade_value = value;
	
	if(*fade_variable == value) return;

    fade_alpha = 0.0f;
    fade_mode = FADE_OUT;
}

void UpdateFade(float dt)
{
    if(fade_mode == FADE_OUT)
    {
        fade_alpha += 255.0f * dt / 0.5f;

        if(fade_alpha >= 255.0f)
        {
            fade_alpha = 255.0f;
            fade_mode = FADE_IN;

            if(fade_variable)
            {
                *fade_variable = fade_value;
                fade_variable = NULL;
            }
        }
    }
    else if(fade_mode == FADE_IN)
    {
        fade_alpha -= 255.0f * dt / 0.5f;

        if(fade_alpha <= 0.0f)
        {
            fade_alpha = 0.0f;
            fade_mode = FADE_NONE;
        }
    }
}


void SpawnProjectile(float x, float y, float vx, float vy, int type, float lifetime)
{
    int i;

    for(i = 0; i < MAX_PROJECTILES; i++)
    {
        if(!projectiles[i].active)
        {
            projectiles[i].active = 1;
            projectiles[i].x = x;
            projectiles[i].y = y;
            projectiles[i].vx = vx;
            projectiles[i].vy = vy;
            projectiles[i].lifetime = lifetime;
            projectiles[i].type = type;
            projectiles[i].anim = 0;
			
			if(type==0) PlaySFX(&sfx_explosion);
			
            return;
        }
    }
}

void UpdateProjectiles(float dt)
{
    int i;

    for(i = 0; i < MAX_PROJECTILES; i++)
    {
        Projectile *p = &projectiles[i];

        if(!p->active) continue;
		
		if(p->type == 1)
		{
			p->x += p->vx * dt * 20;
			p->y += p->vy * dt * 20;
		}
		else
		{
			p->x += p->vx * dt;
			p->y += p->vy * dt;
		}

        p->lifetime -= dt;
		
		if(p->type == 0) p->anim += 0.41f;
		else
		{
			if(ProjectileOnCursor(projectiles[i].x, projectiles[i].y))
			{
				DebugLog("touhou crap activated");
				projectiles[i].active = 0;
				can_move_xd = 0;
				SpawnProjectile(offset_x+(cursorX*RENDER_SIZE), offset_y+(cursorY*RENDER_SIZE), 0, 0, 0, 1.0f);
			}
		}

        if(p->lifetime <= 0.0f)
        {
			if(p->type == 0) can_move_xd = 1;
            p->active = 0;
        }
    }
}