/*

Welcome to PS3Sweeper+ Source code!

Thanks for visiting this amazing project of mine.
Here is something you should know:
1. If using my source code, please credit!
2. Yes, the game code is AI-Assisted/Generated. Any game asset (music, image, story, well.. anything except some code) wasn't made with AI.
3. Please do ANY significant changes. Ports to other platforms are fine.
4. Any test has been made by a real person. If something doesn't work, please, test the issue on real hardware first.

- AcidNT3.1

*/

#define SECOND (1/60.0f)

#include <malloc.h>

#include "rsx_renderer.h"
#include "rsxutil.h"
#define MAX_BUFFERS 2

#include <sysutil/sysutil.h>

#include "soundlib/spu_soundlib.h"
#include "soundlib/audioplayer.h"

#include "bosses.h"
#include "level_data.h"
#include "save.h"
#include "drawing.h"
#include "lang.h"
#include "text.h"

#include "mp4player.h"
#define VIDEO_TEX (RSX_MAX_TEXTURES - 5)
MP4Player vid;

bool play_map_bgm = false;
bool play_10_bgm = false;
bool play_10_sfx = false;

bool temp_t2 = false;
bool test_zero = false;

bool video_finished = false;
bool start_video = false;

int temp_tz = 0;
int temp_t = 0;

int start_menu = 0;
int start_game_mus = 0;
int startbgmgtsp = 0;
int start_shop = 0;
int start_end = 0;
int start_credits = 0;

bool play_bgmz = false;
int max_route = 4;

float ship_speed = 0.75f / 2;

int LEVEL_BKP = 0;
int ready_to_click = 0;

bool HELL = false;

static volatile int exit_requested = 0;
static volatile int xmb_open = 0;

void addLogoFlag(int id, int x, int y)
{
	logoFlags[id].x = x;
	logoFlags[id].y = y;
}

static void sysutil_cb(u64 status, u64 param, void *usrdata)
{
    (void)param; (void)usrdata;
    switch (status)
    {
        case SYSUTIL_EXIT_GAME:  exit_requested = 1; break;
        case SYSUTIL_MENU_OPEN:  xmb_open = 1; break;
        case SYSUTIL_MENU_CLOSE: xmb_open = 0; break;
    }
}

void episode_check()
{
	if(EPISODEs>7) EPISODEs = 7;
	if(EPISODEs>6 && !ep7_unlock) EPISODEs = 6;
	if(EPISODEs>5 && !ep6_unlock) EPISODEs = 5;
	if(EPISODEs>4 && !ep5_unlock) EPISODEs = 4;
	if(EPISODEs>3 && !ep4_unlock) EPISODEs = 3;
	if(EPISODEs>2 && !ep3_unlock) EPISODEs = 2;
	if(EPISODEs>1 && !ep2_unlock) EPISODEs = 1;

	if(EPISODEs<1) EPISODEs=1;
}

void ResetGrid()
{
	boss9_started = 0;
	boss10_started = 0;
	boss11_started = 0;
	boss12_started = 0;
	bossnan_started = 0;
	
	show_warning = false;
	time_decrease = false;
	time_left = 999.0f;
	start_game_mus = 1;
	
    mines_count = MINES;
	tnt_alpha = 0.0f;

    cursorX = GRID_W/2;
    cursorY = GRID_H/2;

    FIRST_CLICK = 1;
    gamewin = 0;
    disable_control = 0;
    gamelose = 0;
	level_minecoins = 0;
	
	timer_running = 0;
	timer_seconds = 0;
	
	boss9_cutscene = 0;
	boss10_cutscene = 0;
	boss11_cutscene = 0;
	boss12_cutscene = 0;
	bossnan_cutscene = 0;
	
	if(!free_play)
	{
		if(EPISODE<4)
		{
			switch(LEVEL)
			{
				case 8: enable_9 = true; break;
				case 9: enable_10 = true; break;
				case 10: enable_11 = true; break;
				default: break;
			}
		}
		else if(EPISODE==4)
		{
			switch(LEVEL)
			{
				case 2: enable_10 = true; break;
				case 3: enable_12 = true; break;
				default: break;
			}
		}
		else if(EPISODE==5)
		{
			switch(LEVEL)
			{
				case 3: enable_11 = true; break;
				case 4: enable_nan = true; break;
				default: break;
			}
		}
		else if(EPISODE==7)
		{
			switch(LEVEL)
			{
				case 0: enable_9 = true; break;
				case 1: enable_10 = true; break;
				case 2: enable_11 = true; break;
				case 3: enable_12 = true; break;
				case 4: enable_nan = true; break;
				default: break;
			}
		}
	}
	
	if(enable_9 && enable_10 && enable_11 && enable_12) HELL = true;
	
	if(enable_9)
	{
		time_decrease = true;
		SetupBoss9Positions(boss9_count);
		if(free_play)
		{
			time_left = 999.9f;
			boss9_cutscene = 2;
		}
		else
		{
			if(EPISODE==1)
			{
				if(legacy_9_time || global_route==1) time_left = 99.9f;
				else
				{
					if(DIFFICULTY==0) time_left = 199.9f;
                    else if(DIFFICULTY==1) time_left = 179.9f;
                    else if(DIFFICULTY==2) time_left = 149.9f;
                    else time_left = 119.9f;
				}
			}
			else if(EPISODE==2)
			{
				if(DIFFICULTY==0) time_left = 249.9f;
				else if(DIFFICULTY==1) time_left = 219.9f;
				else if(DIFFICULTY==2) time_left = 189.9f;
				else time_left = 149.9f;
			}
			else if(EPISODE==3)
			{
				if(DIFFICULTY==0) time_left = 189.9f;
				else if(DIFFICULTY==1) time_left = 169.9f;
				else if(DIFFICULTY==2) time_left = 149.9f;
				else time_left = 119.9f;
			}
			
			if(global_route==3) time_left=88.9f;
		}
	}
	if(enable_10)
	{
		SetupBoss10Positions(boss10_count);
		if(free_play) boss10_cutscene = 2;
	}
	if(enable_11)
	{
		SetupBoss11Positions(boss11_count);
		if(free_play) boss11_cutscene = 2;
	}
	if(enable_12)
	{
		SetupBoss12Position();
		if(free_play) boss12_cutscene = 2;
	}
	if(enable_nan)
	{
		SetupBossNaNPositions(bossnan_count);
		if(free_play) bossnan_cutscene = 2;
	}
	
	if(free_play)
	{
		if(desired_bgm == 9) desired_bgm = 10;
		else if(desired_bgm == 10) desired_bgm = 12;
		else if(desired_bgm == 11) desired_bgm = 15;
		else if(desired_bgm == 14) desired_bgm = 19;
		else if(desired_bgm == 18) desired_bgm = 22;
	}
	
	int j;
	for (j = 0; j < boss10_count; j++)
	{
		boss10_list[j].time = 10;
		boss10_list[j].pulse = 0.0f;
		boss10_list[j].speed = SECOND;
		boss10_list[j].timer = 0.0f;
		boss10_list[j].tspeed = 0.0f;
	}
	
    GenerateBoard();
    CalculateNumbers();
	
	FadeInOut(&currentScreen, GAME);
	
	playing_bgm = true;
}

void ResetGridTest()
{
	boss9_started = 0;
	boss10_started = 0;
	
	show_warning = false;
	start_game_mus = 1;
	ten_arena = 0;
	
	tnt_alpha = 0.0f;
	
    FIRST_CLICK = 1;
    gamewin = 0;
    disable_control = 0;
    gamelose = 0;
	level_minecoins = 0;
	can_move_xd = 1;
	
	timer_running = 0;
	timer_seconds = 0;
	
	time_decrease = true;
	
	GRID_ID = 999;
	
	if(DIFFICULTY==1)
	{
		GRID_W = 18;
		GRID_H = 18;
		MINES = 40;
		time_left = 99.9f;
	}
	else if(DIFFICULTY==2)
	{
		GRID_W = 22;
		GRID_H = 22;
		MINES = 60;
		time_left = 149.9f;
	}
	else
	{
		GRID_W = 26;
		GRID_H = 26;
		MINES = 80;
		time_left = 199.9f;
	}
	mines_count = MINES;
	
	desired_bgm = 24;
	
	SetupBoss9Positions(boss9_count);
	SetupBoss10Positions(boss10_count);
	boss9_cutscene = 2;
	boss10_cutscene = 2;
	
	smiley_face = 4;
	
    cursorX = GRID_W/2;
    cursorY = GRID_H/2;

	int j;
	for (j = 0; j < boss10_count; j++)
	{
		boss10_list[j].time = 10;
		boss10_list[j].pulse = 0.0f;
		boss10_list[j].speed = SECOND;
		boss10_list[j].timer = 0.0f;
		boss10_list[j].tspeed = 0.0f;
	}
	
    GenerateBoard();
    CalculateNumbers();
	
	enable_9 = true;
	enable_10 = true;
	
	currentScreen = GAME;
	playing_bgm = true;
}

void GoBackToTitle()
{
	show_warning = false;
	current_minecoins = 0;
	
	HELL = false;
	
	if(free_play) LEVEL=LEVEL_BKP;
	
	enable_half = 0;
	enable_double = 0;
	enable_anti = 0;
	
	if(fp_music==100)
	{
		fp_music = temp_fp_music;
		fp_width = temp_fp_w;
		fp_height = temp_fp_h;
		fp_mines = temp_fp_m;
	}
	
	i_10 = 0;
	
	menu_pos = 0;
	menu_x = 0.0f;
	corruption = 0.0f;
	ze_x = 0.0f;
	ze_y = 0.0f;
	adventure_select = 0;
	
	if(!bad_apple) StartSave();
	
	bad_apple = 0;
	
	start_menu = 1;
	FadeInOut(&currentScreen, MENU);
}

