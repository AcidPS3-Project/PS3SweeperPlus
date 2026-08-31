#include <tiny3d.h>

#include "soundlib/spu_soundlib.h"
#include "soundlib/audioplayer.h"

#include "bosses.h"
#include "level_data.h"
#include "save.h"
#include "drawing.h"
#include "video.h"

bool play_map_bgm = false;

int start_menu = 0;
int start_game_mus = 0;
int startbgmgtsp = 0;
int start_shop = 0;
int start_end = 0;
int start_credits = 0;

int LEVEL_BKP = 0;
int ready_to_click = 0;

bool HELL = false;

void ResetGrid()
{
	start_game_mus = 1;
	FadeInOut(&currentScreen, GAME);
	
    mines_count = MINES;

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
	
	if(free_play && desired_bgm == 9) enable_9 = true;
	if(free_play && desired_bgm == 10) enable_10 = true;
	if(free_play && desired_bgm == 11) enable_11 = true;
	if(free_play && desired_bgm == 14) enable_12 = true;
	if(free_play && desired_bgm == 15) enable_nan = true;
	
	if(LEVEL==8) enable_9 = true;
	if(LEVEL==9) enable_10 = true;
	if(LEVEL==10) enable_11 = true;
	if(LEVEL==11) enable_12 = true;
	if(LEVEL==12) enable_nan = true;
	
	if(enable_9 && enable_10 && enable_11 && enable_12) HELL = true;
	
	if(enable_9)
	{
		SetupBoss9Positions(boss9_count);
		if(free_play)
		{
			desired_bgm = 10;
			boss9_cutscene = 2;
		}
		// msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON, "There is a boss battle in this game. The battle works the same as normal Minesweeper, but with some key distinctions:\n"
			// "you won't know the number of mines hidden on the grid, you have limited time to complete the level, and you won't be able to use the sonar or pause the game.\n"
			// "Clicking on a mine will make the time run out faster. To win, you must click SQUARE ONLY AFTER revealing all the tiles that don't contain mines.\n"
			// "If the timer hits 0, or you click on the boss before revealing the entire grid, it's game over.", EmptyDialogCallback, NULL, NULL);
	}
	if(enable_10)
	{
		SetupBoss10Positions(boss10_count);
		if(free_play)
		{
			desired_bgm = 12;
			boss10_cutscene = 2;
		}
		// msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
			// "There is another boss battle in this game. As soon as the game starts, this boss will start counting down from 10.\n"
			// "If it counts all the way down to 0, it's game over. Click SQUARE to reset the countdown back to 10, but be careful.\n"
			// "The boss will move faster and faster every time you click. Clicking on a mine will cause the boss to count down more quickly.\n"
			// "To win, reveal the entire grid before the boss is able to finish its countdown.", EmptyDialogCallback, NULL, NULL);
	}
	if(enable_11)
	{
		SetupBoss11Positions(boss11_count);
		if(free_play)
		{
			desired_bgm = 15;
			boss11_cutscene = 2;
		}
		// msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
			// "ELEVEN TEMP DESCRIPTION", EmptyDialogCallback, NULL, NULL);
	}
	if(enable_12)
	{
		SetupBoss12Position();
		if(free_play)
		{
			desired_bgm = 19;
			boss12_cutscene = 2;
		}
		// msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
			// "TWELVE TEMP DESCRIPTION", EmptyDialogCallback, NULL, NULL);
	}
	if(enable_nan)
	{
		SetupBossNaNPositions(bossnan_count);
		if(free_play)
		{
			desired_bgm = 19;
			bossnan_cutscene = 2;
		}
		// msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
			// "NAN TEMP DESCRIPTION", EmptyDialogCallback, NULL, NULL);
	}
	
	int j;
	for (j = 0; j < boss10_count; j++)
	{
		boss10_list[j].time = 10;
		boss10_list[j].pulse = 0.0f;
		boss10_list[j].speed = 0.018f;
		boss10_list[j].timer = 0.0f;
		boss10_list[j].tspeed = 0.0f;
	}
	
    GenerateBoard();
    CalculateNumbers();
	
	if(desired_bgm!=100) playing_bgm = true;
	else playing_bgm = false;
}