void UnloadCustomSweeper()
{
	use_custom_sweeper = false;
	RSX_UnloadSheet(13);

	if(spritesheet[13].bmp_out)
	{
		free(spritesheet[13].bmp_out);
		spritesheet[13].bmp_out = NULL;
	}

	spritesheet[13].pitch = 0;
	spritesheet[13].width = 0;
	spritesheet[13].height = 0;
}

char custom_main_path[256];
char custom_win_path[256];

bool old_control_left = false;
bool old_control_right = false;
bool old_control_up = false;
bool old_control_down = false;
bool old_control_leftR = false;
bool old_control_rightR = false;
bool old_control_upR = false;
bool old_control_downR = false;
bool old_control_any = false;

static void LogMem(const char *tag)
{
    struct mallinfo m = mallinfo();
    DebugLog("MEM[%s]: arena=%u used=%u free=%u\n", tag, (unsigned)m.arena, (unsigned)m.uordblks, (unsigned)m.fordblks);
}

int main(int argc, const char* argv[])
{
	LogMem("start");
	
	padInfo padinfo;
	int ip;
	static padData oldpad;
	padData paddata;
	
	srand(time(NULL));
	sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT0, sysutil_cb, NULL);
	if(RSX_Init() !=0)
	{
		DebugLog("RSX: Init failed!");
		return -1;
	}
	
	u16 screen_width, screen_height;
	rsxBuffer buffers[MAX_BUFFERS];
	if (!getResolution(&screen_width, &screen_height))
	{
		DebugLog("Failed to get display resolution!");
		return -1;
	}

	DebugLog("Display: %dx%d\n", screen_width, screen_height);
	for (int i = 0; i < MAX_BUFFERS; i++) {
		if (!makeBuffer(&buffers[i], screen_width, screen_height, i))
		{
			DebugLog("Failed to create framebuffer %d!\n", i);
			return -1;
		}
	}
	
	if (RSX_LoadShaders() != 0)
	{
		DebugLog("RSX: shader loading failed!");
		return -1;
	}
	
	if (RSX_InitCRT() != 1)
	{
		DebugLog("RSX: CRT effect failed!");
		return -1;
	}
	
	if (!RSX_CreateSpriteBuffers())
	{
		DebugLog("RSX: sprite buffer creation failed!");
		return -1;
	}
	LogMem("after rsx");
	
	sysModuleLoad(SYSMODULE_PNGDEC);
	
	savestate_init();
	SplashesInit();
	Initialize();
	
	boss9_started = 0;
	boss10_started = 0;
	boss11_started = 0;
	boss12_started = 0;
	bossnan_started = 0;
	
	the_hat = 0;
	
	init_bgm();
	init_sfx();
	LogMem("after sfx");
	
	ioPadInit(7);
	Load_PNG();
    RSX_UploadSpritesheets();
	Text_Init();
	LogMem("after assets");
	
	LangInit();
	creditsInit();
	SetupTimeSettings();
	
	StartLoad();
	
	for(int i = 0; i < MAX_BUFFERS; i++) RSX_TestDraw(&buffers[i]);
	if(!flip(RSX_GetContext(), MAX_BUFFERS - 1)) DebugLog("RSX: initial flip failed!\n");
	int currentBuffer = 0;
	
	depth_pitch = screen_width * 2;
	depth_buffer = (u32*)rsxMemalign(64, depth_pitch * screen_height);
	if (!depth_buffer)
	{
		DebugLog("RSX: depth buffer allocation failed!");
		return -1;
	}

	if (rsxAddressToOffset(depth_buffer, &depth_offset) != 0)
	{
		DebugLog("RSX: depth buffer offset failed!");
		return -1;
	}

    while(1)
    {
		sysUtilCheckCallback();
		
		if (exit_requested) break;
		
		UpdateFPS();
		
		UpdateFade(1.0f / 60.0f);
		UpdateProjectiles(1.0f / 60.0f);
		
		if(game_loaded)
		{
			ioPadGetInfo(&padinfo);
			for(ip = 0; ip < MAX_PADS; ip++)
			{
				if(padinfo.status[ip])
				{
					ioPadGetData(ip, &paddata);
					
					bool control_left = paddata.BTN_LEFT || paddata.ANA_L_H < 80;
					bool control_right = paddata.BTN_RIGHT || paddata.ANA_L_H > 176;

					bool control_up = paddata.BTN_UP || paddata.ANA_L_V < 80;
					bool control_down = paddata.BTN_DOWN || paddata.ANA_L_V > 176;
					
					bool control_leftR = paddata.ANA_R_H < 80;
					bool control_rightR = paddata.ANA_R_H > 176;
					bool control_upR = paddata.ANA_R_V < 80;
					bool control_downR = paddata.ANA_R_V > 176;
					
					bool control_any = paddata.BTN_CROSS || paddata.BTN_CIRCLE || paddata.BTN_TRIANGLE || paddata.BTN_SQUARE || paddata.BTN_START || paddata.BTN_SELECT || paddata.BTN_L1 || paddata.BTN_L2 || paddata.BTN_R1 || paddata.BTN_R2 || paddata.BTN_L3 || paddata.BTN_R3;
					
					if(paddata.BTN_CROSS || paddata.BTN_CIRCLE) button_x_hold = true;
					else button_x_hold = false;
					
					if(paddata.BTN_START) button_start_hold = true;
					else button_start_hold = false;
					
					switch(currentScreen)
					{
						case LOGO:
						{
							static int video_frame = 0;
							static int video_framez = 0;
							static int f = 0;
							
							video_framez++;
							video_frame = video_framez / 2;
							
							if(video_frame>=10 && control_any && !old_control_any) video_frame = 9999;
							
							if(video_frame==60)
							{
								addLogoFlag(f++,(876-40)/2,(560-22)/2);
								PlaySFX(&sfx_flag);
							}
							
							if(video_frame>=120 && video_frame<=180)
							{
								int test = video_frame - 120;
								float t = (test / 60.0f) * 4.0f; 
								
								float dx = 1.0f - fabs(fmod(t + 1.0f, 4.0f) - 2.0f);
								float dy = 1.0f - fabs(fmod(t, 4.0f) - 2.0f);
								
								int test1 = (int)(dx * 160);
								int test2 = (int)(dy * 160);
								
								addLogoFlag(f++, ((876-40)/2) + test1, ((560-22)/2) + test2);
								PlaySFX(&sfx_flag);
							}
							
							if(video_frame >= 210 && video_frame <= 270)
							{
								int total_frames = 60;
								int test = video_frame - 210;
								
								float t = ((float)test / total_frames) * 4.0f;
								
								int w = 50;
								int h = 100;
								int test1 = 0;
								int test2 = 0;

								if (t < 1.0f) {
									test1 = (int)(-w * t);
									test2 = (int)(-h + (h * 0.5f * t));
								}
								else if (t < 2.0f) {
									float seg = t - 1.0f;
									test1 = (int)(-w + (2.0f * w * seg));
									test2 = (int)((-h * 0.5f) + (h * seg));
								}
								else if (t < 3.0f) {
									float seg = t - 2.0f;
									test1 = (int)(w - (w * seg));
									test2 = (int)((h * 0.5f) + (h * 0.5f * seg));
								}
								else {
									float seg = t - 3.0f;
									test1 = 0;
									test2 = (int)(h - (2.0f * h * seg));
								}

								addLogoFlag(f++, ((876-40)/2) + test1, ((560-22)/2) + test2);
								PlaySFX(&sfx_flag);
							}
							
							if(video_frame==360)
							{
								for(int fz = 0; fz < 500; fz++)
								{
									logoFlags[fz].x = 0;
									logoFlags[fz].y = 0;
								}
								show_logo=true;
								PlaySFX(&sfx_unflag);
							}
							
							if(video_frame>520)
							{
								for(int fz = 0; fz < 500; fz++)
								{
									logoFlags[fz].x = 0;
									logoFlags[fz].y = 0;
								}
								
								route_selector = global_route;
								
								if(!route_touhou_unlocked && global_route==1) global_route=0;
								if(!route_weird_unlocked && global_route==3) global_route=0;
								
								if(global_route==2) RSX_SetGameBoyEnabled(1);
								else RSX_SetGameBoyEnabled(0);
								
								if(global_route==1) global_route_tex = 21;
								else if(global_route==3) global_route_tex = 23;
								else global_route_tex = 1;
								
								currentScreen = TITLE;
								start_music = 1;
								video_frame = 0;
								
								HolyShit2(0);
							}
							break;
						}
						case TITLE:
						{
							if(start_music)
							{
								start_music = 0;
								playing_bgm = true;
								PlayBGM(0, 1);
								ready_to_click = 1;
								easter_egg = false;
							}
							
							if(settings_select)
							{
								if(paddata.BTN_L2 && !oldpad.BTN_L2)
								{
									if(language>=2) language=0;
									else language++;
									
									LangInit();
								}
								if(paddata.BTN_R2 && !oldpad.BTN_R2)
								{
									if(setting_guarantee>=2) setting_guarantee=0;
									else setting_guarantee++;
								}
								if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
								{
									splash = splashes_random[rand() % SplashesCount()];
									settings_select = 0;
								}
								if(paddata.BTN_R3 && !oldpad.BTN_R3)
								{
									if(R_SENSITIVITY>=8) R_SENSITIVITY=1;
									else R_SENSITIVITY++;
								}
								if(paddata.BTN_L3 && !oldpad.BTN_L3)
								{
									if(draw_grid_outlines==0) draw_grid_outlines=1;
									else draw_grid_outlines=0;
								}
							}
							else if(ready_to_click)
							{
								if(paddata.BTN_START && !oldpad.BTN_START)
								{
									ready_to_click = 0;
									menu_x = 0.0f;
									ze_x = 0.0f;
									ze_y = 0.0f;
									StopAudio();
									start_menu = 1;
									PlaySFX(&sfx_gamestart);
									FadeInOut(&currentScreen, MENU);
								}
								
								if(paddata.BTN_SQUARE && !oldpad.BTN_SQUARE) settings_select = 1;
								
								if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE)
								{
									start_credits = 1;
									ready_to_click = 0;
									FadeInOut(&currentScreen, CREDITS);
								}
							}
							break;
						}
						case MENU:
						{
							if(start_menu)
							{
								bad_apple_plays = false;
								boss_render_x = 0.0f;
								menu_x = 0.0f;
								ze_x = 0.0f;
								ze_y = 0.0f;
								start_menu = 0;
								
								disable_control = 0;
								can_move_xd = 1;
								
								enable_9 = false;
								enable_10 = false;
								enable_11 = false;
								enable_12 = false;
								enable_nan = false;
								
								free_play = 0;

								gamewin = 0;
								gamelose = 0;
								PlayBGM(50+fp_music, 1);
								paused = false;
								ready_to_click = 1;
								
								RSX_SetCRTEnabled(0);
							}
							
							menu_x = lerp(menu_x, -262 * menu_pos, 0.1f);
							boss_render_x = lerp(boss_render_x, 0.0f, 0.1f);
							
							if(ready_to_click)
							{
								if(paddata.BTN_SELECT && !oldpad.BTN_SELECT) master_modeS = !master_modeS;
								
								if(!free_play)
								{
									if(challenge_select)
									{
										if(paddata.BTN_L2 && !oldpad.BTN_L2)
										{
											master_mode = master_modeS;
											if(master_mode) LIFES = 1;
											ready_to_click = 0;
											GRID_ID = 999;
											free_play = 1;
											fp_width = 72;
											fp_height = 48;
											fp_mines = 111;
											LEVEL_BKP = LEVEL;
											LoadFPLevel();
											can_move_xd = 1;
										}
										if(paddata.BTN_R2 && !oldpad.BTN_R2)
										{
											master_mode = master_modeS;
											if(master_mode) LIFES = 1;
											ready_to_click = 0;
											GRID_ID = 999;
											free_play = 1;
											fp_width = 20;
											fp_height = 20;
											fp_mines = 60;
											enable_9 = true;
											enable_10 = true;
											enable_11 = true;
											enable_12 = true;
											LEVEL_BKP = LEVEL;
											LoadFPLevel();
											can_move_xd = 1;
										}
										
										if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE) challenge_select = 0;
									}
									else if(loadout_select)
									{
										if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE) loadout_select = 0;
										
										if(paddata.BTN_SQUARE && !oldpad.BTN_SQUARE)
										{
											if(!use_custom_sweeper)
											{
												FILE *fp = fopen("/dev_usb000/SWEEPING/captain.png", "r");
												if(!fp) msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
													langStr[48], EmptyDialogCallback, NULL, NULL);
												else
												{
													LoadImageA("/dev_usb000/SWEEPING/captain.png", &spritesheet[13]);
													
													int temp = 2048;
													
													if(spritesheet[13].width > temp || spritesheet[13].height > temp)
													{
														DebugLog("CAPTAIN: Wrong image size!");
														UnloadCustomSweeper();
														msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
															langStr[49], EmptyDialogCallback, NULL, NULL);
													}
													else if(spritesheet[13].width != spritesheet[13].height)
													{
														DebugLog("CAPTAIN: Wrong image size!");
														UnloadCustomSweeper();
														msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
															langStr[53], EmptyDialogCallback, NULL, NULL);
													}
													else
													{
														if(RSX_UploadSheet(13)) use_custom_sweeper = true;
														else UnloadCustomSweeper();
													}
												}
											}
											else UnloadCustomSweeper();
										}
										
										if(paddata.BTN_L1 && !oldpad.BTN_L1) if(smiley_face>0) smiley_face--;
										if(paddata.BTN_R1 && !oldpad.BTN_R1) if(smiley_face<8) smiley_face++;
										
										if(paddata.BTN_L2 && !oldpad.BTN_L2) if(the_hat>-1) the_hat--;
										if(paddata.BTN_R2 && !oldpad.BTN_R2) if(the_hat<8) the_hat++;
										
										if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE)
										{
											startbgmgtsp = 1;
											ship_x = -80;
											ship_y = -22+(32*6);
											if(!shop_unlocked)
											{
												if(global_minecoins > 250)
												{
													ready_to_click = 0;
													shop_first = true;
													shop_unlocked = true;
													global_minecoins -= 250;
													StopAudio();
													PlaySFX(&sfx_buy);
													StartSave();
													FadeInOut(&currentScreen, GOTOSHOP);
												}
											}
											else
											{
												ready_to_click = 0;
												shop_first = false;
												StopAudio();
												FadeInOut(&currentScreen, GOTOSHOP);
											}
										}
									}
									else if(adventure_select)
									{
										if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
										{
											episode_check();
											
											if(EPISODEs>5)
											{
												msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
													"This episode is not started it's development, please wait! ^_^", EmptyDialogCallback, NULL, NULL);
												continue;
											}
											else if(EPISODEs>=4)
											{
												msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
													"This episode is W.I.P., however you can still access it. Beware of the crappy stuff going on. Thanks!", EmptyDialogCallback, NULL, NULL);
											}
											
											GRID_ID = 999;
											ready_to_click = 0;
											ship_x = -80;
											ship_y = -44;
											if(paddata.BTN_R2) global_route = 1;
											LEVEL = 0;
											DIFFICULTY = DIFFICULTYs;
											EPISODE = EPISODEs;
											LIFES = 3;
											master_mode = master_modeS;
											if(master_mode) LIFES = 1;
											
											current_minecoins = 0;
											mistakes = 0;
											SetupTimeSettings();
											StopAudio();
											PlaySFX(&sfx_gamestart);
											
											start_video = true;
											if(global_route!=3) FadeInOut(&currentScreen, MAP);
											else FadeInOut(&currentScreen, VIDEOTEST);
											
											ship_a = true;
										}
										if(control_up && !old_control_up)
										{
											DIFFICULTYs--;
											if (DIFFICULTYs<0) DIFFICULTYs = 0;
										}
										if(control_down && !old_control_down)
										{
											DIFFICULTYs++;
											if (DIFFICULTYs>3) DIFFICULTYs = 3;
										}
										if(control_left && !old_control_left)
										{
											EPISODEs--;
											episode_check();
											if (EPISODEs<1) EPISODEs = 1;
										}
										if(control_right && !old_control_right)
										{
											EPISODEs++;
											episode_check();
											if (EPISODEs>7) EPISODEs = 7;
										}
										if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE) adventure_select = 0;
										if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE) currentScreen = ROUTE_SELECT;
									}
									else
									{
										if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
										{
											switch(menu_pos)
											{
												case 0: adventure_select = 1; break;
												case 1: free_play = 1; break;
												case 2: challenge_select = 1; break;
												case 3: loadout_select = 1; break;
												default: break;
											}
										}
										if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE && LEVEL>0)
										{
											GRID_ID = 999;
											ready_to_click = 0;
											free_play = 0;
											ship_x = 120.0f+(64*LEVEL);
											StopAudio();
											
											global_route=saved_route;
											
											if(global_route==2) RSX_SetGameBoyEnabled(1);
											else RSX_SetGameBoyEnabled(0);
											
											if(global_route==1) global_route_tex = 21;
											else if(global_route==3) global_route_tex = 23;
											else global_route_tex = 1;
											
											start_video = true;
											if(global_route!=3) FadeInOut(&currentScreen, MAP);
											else FadeInOut(&currentScreen, VIDEOTEST);
											
											PlaySFX(&sfx_gamestart);
											ship_a = true;
										}
										
										if(control_left && !old_control_left)
										{
											menu_pos--;
											if(menu_pos<0) menu_pos = 0;
										}
										if(control_right && !old_control_right)
										{
											menu_pos++;
											if(menu_pos>3) menu_pos = 3;
										}
										
										if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
										{
											ready_to_click = 0;
											splash = splashes_random[rand() % SplashesCount()];
											StopAudio();
											start_music = 1;
											FadeInOut(&currentScreen, TITLE);
											PlaySFX(&sfx_gamestart);
										}
									}
								}
								else
								{
									if(boss_options)
									{
										if(control_up && !old_control_up)
										{
											boss_menu--;
											boss_render_x += 20.0f;
											if (boss_menu<0) boss_menu = 0;
										}
										if(control_down && !old_control_down)
										{
											boss_menu++;
											boss_render_x -= 20.0f;
											if (boss_menu>4) boss_menu = 4;
										}
										if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
										{
											switch(boss_menu)
											{
												case 0: enable_9 = !enable_9; break;
												case 1: enable_10 = !enable_10; break;
												case 2: enable_11 = !enable_11; break;
												case 3: enable_12 = !enable_12; break;
												case 4: enable_nan = !enable_nan; break;
											}
										}
										if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE) boss_options = false;
									}
									else
									{
										if(paddata.BTN_START && !oldpad.BTN_START)
										{
											GRID_ID = 999;
											ready_to_click = 0;
											LEVEL_BKP = LEVEL;
											PlaySFX(&sfx_gamestart);
											LoadFPLevel();
											can_move_xd = 1;
										}
										
										if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE) free_play = 0;
										if(paddata.BTN_SQUARE && !oldpad.BTN_SQUARE) boss_options = true;
										
										if(control_up && !old_control_up)
										{
											fp_menu--;
											if (fp_menu<0) fp_menu = 0;
										}
										if(control_down && !old_control_down)
										{
											fp_menu++;
											if (fp_menu>7) fp_menu = 7;
										}
										
										int max_mus = 18;
										
										if(control_left && !old_control_left)
										{
											switch(fp_menu)
											{
												case 0: fp_width--; if(fp_width<3) fp_width=3; break;
												case 1: fp_height--; if(fp_height<3) fp_height=3; break;
												case 2: fp_mines--; if(fp_mines<1) fp_mines=1; break;
												case 3: fp_doubles--; if(fp_doubles<0) fp_doubles=0; break;
												case 4: fp_halfes--; if(fp_halfes<0) fp_halfes=0; break;
												case 5: fp_antis--; if(fp_antis<0) fp_antis=0; break;
												case 6:
												{
													fp_music--;
													if(fp_music>95) fp_music=max_mus;
													if(fp_music<1) fp_music=1;
													ReplaceBGM(fp_music);
													break;
												}
												case 7: fp_back--; if(fp_back<0) fp_back=0; break;
												default: break;
											}
											
											if(fp_mines>(fp_width*fp_height)-1) fp_mines=(fp_width*fp_height)-1;
										}
										if(control_right && !old_control_right)
										{
											switch(fp_menu)
											{
												case 0: fp_width++; if(fp_width>72) fp_width=72; break;
												case 1: fp_height++; if(fp_height>48) fp_height=48; break;
												case 2: fp_mines++; break;
												case 3: fp_doubles++; if(fp_doubles>(fp_mines-fp_halfes-fp_antis)) fp_doubles=fp_mines-fp_halfes-fp_antis; break;
												case 4: fp_halfes++; if(fp_halfes>(fp_mines-fp_doubles-fp_antis)) fp_halfes=fp_mines-fp_doubles-fp_antis; break;
												case 5: fp_antis++; if(fp_antis>(fp_mines-fp_doubles-fp_halfes)) fp_antis=fp_mines-fp_doubles-fp_halfes; break;
												case 6:
												{
													fp_music++;
													if(fp_music>max_mus) fp_music=max_mus;
													ReplaceBGM(fp_music);
													break;
												}
												case 7: fp_back++; if(fp_back>14) fp_back=14; break;
												default: break;
											}
											
											if(fp_mines>(fp_width*fp_height)-1) fp_mines=(fp_width*fp_height)-1;
										}
										
										if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE)
										{
											if(bad_apple_init())
											{
												enable_9 = false;
												enable_10 = false;
												enable_11 = false;
												enable_12 = false;
												enable_nan = false;
												ready_to_click = 0;
												LEVEL_BKP = LEVEL;
												bad_apple_plays = true;
												DebugLog("Loaded Bad Apple!!\n");
												PlaySFX(&sfx_gamestart);
												LoadFPLevel();
											}
										}
										
										if(fp_menu==6 && paddata.BTN_CROSS && !oldpad.BTN_CROSS)
										{
											if(!custom_music)
											{
												has_custom_main = FindMusic("/dev_usb000/SWEEPING/bgm", custom_main_path);
												has_custom_win  = FindMusic("/dev_usb000/SWEEPING/win", custom_win_path);

												if(!has_custom_main)
												{
													msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
														langStr[50], EmptyDialogCallback, NULL, NULL);
													continue;
												}
												
												bgm[99] = custom_main_path;
												fp_music = 99;

												if(!has_custom_win)
												{
													msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
														langStr[51], EmptyDialogCallback, NULL, NULL);
													continue;
												}
												bgm[199] = custom_win_path;
												custom_music = true;
											}
											else
											{
												fp_music = 1;
												custom_music = false;
											}
										}
									}
								}
							}
							break;
						}
						case MAP:
						{
							if(!play_map_bgm)
							{
								enable_9 = false;
								enable_10 = false;
								enable_11 = false;
								enable_12 = false;
								enable_nan = false;
								if(video_finished) MP4Player_Close(&vid);
								video_finished = false;
								
								int lvl_slct_bgm = 0;
								switch(EPISODE)
								{
									case 1:
									{
										ship_r = 0;
										ship_y = -22.0f+(32*9);
										if(LEVEL==0) lvl_slct_bgm = 151;
										else if(LEVEL==1) lvl_slct_bgm = 151;
										else if(LEVEL==2) lvl_slct_bgm = 152;
										else if(LEVEL==3) lvl_slct_bgm = 152;
										else if(LEVEL==4) lvl_slct_bgm = 153;
										else if(LEVEL==5) lvl_slct_bgm = 153;
										else if(LEVEL==6) lvl_slct_bgm = 154;
										else if(LEVEL==7) lvl_slct_bgm = 154;
										else
										{
											StopAudio();
											if(global_route==1)
											{
												PlayBGM(190, 1);
												PlaySFX(&sfx_siren);
											}
										}
										break;
									}
									case 2:
									{
										ship_r = 0;
										ten_arena = 0;
										
										switch(LEVEL)
										{
											case 0: lvl_slct_bgm = 155; ship_x = -80.0f; ship_y = -22.0f+(32*7); break;
											case 1: lvl_slct_bgm = 155; ship_x = 88.0f+(64*4); ship_y = -22.0f+(32*7); break;
											case 2: lvl_slct_bgm = 156; ship_x = 88.0f+(64*5); ship_y = -22.0f+(32*7); break;
											case 3: lvl_slct_bgm = 156; ship_x = 88.0f+(64*6); ship_y = -22.0f+(32*7); break;
											case 4: lvl_slct_bgm = 157; ship_x = 120.0f+(64*6); ship_y = -22.0f+(32*10); break;
											case 5: lvl_slct_bgm = 157; ship_x = 120.0f+(64*6); ship_y = -22.0f+(32*12); break;
											case 6: lvl_slct_bgm = 158; ship_x = 88.0f+(64*5); ship_y = -22.0f+(32*13); break;
											case 7: lvl_slct_bgm = 158; ship_x = 88.0f+(64*4); ship_y = -22.0f+(32*13); break;
											case 8: lvl_slct_bgm = 159; ship_x = 120.0f+(64*3); ship_y = -22.0f+(32*10); break;
											case 9: ship_x = 88.0f+(64*5); ship_y = -22.0f+(32*9); StopAudio(); break;
										}
										break;
									}
									case 3:
									{
										ship_r = 3;
										
										switch(LEVEL)
										{
											case 0: lvl_slct_bgm = 151; ship_x = 88.0f+(64*4); ship_y = 592.0f; break;
											case 1: lvl_slct_bgm = 155; ship_x = 88.0f+(64*4); ship_y = -22.0f+(32*14); break;
											case 2: lvl_slct_bgm = 152; ship_x = 88.0f+(64*4); ship_y = -22.0f+(32*12); break;
											case 3: lvl_slct_bgm = 156; ship_x = 88.0f+(64*4); ship_y = -22.0f+(32*10); break;
											case 4: lvl_slct_bgm = 153; ship_x = 120.0f+(64*4); ship_y = -22.0f+(32*9); break;
											case 5: lvl_slct_bgm = 157; ship_x = 120.0f+(64*5); ship_y = -22.0f+(32*9); break;
											case 6: lvl_slct_bgm = 154; ship_x = 88.0f+(64*6); ship_y = -22.0f+(32*10); break;
											case 7: lvl_slct_bgm = 158; ship_x = 88.0f+(64*6); ship_y = -22.0f+(32*12); break;
											case 8: lvl_slct_bgm = 159; ship_x = 120.0f+(64*5); ship_y = -22.0f+(32*15); break;
											case 9: lvl_slct_bgm = 160; ship_x = 88.0f+(64*5); ship_y = -22.0f+(32*12); break;
											case 10: ship_x = 88.0f+(64*5); ship_y = -22.0f+(32*14); StopAudio(); break;
										}
										break;
									}
									case 4:
									{
										ship_r = 0;
										
										switch(LEVEL)
										{
											case 0: lvl_slct_bgm = 162; ship_x = -80.0f; ship_y = -22.0f+(32*7); break;
											case 1: lvl_slct_bgm = 163; ship_x = 120.0f+(64*6); ship_y = -22.0f+(32*10); break;
											case 2: lvl_slct_bgm = 160; ship_x = 88.0f+(64*5); ship_y = -22.0f+(32*13); break;
											case 3: ship_x = 88.0f+(64*5); ship_y = -22.0f+(32*9); StopAudio(); break;
										}
										break;
									}
									case 7:
									{
										ship_r = 0;
										ship_y = -22.0f+(32*9);
										if(LEVEL==0) lvl_slct_bgm = 159;
										else if(LEVEL==1) lvl_slct_bgm = 160;
										else if(LEVEL==2) lvl_slct_bgm = 152;
										else if(LEVEL==3) lvl_slct_bgm = 152;
										else if(LEVEL==4) lvl_slct_bgm = 153;
										else if(LEVEL==5) lvl_slct_bgm = 153;
										else StopAudio();
										break;
									}
									default: break;
								}
								play_bgmz = false;
								play_10_bgm = false;
								
								if(lvl_slct_bgm>0) PlayBGM(lvl_slct_bgm, 1);
								play_map_bgm = true;
								ready_to_click = 1;
							}
							
							if(ship_a)
							{
								switch(EPISODE)
								{
									case 1:
									{
										if(LEVEL<8)
										{
											if(ship_x < 120.0f+(64*(LEVEL+1))) ship_x+=ship_speed;
											else ship_a = false;
										}
										else
										{
											if(global_route==1)
											{
												if(ship_x > 1314.0f - (876*2) - 256) ship_x-=ship_speed*2;
												else ship_a = false;
											}
											else
											{
												if(ship_x < 1314.0f) ship_x+=ship_speed;
												else ship_a = false;
											}
										}
										
										if(ship_x > 876.0f && !play_bgmz)
										{
											play_bgmz = true;
											PlayBGM(159, 1);
										}
										if(ship_x < -79.0f && LEVEL>0 && !play_bgmz)
										{
											play_bgmz = true;
											StopSFX(&sfx_siren);
											PlayBGM(159, 1);
										}
										
										break;
									}
									case 2:
									{
										if(LEVEL == 0 && (ship_x < 88.0f+(64*4))) ship_x+=ship_speed;
										else if(LEVEL == 1 && (ship_x < 88.0f+(64*5))) ship_x+=ship_speed;
										else if(LEVEL == 2 && (ship_x < 88.0f+(64*6))) ship_x+=ship_speed;
										else if(LEVEL == 3)
										{
											if(ship_x < 88.0f+(64*6)+32) ship_x+=ship_speed;
											else
											{
												ship_r = 1;
												if(ship_y < -22.0f+(32*10)) ship_y+=ship_speed;
												else ship_a = false;
											}
										}
										else if(LEVEL == 4)
										{
											ship_r = 1;
											if(ship_y < -22.0f+(32*12)) ship_y+=ship_speed;
											else ship_a = false;
										}
										else if(LEVEL == 5)
										{
											if(ship_y < -22.0f+(32*13))
											{
												ship_r = 1;
												ship_y+=ship_speed;
											}
											else
											{
												ship_r = 2;
												if(ship_x > 88.0f+(64*5)) ship_x-=ship_speed;
												else ship_a = false;
											}
										}
										else if(LEVEL == 6)
										{
											ship_r = 2;
											if(ship_x > 88.0f+(64*4)) ship_x-=ship_speed;
											else ship_a = false;
										}
										else if(LEVEL == 7)
										{
											if(ship_x > 88.0f+(64*3)+32)												
											{
												ship_r = 2;
												ship_x-=ship_speed;
											}
											else
											{
												ship_r = 3;
												if(ship_y > -22.0f+(32*10)) ship_y-=ship_speed;
												else ship_a = false;
											}
										}
										else if(LEVEL == 8)
										{
											if(ship_y > -22.0f+(32*9))
											{
												ship_r = 3;
												ship_y-=ship_speed;
											}
											else
											{
												ship_r = 0;
												if(ship_x < 88.0f+(64*5)) ship_x+=ship_speed;
												else ship_a = false;
											}
										}
										else if(LEVEL == 9)
										{
											ten_arena++;
											int ten_arena2 = ten_arena / 4;
											if(ten_arena2==160 && !play_10_sfx)
											{
												PlaySFX(&sfx_blackout);
												play_10_sfx = true;
											}
											else if(ten_arena2==300 && !play_10_bgm)
											{
												PlayBGM(160, 1);
												play_10_bgm = true;
											}
											else if(ten_arena2>300) ship_a = false;
										}
										else ship_a = false;
										break;
									}
									case 3:
									{
										if(LEVEL == 0 && (ship_y > -22.0f+(32*14))) ship_y-=ship_speed;
										else if(LEVEL == 1 && (ship_y > -22.0f+(32*12))) ship_y-=ship_speed;
										else if(LEVEL == 2 && (ship_y > -22.0f+(32*10))) ship_y-=ship_speed;
										else if(LEVEL == 3)
										{
											if(ship_y > -22.0f+(32*9))
											{
												ship_r = 3;
												ship_y-=ship_speed;
											}
											else
											{
												ship_r = 0;
												if(ship_x < 120.0f+(64*4)) ship_x+=ship_speed;
												else ship_a = false;
											}
										}
										else if(LEVEL == 4)
										{
											ship_r = 0;
											if(ship_x < 120.0f+(64*5)) ship_x+=ship_speed;
											else ship_a = false;
										}
										else if(LEVEL == 5)
										{
											if(ship_x < 88.0f+(64*6))
											{
												ship_r = 0;
												ship_x+=ship_speed;
											}
											else
											{
												ship_r = 1;
												if(ship_y < -22.0f+(32*10)) ship_y+=ship_speed;
												else ship_a = false;
											}
										}
										else if(LEVEL == 6)
										{
											ship_r = 1;
											if(ship_y < -22.0f+(32*12)) ship_y+=ship_speed;
											else ship_a = false;
										}
										else if(LEVEL == 7)
										{
											if(ship_y < -22.0f+(32*15))
											{
												ship_r = 1;
												ship_y+=ship_speed;
											}
											else
											{
												ship_r = 2;
												if(ship_x > 120.0f+(64*5)) ship_x-=ship_speed;
												else ship_a = false;
											}
										}
										else if(LEVEL == 8)
										{
											if(ship_x > 88.0f+(64*5))												
											{
												ship_r = 2;
												ship_x-=ship_speed;
											}
											else
											{
												ship_r = 3;
												if(ship_y > -22.0f+(32*12)) ship_y-=ship_speed;
												else ship_a = false;
											}
										}
										else if(LEVEL == 9)
										{
											ship_r = 3;
											if(ship_y > -22.0f+(32*10)) ship_y-=ship_speed;
											else ship_a = false;
										}
										else if(LEVEL == 10)
										{
											ship_a = false;
										}
										else ship_a = false;
										break;
									}
									case 4:
									{
										if(LEVEL == 0 && (ship_x < 88.0f+(64*5))) ship_x+=ship_speed;
										else if(LEVEL == 1)
										{
											ship_r = 1;
											if(ship_y < -22.0f+(32*12)) ship_y+=ship_speed;
											else ship_a = false;
										}
										else if(LEVEL == 2)
										{
											ship_r = 2;
											if(ship_x > 88.0f+(64*4)) ship_x-=ship_speed;
											else ship_a = false;
										}
										else if(LEVEL == 3)
										{
											ship_a = false;
										}
										else ship_a = false;
										break;
									}
									case 7:
									{
										if(LEVEL<6)
										{
											if(ship_x < 152.0f+(64*(LEVEL+1))) ship_x+=ship_speed;
											else ship_a = false;
										}
										break;
									}
									default: break;
								}
							}
							else if(ready_to_click)
							{
								if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
								{
									switch(LEVEL)
									{
										case 0: case 1: smiley_face = 0; break;
										case 2: case 3: smiley_face = 1; break;
										case 4: case 5: smiley_face = 2; break;
										case 6: case 7: smiley_face = 3; break;
										case 8: smiley_face = 4; break;
										case 9: smiley_face = 5; break;
										case 10: smiley_face = 6; break;
										case 11: smiley_face = 7; break;
										case 12: smiley_face = 8; break;
										default: smiley_face = 0; break;
									}
									
									if(LEVEL==2 && EPISODE==4) smiley_face = 5;
									if(LEVEL==3 && EPISODE==4) smiley_face = 4;
									
									ready_to_click = 0;
									StopAudio();
									FIRST_CLICK = 1;
									
									LoadLevel(LEVEL);
									
									if(LEVEL>=8) PlaySFX(&sfx_level2);
									else if(LEVEL>=2 && EPISODE==4) PlaySFX(&sfx_level2);
									else if(LEVEL>=3 && EPISODE==5) PlaySFX(&sfx_level2);
									else if(EPISODE==7) PlaySFX(&sfx_level2);
									else PlaySFX(&sfx_level);
									
									can_move_xd = 1;
								}
							}
							break;
						}
						case GAME:
						{
							if(ten_arena > 0) ten_arena = 0;
							
							if(start_game_mus)
							{
								play_map_bgm = false;
								playing_bgm = true;
								start_game_mus = 0;
								if(HELL) desired_bgm = 194;
								PlayBGM(desired_bgm, 1);
								ready_to_click = 1;
								test_zero = true;
								local_mistakes = 0;
								fp_lifes = 3;
								
								cursor_x = (876-40)/2;
								cursor_y = (560-22)/2;
								
								if(enable_nan) RSX_SetCRTEnabled(1);
							}
							
							if(ready_to_click)
							{
								if(gamelose && paddata.BTN_START && !oldpad.BTN_START)
								{
									ready_to_click = 0;
									LEVEL=0;
									saved_route = global_route;
									GoBackToTitle();
								}
								else if(paused)
								{
									if(paddata.BTN_SELECT && !oldpad.BTN_SELECT)
									{
										ready_to_click = 0;
										saved_route = global_route;
										GoBackToTitle();
									}
									if(!paddata.BTN_START && oldpad.BTN_START)
									{
										paused = false;
										SND_Pause(0);
									}
								}
								else if(gamewin)
								{
									if(paddata.BTN_SELECT && !oldpad.BTN_SELECT)
									{
										ready_to_click = 0;
										saved_route = global_route;
										if(free_play) LoadFPLevel();
										else
										{
											LEVEL++;
											GoBackToTitle();
										}
									}

									if(paddata.BTN_START && !oldpad.BTN_START)
									{
										ready_to_click = 0;
										saved_route = global_route;
										if(!free_play)
										{
											int max_lvl = episodes[EPISODE-1].level_count;
											LEVEL++;
											
											if(LEVEL >= max_lvl)
											{
												if(EPISODE == 2 && DIFFICULTY > 0 && LEVEL < 67)
												{
													start_end = 1;
													LEVEL = 66;
													FadeInOut(&currentScreen, ENDING2);
												}
												else
												{
													start_end = 1;
													FadeInOut(&currentScreen, ENDING);
												}
											}
											else
											{
												StartSave();
												ship_a = true;
												FadeInOut(&currentScreen, MAP);
											}
										}
										else GoBackToTitle();
									}
								}
								else
								{
									if(show_warning)
									{
										if(!paddata.BTN_START && oldpad.BTN_START) show_warning = false;
										if(!paddata.BTN_TRIANGLE && oldpad.BTN_TRIANGLE)
										{
											if(enable_9 && !seen_nine) seen_nine = true;
											else if(enable_9 && global_route==1 && !seen_fnine) seen_fnine = true;
											else if(enable_10 && !seen_ten) seen_ten = true;
											else if(enable_10 && global_route==1 && !seen_nten) seen_nten = true;
											else if(enable_11 && !seen_eleven) seen_eleven = true;
											else if(enable_11 && global_route==1 && !seen_meleven) seen_meleven = true;
											else if(enable_12 && !seen_twelve) seen_twelve = true;
											else if(enable_12 && global_route==1 && !seen_ktwelve) seen_ktwelve = true;
											else if(enable_nan && !seen_nan) seen_nan = true;
										}
									}
									else if (!disable_control && can_move_xd && !bad_apple)
									{
										// pausing shit
										if(!paddata.BTN_START && oldpad.BTN_START)
										{
											bool enable_bosses = enable_9 || enable_10 || enable_11 || enable_12 || enable_nan;
											if(!enable_bosses)
											{
												paused = true;
												SND_Pause(1);
											}
										}
										
										// basic crap
										if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
										{
											//DebugLog("Cross Received");
											if(!board[cursorX][cursorY].active) continue;
											if(board[cursorX][cursorY].ice_block) continue;
											
											bool enable_bosses = enable_9 || enable_10 || enable_11 || enable_12 || enable_nan;
											if(!free_play && test_zero && enable_bosses)
											{
												if((LEVEL==8 && EPISODE!=4) || (LEVEL==0 && EPISODE==7))
												{
													if(boss9_cutscene!=0) continue;
													if(cursorX==GRID_W/2 && cursorY==GRID_H/2)
													{
														enable_9 = true;
														boss9_cutscene = 1;
														the_bg = 4;
														desired_bgm = 10;
														PlayBGM(10, 1);
														test_zero = false;
														if(!seen_nine || (global_route==1 && !seen_fnine)) show_warning = true;
													}
												}
												else if((LEVEL==9 && EPISODE!=4) || (LEVEL==2 && EPISODE==4) || (LEVEL==1 && EPISODE==7))
												{
													if(boss10_cutscene!=0) continue;
													if(cursorX==GRID_W/2 && cursorY==GRID_H/2)
													{
														enable_10 = true;
														boss10_cutscene = 1;
														the_bg = 9;
														desired_bgm = 12;
														PlayBGM(12, 1);
														test_zero = false;
														if(!seen_ten || (global_route==1 && !seen_nten)) show_warning = true;
													}
												}
												else if((LEVEL==10 && EPISODE!=4) || (LEVEL==3 && EPISODE==5) || (LEVEL==2 && EPISODE==7))
												{
													if(boss11_cutscene!=0) continue;
													if(cursorX==GRID_W/2 && cursorY==GRID_H/2)
													{
														enable_11 = true;
														boss11_cutscene = 1;
														the_bg = 14;
														desired_bgm = 15;
														PlayBGM(15, 1);
														test_zero = false;
														if(!seen_eleven || (global_route==1 && !seen_meleven)) show_warning = true;
													}
												}
												else if((LEVEL==3 && (EPISODE==4 || EPISODE==7)))
												{
													if(boss12_cutscene!=0) continue;
													if(cursorX==GRID_W/2 && cursorY==GRID_H/2)
													{
														enable_12 = true;
														boss12_cutscene = 1;
														desired_bgm = 19;
														PlayBGM(19, 1);
														test_zero = false;
														if(!seen_twelve || (global_route==1 && !seen_ktwelve)) show_warning = true;
													}
												}
												else if((LEVEL==4 && (EPISODE==5 || EPISODE==7)))
												{
													if(bossnan_cutscene!=0) continue;
													if(cursorX==GRID_W/2 && cursorY==GRID_H/2)
													{
														enable_nan = true;
														bossnan_cutscene = 1;
														desired_bgm = 22;
														PlayBGM(22, 1);
														test_zero = false;
														if(!seen_nan || (global_route==1 && !seen_q)) show_warning = true;
													}
												}
												else test_zero = false;
												
												continue;
											}
											else if(free_play && test_zero && enable_bosses)
											{
												test_zero = false;
												continue;
											}
											else if(!enable_bosses) test_zero = false;
											
											if(!test_zero)
											{
												if(FIRST_CLICK) timer_running = 1;
												
												if(board[cursorX][cursorY].mine && FIRST_CLICK && setting_guarantee == 1)
												{
													do
													{
														GenerateBoard();
														CalculateNumbers();
													}
													while(board[cursorX][cursorY].mine);
												}
												if((board[cursorX][cursorY].mine || board[cursorX][cursorY].neighbors!=0) && FIRST_CLICK && setting_guarantee == 0)
												{
													do
													{
														GenerateBoard();
														CalculateNumbers();
													}
													while(board[cursorX][cursorY].mine || board[cursorX][cursorY].neighbors!=0);
												}
												
												if(HELL && FIRST_CLICK)
												{
													boss9_started = 1;
													boss10_started = 1;
													boss11_started = 1;
													boss12_started = 1;
													
													int i, j, k;
													for (i = 0; i < boss9_count; i++)
													{
														boss9_list[i].active = true;
													}
													for (j = 0; j < boss10_count; j++)
													{
														boss10_list[j].active = true;
													}
													for (k = 0; k < boss11_count; k++)
													{
														boss11_list[k].active = true;
													}
													
													boss12.active = true;
													PlayBGM(195, 1);
												}
												
												int bonked = 0;
												
												if(enable_11 && boss11_started)
												{
													int i;
													for (i = 0; i < boss11_count; i++)
													{
														if(boss11_list[i].x == cursorX && boss11_list[i].y == cursorY && !boss11_list[i].placed)
														{
															boss11_list[i].bonk = true;
															boss11_list[i].placed = 1;
															boss11_list[i].timer = 3.01f;
															bonked = 1;
															PlaySFX(&sfx_11bonk);
															continue;
														}
													}
												}
												
												bool nine_mines_intro = false;
												
												if(enable_9)
												{
													if(!boss9_started)
													{
														for (int i = 0; i < boss9_count; i++)
														{
															for(int x = boss9_list[i].x - 1; x <= boss9_list[i].x + 1; x++)
															{
																for(int y = boss9_list[i].y - 1; y <= boss9_list[i].y + 1; y++)
																{
																	if(cursorX == x && cursorY == y)
																	{
																		nine_mines_intro = true;
																		FIRST_CLICK = 1;
																	}
																}
															}
														}
														
														if(!nine_mines_intro)
														{
															boss9_started = 1;
															
															int i;
															for (i = 0; i < boss9_count; i++)
															{
																boss9_list[i].active = true;
															}
														}
													}
												}
												if(enable_10 && !boss10_started)
												{
													boss10_started = 1;
													
													int i;
													for (i = 0; i < boss10_count; i++)
													{
														boss10_list[i].active = true;
													}
												}
												if(enable_11 && !boss11_started)
												{
													boss11_started = 1;
													
													int i;
													for (i = 0; i < boss11_count; i++)
													{
														boss11_list[i].active = true;
													}
												}
												if(enable_12 && !boss12_started)
												{
													boss12_started = 1;
													boss12.active = true;
												}
												if(enable_nan && !bossnan_started)
												{
													bossnan_started = 1;
													
													int i;
													for (i = 0; i < bossnan_count; i++)
													{
														bossnan_list[i].active = true;
													}
												}
												
												if(!bonked && !nine_mines_intro)
												{
													if (board[cursorX][cursorY].revealed) Chord(cursorX, cursorY);
														else Reveal(cursorX, cursorY);
												}
												
												if(free_play && !HELL && FIRST_CLICK)
												{
													if(desired_bgm == 10)
													{
														PlayBGM(11, 1);
														desired_bgm = 9;
													}
													else if(desired_bgm == 12)
													{
														PlayBGM(13, 1);
														desired_bgm = 10;
													}
													else if(desired_bgm == 15)
													{
														PlayBGM(16, 1);
														desired_bgm = 11;
													}
													else if(desired_bgm == 19)
													{
														PlayBGM(20, 1);
														desired_bgm = 12;
													}
													else if(desired_bgm == 22)
													{
														PlayBGM(23, 1);
														desired_bgm = 19;
													}
												}
												else if(!free_play && FIRST_CLICK && LEVEL != 66)
												{
													if(enable_9)
													{
														if(global_route==3) PlayBGM(11, 0);
														else PlayBGM(11, 1);
														desired_bgm = 9;
													}
													else if(enable_10)
													{
														PlayBGM(13, 1);
														desired_bgm = 10;
													}
													else if(enable_11)
													{
														PlayBGM(16, 1);
														desired_bgm = 11;
													}
													else if(enable_12)
													{
														PlayBGM(20, 1);
														desired_bgm = 12;
													}
													else if(enable_nan)
													{
														PlayBGM(23, 1);
														desired_bgm = 19;
													}
												}
												
												if(desired_bgm == 24)
												{
													desired_bgm = 10;
													PlayBGM(25, 1);
												}
												
												FIRST_CLICK = 0;
												
												if(!enable_9) WinCheck();
											}
										}
										if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
										{
											//DebugLog("Circle Received");
											if((LEVEL==10 && !boss11_started) || (free_play && boss11_cutscene && !boss11_started)) continue;
											
											if(!board[cursorX][cursorY].revealed)
											{
												if(board[cursorX][cursorY].flagged == 0) mines_count--;
												board[cursorX][cursorY].flagged++;
												
												if(board[cursorX][cursorY].flagged > 4)
												{
													board[cursorX][cursorY].flagged = 0;
													PlaySFX(&sfx_unflag);
													mines_count++;
													
													if(board[cursorX][cursorY].mine && (LEVEL==10 || (free_play && boss11_cutscene)))
													{
														board[cursorX][cursorY].revealed = 1;
														game_lost();
													}
												}
												else
												{
													PlaySFX(&sfx_flag);
													if(!enable_half && board[cursorX][cursorY].flagged == 1) board[cursorX][cursorY].flagged = 2;
													if(!enable_double && board[cursorX][cursorY].flagged == 3) board[cursorX][cursorY].flagged = 4;
													if(!enable_anti && board[cursorX][cursorY].flagged == 4)
													{
														board[cursorX][cursorY].flagged = 0;
														PlaySFX(&sfx_unflag);
														mines_count++;
														
														if(board[cursorX][cursorY].mine && (LEVEL==10 || (free_play && boss11_cutscene)))
														{
															board[cursorX][cursorY].revealed = 1;
															game_lost();
														}
													}
												}
											}
										}
										
										cursor_x += (control_rightR-control_leftR)*2*R_SENSITIVITY;
										cursor_y += (control_downR-control_upR)*2*R_SENSITIVITY;
										
										if(paddata.BTN_R3 && !oldpad.BTN_R3 && boss10_started)
										{
											i_10 = -1;
											
											for(int zi = 0; zi < boss10_count; zi++)
											{
												float temp_x = boss10_list[zi].x;
												float temp_y = boss10_list[zi].y;
												if(cursor_x > temp_x-48 && cursor_x < temp_x+48)
												{
													if(cursor_y > temp_y-48 && cursor_y < temp_y+48)
													{
														i_10 = zi;
														continue;
													}
												}
											}
											
											if(i_10==-1) continue;
											
											boss10_list[i_10].time=10;
											boss10_list[i_10].tspeed+=(SECOND*(1+DIFFICULTY));
											tnt_alpha += 5.1f;
											float min_speed = boss10_list[i_10].tspeed;
											
											boss10_list[i_10].hsp = min_speed + (((float)rand() / (float)RAND_MAX) * boss10_list[i_10].tspeed);
											boss10_list[i_10].vsp = min_speed + (((float)rand() / (float)RAND_MAX) * boss10_list[i_10].tspeed);
											
											PlaySFX(&sfx_10reset);
											
											if(rand() % 2) boss10_list[i_10].hsp = -boss10_list[i_10].hsp;
											if(rand() % 2) boss10_list[i_10].vsp = -boss10_list[i_10].vsp;
										}
										
										if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE && boss9_started && global_route!=3) WinCheck();
										
										// movement
										if(control_left && !old_control_left) MoveCursor(-1, 0);
										if(control_right && !old_control_right) MoveCursor(1, 0);
										if(control_up && !old_control_up) MoveCursor(0, -1);
										if(control_down && !old_control_down) MoveCursor(0, 1);
										
										// zoom check
										if(paddata.BTN_R1 && !oldpad.BTN_R1) zoomCheck(1);
										if(paddata.BTN_L1 && !oldpad.BTN_L1) zoomCheck(0);
									}
								}
							}
							break;
						}
						case ENDING:
						{
							int max_lvl = episodes[EPISODE-1].level_count;
							if(start_end)
							{
								temp_tz = 0;
								temp_t = 0;

								show_shit = false;
								time_total = 0.0f;
								
								for(int _t = 0; _t < max_lvl; _t++)
								{
									time_total += level_time[_t];
								}
								
								rank_result = calculate_rank(time_total);
								DebugLog("RANK: %d\n", rank_result);
								
								global_minecoins += current_minecoins;
								
								if(EPISODE==1) ep2_unlock = true;
								else if(EPISODE==2) ep3_unlock = true;
								else if(EPISODE==3) ep4_unlock = true;
								else if(EPISODE==4) ep5_unlock = true;
								else if(EPISODE==5) ep6_unlock = true;
								else if(EPISODE==6) ep7_unlock = true;
								
								if(global_route==0) CollectAchievement(EPISODE-1);
								else if(global_route==1) CollectAchievement(6+EPISODE);
								
								LEVEL=0;
								StartSave();
								start_end = 0;
								PlayBGM(193, 0);
								ready_to_click = 1;
								end_time = 0;
							}
							
							temp_tz++;
							temp_t = temp_tz / 2;
							
							if(temp_t>100)
							{
								if(temp_t>384)
								{
									if(temp_t>484)
									{
										if(temp_t>916)
										{
											if(temp_t>964)
											{
												if(temp_t>1012)
												{
													if(temp_t>1152) show_shit = true;
													end_time = 6 + max_lvl;
												}
												else end_time = 5 + max_lvl;
											}
											else end_time = 4 + max_lvl;
										}
										else
										{
											int s = (temp_t-460) / 24;
											if(s>=max_lvl) s = max_lvl;
											end_time = 3+s;
										}
									}
									else end_time = 2;
								}
								else end_time = 1;
							}
							
							if(show_shit && ready_to_click)
							{
								if(paddata.BTN_START && !oldpad.BTN_START)
								{
									StopAudio();
									PlayBGM(198, 0); // to prevent many sfx playing idk
									PlaySFX(&sfx_gamestart);
									if(mistakes == 0 && EPISODE == 1 && DIFFICULTY>0 && !route_touhou_unlocked)
									{
										route_touhou_unlocked = true;
										DebugLog("Touhou Route Unlocked!");
										CollectAchievement(7);
										StartSave();
									}
									
									if(EPISODE == 1 && rank_result==0 && !route_weird_unlocked)
									{
										route_weird_unlocked = true;
										DebugLog("Weird Route Unlocked!");
										CollectAchievement(22);
										StartSave();
									}

									ready_to_click = 0;
									splash = splashes_random[rand() % SplashesCount()];
									start_music = 1;
									FadeInOut(&currentScreen, TITLE);
								}
							}
							
							break;
						}
						case GOTOSHOP:
						{
							if(startbgmgtsp)
							{
								startbgmgtsp = 0;
								PlayBGM(9, 0);
							}
							
							ship_r=0;
							if(ship_x<-40.0f+(32*15)) ship_x+=ship_speed*2;
							else
							{
								if(ship_x <= -38.0f+(32*15))
								{
									start_shop = 1;
									FadeInOut(&currentScreen, SHOP);
									ship_x+=ship_speed*2;
								}
							}
							break;
						}
						case SHOP:
						{
							if(start_shop)
							{
								start_shop = 0;
								PlayBGM(197, 1);
								ready_to_click = 1;
							}
							
							if(ready_to_click)
							{
								if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
								{
									ready_to_click = 0;
									start_menu = 1;
									FadeInOut(&currentScreen, MENU);
								}
							}
							
							break;
						}
						case CREDITS:
						{
							if(start_credits)
							{
								specialthanks = false;
								start_credits = 0;
								
								if(RandRange(0, 10) == 3)
								{
									PlayBGM(191, 1);
									easter_egg = true;
								}
								else PlayBGM(192, 1);
								
								ready_to_click = 1;
							}
							
							if(ready_to_click)
							{
								if(control_left && !old_control_left) specialthanks = false;
								if(control_right && !old_control_right) specialthanks = true;
								
								if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
								{
									StopAudio();
									PlaySFX(&sfx_gamestart);
									ready_to_click = 0;
									splash = splashes_random[rand() % SplashesCount()];
									start_music = 1;
									FadeInOut(&currentScreen, TITLE);
								}
							}
							
							break;
						}
						case ENDING2:
						{
							if(start_end)
							{
								temp_tz = 0;
								temp_t = 0;
								
								DebugLog("RANK: %d\n", rank_result);
								start_end = 0;
								PlayBGM(189, 0);
								ready_to_click = 1;
								end_time = 0;
								
								temp_t2 = false;
							}
							
							temp_tz++;
							
							temp_t = temp_tz / 2;
							
							if(temp_t>100)
							{
								if(temp_t>200 && !temp_t2)
								{
									temp_t2 = true;
									ResetGridTest();
								}
								else end_time = 1;
							}
						}
						case ROUTE_SELECT:
						{
							int old_route = global_route;
							
							if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
							{
								switch(route_selector)
								{
									case 1:
									{
										if(route_touhou_unlocked)
										{
											RSX_SetGameBoyEnabled(0);
											global_route = 1;
											global_route_tex = 21;
										}
										else
										{
											msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
												"To unlock the Touhou route, You must complete the Episode 1 with no mistakes in Novice and higher difficulty.", EmptyDialogCallback, NULL, NULL);
										}
										break;
									}
									case 2:
									{
										global_route = 2;
										global_route_tex = 1;
										RSX_SetGameBoyEnabled(1);
										break;
									}
									case 3:
									{
										if(route_weird_unlocked)
										{
											RSX_SetGameBoyEnabled(0);
											msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
												"Warning.\nThis route contains so many sensitive jokes, bad language, weird images and some more.\nAlso it's WIP.\nProceed with caution.", EmptyDialogCallback, NULL, NULL);
											global_route = 3;
											global_route_tex = 23;
										}
										else
										{
											msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
												"To unlock the Weird route, You must complete the Episode 1 with the lowest rank on any difficulty.", EmptyDialogCallback, NULL, NULL);
										}
										break;
									}
									case 4:
									{
										if(route_playstation_unlocked)
										{
											msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
												"PlayStation Route is in development. Please wait...", EmptyDialogCallback, NULL, NULL);
										}
										else
										{
											msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
												"To unlock the PlayStation route, You must uh... wait.", EmptyDialogCallback, NULL, NULL);
										}
										break;
									}
									case 5:
									{
										msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
											"Ultrakill Route is in development. Please wait...", EmptyDialogCallback, NULL, NULL);
										break;
									}
									default:
									{
										RSX_SetGameBoyEnabled(0);
										global_route = route_selector;
										global_route_tex = 1;
										break;
									}
								}
								if(old_route!=global_route) ReplaceBGM(fp_music);
							}
							
							if(control_left && !old_control_left)
							{
								route_selector--;
								if(route_selector<0) route_selector = 0;
							}
							if(control_right && !old_control_right)
							{
								route_selector++;
								if(route_selector>max_route) route_selector = max_route;
							}
							
							if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE) currentScreen = MENU;
							
							break;
						}
						case VIDEOTEST:
						{
							if(start_video)
							{
								start_video = false;
								video_finished = false;
								if(MP4Player_Init(&vid, "/dev_hdd0/game/MINESWEEP/USRDIR/data/weirdroute.mp4", VIDEO_TEX)) PlayBGM(188, 0);
								else
								{
									DebugLog("Video load failed\n");
									video_finished = true;
									FadeInOut(&currentScreen, MAP);
								}
							}
							
							if(!video_finished)
							{
								int r = MP4Player_Tick(&vid);
								if(r == -1)
								{
									DebugLog("Video finished\n");
									video_finished = true;
									MP4Player_Close(&vid);
									FadeInOut(&currentScreen, MAP);
								}
								
								if(control_any && !old_control_any)
								{
									DebugLog("FORCED Video finish\n");
									video_finished = true;
									FadeInOut(&currentScreen, MAP);
								}
							}
							break;
						}
						default: break;
					}
					
					old_control_left = control_left;
					old_control_right = control_right;
					old_control_up = control_up;
					old_control_down = control_down;
					old_control_leftR = control_leftR;
					old_control_rightR = control_rightR;
					old_control_upR = control_upR;
					old_control_downR = control_downR;
					old_control_any = control_any;
					oldpad = paddata;
				}
			}
		}

		if(boss9_started && !disable_control && enable_9)
		{
			int i;
			for (i = 0; i < boss9_count; i++)
			{
				boss9_list[i].timer--;

				if(boss9_list[i].timer <= 0)
				{
					boss9_list[i].timer = 6;
					UpdateBoss9(i);
				}
				
				if(global_route == 1) UpdateFrozenNine();
			}
		}
		
		if(boss9_count>0)
		{
			int i;
			for (i = 0; i < boss9_count; i++)
			{
				boss9_list[i].rotation += ((float)boss9_list[i].angle * boss9_list[i].speed);
			}
		}
		
		boss12.rotation += 1;
		
		if(timer_running && !paused)
		{
			timer_seconds += SECOND;
			if(time_decrease && !gamelose)
			{
				time_left -= SECOND;
				if(enable_9 && global_route!=3) time_left -= (local_mistakes * SECOND);
				if(time_left <= 0.99f)
				{
					timer_running = 0;
					DebugLog("TIME OVER\n");
					if(global_route!=3)
					{
						if(free_play) fp_lifes=0; else LIFES=0;
						game_lost();
					}
					else WinCheck();
				}
			}
		}
		
		if(boss10_started && enable_10)
		{
			int i;

			for (i = 0; i < boss10_count; i++)
			{
				if(!gamelose)
				{
					boss10_list[i].timer += boss10_list[i].speed;
					
					if(boss10_list[i].time <= 5 && boss10_list[i].time > 0)
					{
						boss10_list[i].beep_timer -= boss10_list[i].speed;
						
						if(boss10_list[i].beep_timer <= 0.0f)
						{
							PlaySFX(&sfx_beep);
							float t = 60.0f * SECOND;
							switch(boss10_list[i].time)
							{
								case 5: boss10_list[i].beep_timer = t / 1.5f; break;
								case 4: boss10_list[i].beep_timer = t / 2.0f; break;
								case 3: boss10_list[i].beep_timer = t / 4.0f; break;
								case 2: boss10_list[i].beep_timer = t / 10.0f; break;
								case 1: boss10_list[i].beep_timer = t / 15.0f; break;
							}
						}
					}
					
					if(boss10_list[i].timer > 1.0f)
					{
						boss10_list[i].timer = 0.0f;
						if(boss10_list[i].time<=1 && boss10_list[i].speed!=0)
						{
							boss10_list[i].time = 0;
							boss10_list[i].speed = 0;
							boss10_list[i].vsp = 0;
							boss10_list[i].hsp = 0;
							if(free_play) fp_lifes=0; else LIFES=0;
							game_lost();
						}
						else boss10_list[i].time--;
					}
					
					if(global_route==1 && tnt_alpha >= 249.0f && tnt_alpha < 255.0f)
					{
						tnt_alpha = 255.0f;
						if(free_play) fp_lifes=0; else LIFES=0;
						game_lost();
					}
				}
				
				boss10_update(i);
			}
		}
		
		if(boss11_started && !disable_control && enable_11)
		{
			int i;
			for (i = 0; i < boss11_count; i++)
			{
				boss11_update(i);
			}
		}
		
		if(boss12_started && enable_12) boss12_update(disable_control);
		
		if(bossnan_started && !disable_control && enable_nan)
		{
			int i;
			for (i = 0; i < bossnan_count; i++)
			{
				if(!gamelose && corruption>49.9f)
				{
					if(free_play) fp_lifes=0; else LIFES=0;
					game_lost();
				}
				bossnan_update(i);
			}
		}
		
		if(disable_control && !gamelose && !gamewin)
		{
			if(explode_time>0) explode_time--;
			else
			{
				can_move_xd = 0;
				disable_control = 0;
				
				bool enable_bosses = enable_9 || enable_10 || enable_11 || enable_12 || enable_nan;
				
				if(free_play && !enable_bosses) fp_lifes--;
				else
				{
					mistakes++;
					local_mistakes++;
					if(!enable_bosses) LIFES--;
				}
				
				CalculateNumbers();
				SpawnProjectile(offset_x+(cursorX*RENDER_SIZE), offset_y+(cursorY*RENDER_SIZE), 0, 0, 0, 1.0f);
				if((fp_lifes < 1 && free_play) || (!free_play && LIFES < 1))
				{
					if(!free_play && DIFFICULTY==0 && LEVEL==0 && master_mode) CollectAchievement(15);
					disable_control = 1;
					gamelose = 1;
					if(global_route==1 && enable_10) tnt_alpha = 240.0f;
					DeactivateBosses();
					timer_running = 0;
					playing_bgm = false;
					StopAudio();
				}
			}
		}
		
		if(game_init && !game_loaded)
		{
			ioPadGetInfo(&padinfo);
			for(ip = 0; ip < MAX_PADS; ip++)
			{
				if(padinfo.status[ip])
				{
					ioPadGetData(ip, &paddata);
					if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
					{
						splash = splashes_random[RandRange(0, SplashesCount())];
						DebugLog("LOGOSHOW\n");
						
						FILE *fp = fopen("/dev_hdd0/game/MINESWEEP/USRDIR/ultra/killer.txt", "w");
						if(fp)
						{
							max_route = 5;
							fprintf(fp, "Ultrakill DLC Initialized!");
							fclose(fp);
						}
						show_logo=false;
						HolyShit2(2);
						PlayBGM(198, 1);
						game_loaded = 1;
					}
					oldpad = paddata;
				}
			}
		}
		
		if(playing_bgm) UpdateBGM();
		if(bad_apple) badAppleUpdate();
		if(show_ach)
		{
			static int ach_show_time = 0;
			ach_show_time++;
			if(ach_show_time>=210)
			{
				ach_show_time=0;
				show_ach = false;
			}
		}
		
		waitFlip();
		
		RSX_BeginFrame();
		
		RSX_SetRenderTarget(&buffers[currentBuffer]);
		RSX_SetupViewport();
		RSX_ClearScreen();

		drawScene();
		RSX_DrawCRT();
		RSX_FlushDraws();
		RSX_EndFrameMark();
		
		if (!flip(RSX_GetContext(), buffers[currentBuffer].id)) DebugLog("RSX: test flip failed!\n");
		currentBuffer = (currentBuffer + 1) % MAX_BUFFERS;
    }
	
	MP4Player_Close(&vid);
	rsxFinish(RSX_GetContext(), 1);
	StopAudio();
	ioPadEnd();
	sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT0);
    return 0;
}