void GoBackToTitle()
{
	current_minecoins = 0;
	
	enable_9 = false;
	enable_10 = false;
	enable_11 = false;
	enable_12 = false;
	enable_nan = false;
	
	if(free_play) LEVEL=LEVEL_BKP;
	
	bad_apple = 0;
	
	enable_half = 0;
	enable_double = 0;
	enable_anti = 0;
	
	if(temp_fp_music==100) fp_music = temp_fp_music;
	
	i_10 = 0;
	
	menu_pos = 0;
	menu_x = 0.0f;
	ze_x = 0.0f;
	ze_y = 0.0f;
	adventure_select = 0;
	
	StartSave();
	
	start_menu = 1;
	FadeInOut(&currentScreen, MENU);
}

void UnloadCustomSweeper()
{
	use_custom_sweeper = false;
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

int main(int argc, const char* argv[])
{
	padInfo padinfo;
	int ip;
	static padData oldpad;
	padData paddata;
	
	srand(time(NULL));
	tiny3d_Init(4*1024*1024);
	tiny3d_UserViewport(1, 0, 0, (float)(Video_Resolution.width / 916.0f), (float)(Video_Resolution.height / 582.0f),
		(float)(Video_Resolution.width / 1920.0f), (float)(Video_Resolution.height / 1080.0f));
	
	sysModuleLoad(SYSMODULE_PNGDEC);
	
	s32 ret = sysModuleLoad(SYSMODULE_VDEC);
	printf("[VDEC] sysModuleLoad(SYSMODULE_VDEC) = %08X\n", ret);
	
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
	
	ioPadInit(7);
    LoadTexture();
	LangInit();
	creditsInit();
	SetupTimeSettings();
	
	StartLoad();

	PS3Video_Init(&the_video, 1280, 720);
	
    while(1)
    {
		sysUtilCheckCallback();
		UpdateFPS();
		
		UpdateFade(1.0f / 60.0f);
		UpdateProjectiles(1.0f / 60.0f);
		
        tiny3d_Clear(0xff000000, TINY3D_CLEAR_ALL);
		tiny3d_AlphaTest(1, 0x10, TINY3D_ALPHA_FUNC_GEQUAL);
		tiny3d_BlendFunc(1, TINY3D_BLEND_FUNC_SRC_RGB_SRC_ALPHA | TINY3D_BLEND_FUNC_SRC_ALPHA_SRC_ALPHA,
            TINY3D_BLEND_FUNC_DST_RGB_ONE_MINUS_SRC_ALPHA | TINY3D_BLEND_FUNC_DST_ALPHA_ZERO,
            TINY3D_BLEND_RGB_FUNC_ADD | TINY3D_BLEND_ALPHA_FUNC_ADD);
		
		if(game_loaded)
		{
			ioPadGetInfo(&padinfo);
			for(ip = 0; ip < MAX_PADS; ip++)
			{
				if(padinfo.status[ip])
				{
					ioPadGetData(ip, &paddata);
					
					if(paddata.BTN_CROSS || paddata.BTN_CIRCLE) button_x_hold = true;
					else button_x_hold = false;
					
					if(paddata.BTN_START) button_start_hold = true;
					else button_start_hold = false;
					
					switch(currentScreen)
					{
						case LOGO:
						{
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
									if(language>=2) language=0; else language++;
									LangInit();
								}
								if(paddata.BTN_R2 && !oldpad.BTN_R2)
								{
									if(setting_guarantee>=2) setting_guarantee=0; else setting_guarantee++;
								}
								if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
								{
									splash = splashes_random[rand() % SplashesCount()];
									settings_select = 0;
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
								
								if(paddata.BTN_SQUARE && !oldpad.BTN_SQUARE)
								{
									settings_select = 1;
								}
								
								if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE)
								{
									start_credits = 1;
									ready_to_click = 0;
									FadeInOut(&currentScreen, CREDITS);
								}
								if(paddata.BTN_SELECT && !oldpad.BTN_SELECT)
								{
									start_end = 1;
									ready_to_click = 0;
									FadeInOut(&currentScreen, ENDING);
								}
								
								if(paddata.BTN_CROSS && !oldpad.BTN_CROSS) LEVEL=8;
							}
							break;
						}
						case MENU:
						{
							if(start_menu)
							{
								menu_x = 0.0f;
								ze_x = 0.0f;
								ze_y = 0.0f;
								start_menu = 0;
								
								disable_control = 0;
								can_move_xd = 1;
								
								free_play = 0;

								gamewin = 0;
								gamelose = 0;
								PlayBGM(50+fp_music, 1);
								paused = false;
								ready_to_click = 1;
							}
							
							menu_x = lerp(menu_x, -262 * menu_pos, 0.1f);
							
							if(ready_to_click)
							{
								if(!free_play)
								{
									if(challenge_select)
									{
										if(paddata.BTN_L2 && !oldpad.BTN_L2)
										{
											ready_to_click = 0;
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
											ready_to_click = 0;
											free_play = 1;
											fp_width = 16;
											fp_height = 16;
											fp_mines = 40;
											enable_9 = true;
											enable_10 = true;
											enable_11 = true;
											enable_12 = true;
											LEVEL_BKP = LEVEL;
											LoadFPLevel();
											can_move_xd = 1;
										}
										
										if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
										{
											challenge_select = 0;
										}
									}
									else if(loadout_select)
									{
										if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
										{
											loadout_select = 0;
										}
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
											ready_to_click = 0;
											ship_x = -80;
											ship_y = -44;
											if(paddata.BTN_R2) global_route = 1;
											LEVEL = 0;
											DIFFICULTY = DIFFICULTYs;
											EPISODE = EPISODEs;
											LIFES = 3;
											
											if(paddata.BTN_L2)
											{
												int max_lvl = episodes[EPISODE-1].level_count;
												LEVEL = max_lvl-1;
											}
											
											current_minecoins = 0;
											mistakes = 0;
											SetupTimeSettings();
											StopAudio();
											PlaySFX(&sfx_gamestart);
											FadeInOut(&currentScreen, MAP);
											ship_a = true;
										}
										if(paddata.BTN_UP && !oldpad.BTN_UP)
										{
											DIFFICULTYs--;
											if (DIFFICULTYs<0) DIFFICULTYs = 0;
										}
										if(paddata.BTN_DOWN && !oldpad.BTN_DOWN)
										{
											DIFFICULTYs++;
											if (DIFFICULTYs>3) DIFFICULTYs = 3;
										}
										if(paddata.BTN_LEFT && !oldpad.BTN_LEFT)
										{
											EPISODEs--;
											if (EPISODEs<1) EPISODEs = 1;
										}
										if(paddata.BTN_RIGHT && !oldpad.BTN_RIGHT)
										{
											EPISODEs++;
											if (EPISODEs>episode_count) EPISODEs = episode_count;
										}
										if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
										{
											adventure_select = 0;
										}
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
											ready_to_click = 0;
											free_play = 0;
											ship_x = 120.0f+(64*LEVEL);
											StopAudio();
											FadeInOut(&currentScreen, MAP);
											PlaySFX(&sfx_gamestart);
											ship_a = true;
										}
										
										if(paddata.BTN_LEFT && !oldpad.BTN_LEFT)
										{
											menu_pos--;
											if (menu_pos<0) menu_pos = 0;
										}
										if(paddata.BTN_RIGHT && !oldpad.BTN_RIGHT)
										{
											menu_pos++;
											if (menu_pos>3) menu_pos = 3;
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
									if(paddata.BTN_START && !oldpad.BTN_START)
									{
										ready_to_click = 0;
										LEVEL_BKP = LEVEL;
										PlaySFX(&sfx_gamestart);
										LoadFPLevel();
										can_move_xd = 1;
									}
									
									if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
									{
										free_play = 0;
									}
									
									if(paddata.BTN_L1 && !oldpad.BTN_L1)
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
													memcpy(texture_pointer, spritesheet[13].bmp_out, spritesheet[13].pitch * spritesheet[13].height);
													sprite_off[13] = tiny3d_TextureOffset(texture_pointer);
													free(spritesheet[13].bmp_out);
													spritesheet[13].bmp_out = NULL;
													
													texture_pointer += ((spritesheet[13].pitch * spritesheet[13].height + 15) & ~15) / 4;
													
													use_custom_sweeper = true;
												}
											}
										}
										else UnloadCustomSweeper();
									}
									
									if(paddata.BTN_UP && !oldpad.BTN_UP)
									{
										fp_menu--;
										if (fp_menu<0) fp_menu = 0;
									}
									if(paddata.BTN_DOWN && !oldpad.BTN_DOWN)
									{
										fp_menu++;
										if (fp_menu>7) fp_menu = 7;
									}
									
									int max_mus = 14;
									
									if(paddata.BTN_LEFT && !oldpad.BTN_LEFT)
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
									if(paddata.BTN_RIGHT && !oldpad.BTN_RIGHT)
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
											ready_to_click = 0;
											LEVEL_BKP = LEVEL;
											DebugLog("Loaded Bad Apple!!");
											PlaySFX(&sfx_gamestart);
											LoadFPLevel();
										}
									}
									
									if(fp_menu==6 && paddata.BTN_SQUARE && !oldpad.BTN_SQUARE)
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
											
											bgm[99] = (unsigned char *)custom_main_path;
											fp_music = 99;

											if(!has_custom_win)
											{
												msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
													langStr[51], EmptyDialogCallback, NULL, NULL);
												continue;
											}
											bgm[199] = (unsigned char *)custom_win_path;
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
							break;
						}
						case MAP:
						{
							if(!play_map_bgm)
							{
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
										else StopAudio();
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
											case 10: StopAudio(); break;
										}
										break;
									}
									default: break;
								}
								
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
											if(ship_x < 120.0f+(64*(LEVEL+1))) ship_x+=0.75f;
											else ship_a = false;
										}
										else
										{
											if(ship_x < 1314.0f) ship_x+=0.75f;
											else ship_a = false;
										}
										
										if(ship_x > 876.0f && ship_x < 877.0f) PlayBGM(159, 1);
										
										break;
									}
									case 2:
									{
										if(LEVEL == 0 && (ship_x < 88.0f+(64*4))) ship_x+=0.75f;
										else if(LEVEL == 1 && (ship_x < 88.0f+(64*5))) ship_x+=0.75f;
										else if(LEVEL == 2 && (ship_x < 88.0f+(64*6))) ship_x+=0.75f;
										else if(LEVEL == 3)
										{
											if(ship_x < 88.0f+(64*6)+32) ship_x+=0.75f;
											else
											{
												ship_r = 1;
												if(ship_y < -22.0f+(32*10)) ship_y+=0.75f;
												else ship_a = false;
											}
										}
										else if(LEVEL == 4)
										{
											ship_r = 1;
											if(ship_y < -22.0f+(32*12)) ship_y+=0.75f;
											else ship_a = false;
										}
										else if(LEVEL == 5)
										{
											if(ship_y < -22.0f+(32*13))
											{
												ship_r = 1;
												ship_y+=0.75f;
											}
											else
											{
												ship_r = 2;
												if(ship_x > 88.0f+(64*5)) ship_x-=0.75f;
												else ship_a = false;
											}
										}
										else if(LEVEL == 6)
										{
											ship_r = 2;
											if(ship_x > 88.0f+(64*4)) ship_x-=0.75f;
											else ship_a = false;
										}
										else if(LEVEL == 7)
										{
											if(ship_x > 88.0f+(64*3)+32)												
											{
												ship_r = 2;
												ship_x-=0.75f;
											}
											else
											{
												ship_r = 3;
												if(ship_y > -22.0f+(32*10)) ship_y-=0.75f;
												else ship_a = false;
											}
										}
										else if(LEVEL == 8)
										{
											if(ship_y > -22.0f+(32*9))
											{
												ship_r = 3;
												ship_y-=0.75f;
											}
											else
											{
												ship_r = 0;
												if(ship_x < 88.0f+(64*5)) ship_x+=0.75f;
												else ship_a = false;
											}
										}
										else if(LEVEL == 9)
										{
											ten_arena++;
											if(ten_arena==160) PlaySFX(&sfx_blackout);
											else if(ten_arena==300) PlayBGM(160, 1);
											else if(ten_arena>300) ship_a = false;
										}
										else ship_a = false;
										break;
									}
									case 3:
									{
										if(LEVEL == 0 && (ship_y > -22.0f+(32*14))) ship_y-=0.75f;
										else if(LEVEL == 1 && (ship_y > -22.0f+(32*12))) ship_y-=0.75f;
										else if(LEVEL == 2 && (ship_y > -22.0f+(32*10))) ship_y-=0.75f;
										else if(LEVEL == 3)
										{
											if(ship_y > -22.0f+(32*9))
											{
												ship_r = 3;
												ship_y-=0.75f;
											}
											else
											{
												ship_r = 0;
												if(ship_x < 120.0f+(64*4)) ship_x+=0.75f;
												else ship_a = false;
											}
										}
										else if(LEVEL == 4)
										{
											ship_r = 0;
											if(ship_x < 120.0f+(64*5)) ship_x+=0.75f;
											else ship_a = false;
										}
										else if(LEVEL == 5)
										{
											if(ship_x < 88.0f+(64*6))
											{
												ship_r = 0;
												ship_x+=0.75f;
											}
											else
											{
												ship_r = 1;
												if(ship_y < -22.0f+(32*10)) ship_y+=0.75f;
												else ship_a = false;
											}
										}
										else if(LEVEL == 6)
										{
											ship_r = 1;
											if(ship_y < -22.0f+(32*12)) ship_y+=0.75f;
											else ship_a = false;
										}
										else if(LEVEL == 7)
										{
											if(ship_y < -22.0f+(32*15))
											{
												ship_r = 1;
												ship_y+=0.75f;
											}
											else
											{
												ship_r = 2;
												if(ship_x > 120.0f+(64*5)) ship_x-=0.75f;
												else ship_a = false;
											}
										}
										else if(LEVEL == 8)
										{
											if(ship_x > 88.0f+(64*5))												
											{
												ship_r = 2;
												ship_x-=0.75f;
											}
											else
											{
												ship_r = 3;
												if(ship_y > -22.0f+(32*12)) ship_y-=0.75f;
												else ship_a = false;
											}
										}
										else if(LEVEL == 9)
										{
											ship_r = 3;
											if(ship_y > -22.0f+(32*10)) ship_y-=0.75f;
											else ship_a = false;
										}
										else if(LEVEL == 10)
										{
											
										}
										else ship_a = false;
										break;
									}
									default: break;
								}
							}
							else if(ready_to_click)
							{
								if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
								{
									ready_to_click = 0;
									StopAudio();
									LoadLevel(LEVEL);
									
									if(LEVEL>=8) PlaySFX(&sfx_level2);
									else PlaySFX(&sfx_level);
									
									can_move_xd = 1;
								}
							}
							break;
						}
						case GAME:
						{
							if(start_game_mus)
							{
								play_map_bgm = false;
								start_game_mus = 0;
								if(HELL) desired_bgm = 194;
								PlayBGM(desired_bgm, 1);
								ready_to_click = 1;
								fp_lifes = 3;
							}
							
							if(ready_to_click)
							{
								if(gamelose && paddata.BTN_START && !oldpad.BTN_START) GoBackToTitle();
								else if(paused)
								{
									if(paddata.BTN_SELECT && !oldpad.BTN_SELECT)
									{
										ready_to_click = 0;
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
										if(free_play) LoadFPLevel();
										else
										{
											LEVEL++;
											if(EPISODE==4)
											{
												if(LEVEL<4) LEVEL=4;
												else if(LEVEL<8) LEVEL=8;
												else if(LEVEL<11) LEVEL=11;
											}
											GoBackToTitle();
										}
									}

									if(paddata.BTN_START && !oldpad.BTN_START)
									{
										ready_to_click = 0;
										if(!free_play)
										{
											int max_lvl = episodes[EPISODE-1].level_count;
											LEVEL++;
											
											if(EPISODE==4)
											{
												if(LEVEL<4) LEVEL=4;
												else if(LEVEL<8) LEVEL=8;
												else if(LEVEL<11) LEVEL=11;
											}

											if(LEVEL >= max_lvl)
											{
												start_end = 1;
												FadeInOut(&currentScreen, ENDING);
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
									if (!disable_control && can_move_xd && !bad_apple)
									{
										// pausing shit
										if(!paddata.BTN_START && oldpad.BTN_START)
										{
											if((!free_play && LEVEL<8) || (free_play && fp_music<9))
											{
												paused = true;
												SND_Pause(1);
											}
										}
										
										// basic crap
										if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
										{
											if(LEVEL==8 && boss9_cutscene==0)
											{
												if(cursorX==GRID_W/2 && cursorY==GRID_H/2)
												{
													boss9_cutscene = 1;
													the_bg = 4;
													PlayBGM(10, 1);
												}
											}
											else if(LEVEL==9 && boss10_cutscene==0)
											{
												if(cursorX==GRID_W/2 && cursorY==GRID_H/2)
												{
													boss10_cutscene = 1;
													the_bg = 9;
													PlayBGM(12, 1);
												}
											}
											else if(LEVEL==10 && boss11_cutscene==0)
											{
												if(cursorX==GRID_W/2 && cursorY==GRID_H/2)
												{
													boss11_cutscene = 1;
													the_bg = 14;
													PlayBGM(15, 1);
												}
											}
											else if(LEVEL==11 && boss12_cutscene==0)
											{
												if(cursorX==GRID_W/2 && cursorY==GRID_H/2)
												{
													boss12_cutscene = 1;
													PlayBGM(19, 1);
												}
											}
											else
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
												
												FIRST_CLICK = 0;
												
												int bonked = 0;
												
												if(boss11_started)
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
												
												if((LEVEL==8 && !boss9_started) || (free_play && boss9_cutscene==2 && !boss9_started))
												{
													boss9_started = 1;
													
													int i;
													for (i = 0; i < boss9_count; i++)
													{
														boss9_list[i].active = true;
													}
													
													if(!HELL && !custom_music) PlayBGM(11, 1);
													if(free_play) desired_bgm = 9;
												}
												else if((LEVEL==9 && !boss10_started) || (free_play && boss10_cutscene==2 && !boss10_started))
												{
													boss10_started = 1;
													
													int i;
													for (i = 0; i < boss10_count; i++)
													{
														boss10_list[i].active = true;
													}
													
													if(!HELL && !custom_music) PlayBGM(13, 1);
													if(free_play) desired_bgm = 10;
												}
												else if((LEVEL==10 && !boss11_started) || (free_play && boss11_cutscene==2 && !boss11_started))
												{
													boss11_started = 1;
													
													int i;
													for (i = 0; i < boss11_count; i++)
													{
														boss11_list[i].active = true;
													}
													
													StopAudio();
													if(!HELL && !custom_music) PlayBGM(16, 1);
													if(free_play) desired_bgm = 11;
												}
												else if((LEVEL==11 && !boss12_started) || (free_play && boss12_cutscene==2 && !boss12_started))
												{
													boss12_started = 1;
													boss12.active = true;
													
													if(!HELL && !custom_music) PlayBGM(20, 1);
													if(free_play) desired_bgm = 96;
												}
												
												if (bonked == 0)
												{
													if (board[cursorX][cursorY].revealed) Chord(cursorX, cursorY);
														else Reveal(cursorX, cursorY);
												}
												
												if(!boss9_started) WinCheck();
											}
										}
										if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
										{
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
										if(paddata.BTN_SQUARE && !oldpad.BTN_SQUARE)
										{
											if(boss10_started)
											{
												boss10_list[i_10].time=10;
												boss10_list[i_10].tspeed+=(0.0155f*(1+DIFFICULTY));
												float min_speed = boss10_list[i_10].tspeed;
												
												boss10_list[i_10].hsp = min_speed + (((float)rand() / (float)RAND_MAX) * boss10_list[i_10].tspeed);
												boss10_list[i_10].vsp = min_speed + (((float)rand() / (float)RAND_MAX) * boss10_list[i_10].tspeed);
												
												PlaySFX(&sfx_10reset);
												
												if (rand() % 2) boss10_list[i_10].hsp = -boss10_list[i_10].hsp;
												if (rand() % 2) boss10_list[i_10].vsp = -boss10_list[i_10].vsp;
												
												i_10++;
												if(i_10>boss10_count-1) i_10=0;
											}
										}
										if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE && boss9_started) WinCheck();
										
										// movement
										if(paddata.BTN_LEFT && !oldpad.BTN_LEFT) MoveCursor(-1, 0);
										if(paddata.BTN_RIGHT && !oldpad.BTN_RIGHT) MoveCursor(1, 0);
										if(paddata.BTN_UP && !oldpad.BTN_UP) MoveCursor(0, -1);
										if(paddata.BTN_DOWN && !oldpad.BTN_DOWN) MoveCursor(0, 1);
										
										// zoom check
										if(paddata.BTN_R1 && !oldpad.BTN_R1) zoomCheck(1);
										if(paddata.BTN_L1 && !oldpad.BTN_L1) zoomCheck(0);
										
										if(paddata.BTN_L2 && !oldpad.BTN_L2) if(the_hat>-1) the_hat--;
										if(paddata.BTN_R2 && !oldpad.BTN_R2) if(the_hat<8) the_hat++;
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
								show_shit = false;
								time_total = 0.0f;
								
								for(int _t = 0; _t < max_lvl; _t++)
								{
									time_total += level_time[_t];
								}
								
								rank_result = calculate_rank(time_total);
								printf("RANK: %d\n", rank_result);
								
								global_minecoins += current_minecoins;
								
								LEVEL=0;
								StartSave();
								start_end = 0;
								PlayBGM(193, 0);
								ready_to_click = 1;
								end_time = 0;
							}
							
							static int temp_t = 0;
							temp_t++;
							
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
									ready_to_click = 0;
									splash = splashes_random[rand() % SplashesCount()];
									start_music = 1;
									FadeInOut(&currentScreen, TITLE);
									PlaySFX(&sfx_gamestart);
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
							if(ship_x<-40.0f+(32*15)) ship_x+=1.75f;
							else
							{
								if(ship_x <= -38.0f+(32*15))
								{
									start_shop = 1;
									FadeInOut(&currentScreen, SHOP);
									ship_x+=1.75f;
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
								if(paddata.BTN_LEFT && !oldpad.BTN_LEFT) specialthanks = false;
								if(paddata.BTN_RIGHT && !oldpad.BTN_RIGHT) specialthanks = true;
								
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
							
							break;
						}
						default: break;
					}
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
		
		if(timer_running && !paused) timer_seconds += 0.018f;
		
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
							float t = 60.0f * 0.018f;
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
						if(boss10_list[i].time<=0)
						{
							if(free_play) fp_lifes=0; else LIFES=0;
							game_lost();
						}
						else boss10_list[i].time--;
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
		
		if(disable_control && !gamelose && !gamewin)
		{
			if(explode_time>0) explode_time--;
			else
			{
				can_move_xd = 0;
				disable_control = 0;
				
				if(free_play && fp_music<9) fp_lifes--;
				else
				{
					mistakes++;
					if(LEVEL<8) LIFES--;
				}
				
				CalculateNumbers();
				SpawnProjectile(offset_x+(cursorX*RENDER_SIZE), offset_y+(cursorY*RENDER_SIZE), 0, 0, 0, 1.0f);
				if((fp_lifes < 1 && free_play) || (!free_play && LIFES < 1))
				{
					disable_control = 1;
					gamelose = 1;
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
						
						DebugLog("LOGOSHOW");
						
						PS3Video_OpenFile(&the_video, "/dev_hdd0/game/MINESWEEP/USRDIR/data/test.264");
						PS3Video_Start(&the_video);
						PlayBGM(198, 1);
						game_loaded = 1;
					}
					oldpad = paddata;
				}
			}
		}
		
		if(playing_bgm) UpdateBGM();
		if(bad_apple) badAppleUpdate();
		
		drawScene();
		tiny3d_Flip();
		usleep(3666);
    }

    return 0;
}