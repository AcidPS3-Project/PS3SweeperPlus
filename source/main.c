#include "bosses.h"
#include "level_data.h"
#include "save.h"
#include "drawing.h"

void ResetGrid()
{
	currentScreen = GAME;
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
	
	if(LEVEL==8 || (free_play && desired_bgm == 9))
	{
		SetupBoss9Positions(boss9_count);
		if(free_play)
		{
			desired_bgm = 10;
			boss9_cutscene = 1;
		}
		// msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON, "There is a boss battle in this game. The battle works the same as normal Minesweeper, but with some key distinctions:\n"
			// "you won't know the number of mines hidden on the grid, you have limited time to complete the level, and you won't be able to use the sonar or pause the game.\n"
			// "Clicking on a mine will make the time run out faster. To win, you must click SQUARE ONLY AFTER revealing all the tiles that don't contain mines.\n"
			// "If the timer hits 0, or you click on the boss before revealing the entire grid, it's game over.", EmptyDialogCallback, NULL, NULL);
	}
	else if(LEVEL==9 || (free_play && desired_bgm == 10))
	{
		SetupBoss10Positions(boss10_count);
		if(free_play)
		{
			desired_bgm = 12;
			boss10_cutscene = 1;
		}
		// msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
			// "There is another boss battle in this game. As soon as the game starts, this boss will start counting down from 10.\n"
			// "If it counts all the way down to 0, it's game over. Click SQUARE to reset the countdown back to 10, but be careful.\n"
			// "The boss will move faster and faster every time you click. Clicking on a mine will cause the boss to count down more quickly.\n"
			// "To win, reveal the entire grid before the boss is able to finish its countdown.", EmptyDialogCallback, NULL, NULL);
	}
	else if(LEVEL==10 || (free_play && desired_bgm == 11))
	{
		SetupBoss11Positions(boss11_count);
		if(free_play)
		{
			desired_bgm = 15;
			boss11_cutscene = 1;
		}
		// msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
			// "ELEVEN TEMP DESCRIPTION", EmptyDialogCallback, NULL, NULL);
	}
	else if(LEVEL==11)
	{
		SetupBoss12Positions(boss12_count);
		if(free_play)
		{
			//desired_bgm = 15;
			boss12_cutscene = 1;
		}
		// msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
			// "ELEVEN TEMP DESCRIPTION", EmptyDialogCallback, NULL, NULL);
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
	PlayBGM(desired_bgm, 1);
}

void GoBackToTitle()
{
	in_game = 0;

	gamewin = 0;
	gamelose = 0;
	current_minecoins = 0;
	disable_control = 0;

	splash = splashes_random[rand() % SplashesCount()];
	free_play = 0;
	
	bad_apple = 0;
	
	enable_half = 0;
	enable_double = 0;
	enable_anti = 0;
	
	if(temp_fp_music==100) fp_music = temp_fp_music;
	
	i_10 = 0;
	
	currentScreen = MENU;
	menu_pos = 0;
	menu_x = 0.0f;
	ze_x = 0.0f;
	adventure_select = 0;

	PlayBGM(50+fp_music, 1);
	
	StartSave();
}

char custom_main_path[256];
char custom_win_path[256];

int main(int argc, const char* argv[])
{
	padInfo padinfo;
	int dick, ip;
	static padData oldpad;
	padData paddata;
	
	srand(time(NULL));
	tiny3d_Init(1024*1024);
	
	sysModuleLoad(SYSMODULE_PNGDEC);
	savestate_init();
	Initialize();
	
	SplashesInit();
	for (dick = 0; dick < rand() % SplashesCount(); dick++) splash = splashes_random[rand() % SplashesCount()];
	
	boss9_started = 0;
	boss10_started = 0;
	boss11_started = 0;
	
	init_bgm();
	init_sfx();
	
	ioPadInit(7);
    LoadTexture();
	
	StartLoad();
	
    while(1)
    {
		sysUtilCheckCallback();
		UpdateFPS();
		
		if(start_music)
		{
			start_music = 0;
			playing_bgm = true;
			PlayBGM(0, 1);
		}
		
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
					switch(currentScreen)
					{
						case TITLE:
						{
							if(paddata.BTN_START && !oldpad.BTN_START)
							{
								StopAudio();
								PlayBGM(50+fp_music, 1);
								currentScreen = MENU;
							}
							
							if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE)
							{
								msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
									"CREDITS\n\n"
									"Original Minesweeper Plus game made by Jorel Simpson\n"
									"XMB Background made by Zeltizy\n"
									"Half Related Sprites by Axelbastian",
									EmptyDialogCallback, NULL, NULL);
							}
							
							break;
						}
						case MENU:
						{
							menu_x = lerp(menu_x, -262 * menu_pos, 0.1f);
							if(!free_play)
							{
								if(challenge_select)
								{
									if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
									{
										challenge_select = 0;
									}
								}
								else if(adventure_select)
								{
									if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
									{
										if(paddata.BTN_R3) global_route = 1;
										LEVEL = 0;
										if(paddata.BTN_L2)
										{
											EPISODE = 3;
											LEVEL = 1;
										}
										if(paddata.BTN_R2)
										{
											EPISODE = 3;
											LEVEL = 0;
										}
										current_minecoins = 0;
										StopAudio();
										LoadLevel(LEVEL);
										in_game = 1;
									}
									if(paddata.BTN_UP && !oldpad.BTN_UP)
									{
										DIFFICULTY--;
										if (DIFFICULTY<0) DIFFICULTY = 0;
									}
									if(paddata.BTN_DOWN && !oldpad.BTN_DOWN)
									{
										DIFFICULTY++;
										if (DIFFICULTY>3) DIFFICULTY = 3;
									}
									if(paddata.BTN_LEFT && !oldpad.BTN_LEFT)
									{
										EPISODE--;
										if (EPISODE<1) EPISODE = 1;
									}
									if(paddata.BTN_RIGHT && !oldpad.BTN_RIGHT)
									{
										EPISODE++;
										if (EPISODE>episode_count) EPISODE = episode_count;
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
											default: break;
										}
									}
									if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE && LEVEL>0)
									{
										free_play = 0;
										StopAudio();
										LoadLevel(LEVEL);
										in_game = 1;
									}
									
									//if(paddata.BTN_R2 && !oldpad.BTN_R2) HolyShit();
									
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
								}
							}
							else
							{
								if(paddata.BTN_START && !oldpad.BTN_START)
								{
									LEVEL = 0;
									LoadFPLevel();
									in_game = 1;
								}
								
								if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
								{
									splash = splashes_random[rand() % SplashesCount()];
									free_play = 0;
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
								
								if(paddata.BTN_LEFT && !oldpad.BTN_LEFT)
								{
									switch(fp_menu)
									{
										case 0: fp_width--; if(fp_width<3) fp_width=3; break;
										case 1: fp_height--; if(fp_height<3) fp_height=3; break;
										case 2: fp_mines--; if(fp_mines<1) fp_mines=1; break;
										case 3: fp_doubles--; if(fp_doubles<0) fp_doubles=0; break;
										case 4: fp_halfes--; if(fp_halfes<0) fp_halfes=0; break;
										//case 5: fp_antis--; if(fp_antis<0) fp_antis=0; break;
										case 6:
										{
											fp_music--;
											if(fp_music>95) fp_music=8;
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
										case 0: fp_width++; if(fp_width>48) fp_width=48; break;
										case 1: fp_height++; if(fp_height>36) fp_height=36; break;
										case 2: fp_mines++; break;
										case 3: fp_doubles++; if(fp_doubles>(fp_mines-fp_halfes-fp_antis)) fp_doubles=fp_mines-fp_halfes-fp_antis; break;
										case 4: fp_halfes++; if(fp_halfes>(fp_mines-fp_doubles-fp_antis)) fp_halfes=fp_mines-fp_doubles-fp_antis; break;
										//case 5: fp_antis++; if(fp_antis>(fp_mines-fp_doubles-fp_halfes)) fp_antis=fp_mines-fp_doubles-fp_halfes; break;
										case 6:
										{
											fp_music++;
											if(fp_music>12) fp_music=12;
											ReplaceBGM(fp_music);
											break;
										}
										case 7: fp_back++; if(fp_back>14) fp_back=14; break;
										default: break;
									}
									
									if(fp_mines>(fp_width*fp_height)-1) fp_mines=(fp_width*fp_height)-1;
								}
								
								if(paddata.BTN_SELECT && !oldpad.BTN_SELECT)
								{
									fp_width = 72;
									fp_height = 48;
									fp_mines = 111;
									LEVEL = 0;
									LoadFPLevel();
									in_game = 1;
								}
								if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE)
								{
									if(bad_apple_init())
									{
										printf("Loaded Bad Apple!!\n");
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
												"Error!\nFile not found in:\n/dev_usb000/SWEEPING/bgm.mp3 (or bgm.ogg)!",
												EmptyDialogCallback, NULL, NULL);
											continue;
										}
										
										bgm[99] = (unsigned char *)custom_main_path;
										fp_music = 99;

										if(!has_custom_win)
										{
											msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
												"Warning!\nFile not found in:\n/dev_usb000/SWEEPING/win.mp3 (or win.ogg)!\nIt is not necessary, but winning music won't play.",
												EmptyDialogCallback, NULL, NULL);
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
						break;
					}
						case MAP:
						{
							break;
						}
						case GAME:
						{
							if(gamelose && paddata.BTN_START && !oldpad.BTN_START) GoBackToTitle();
							else if(gamewin)
							{
								if(paddata.BTN_SELECT && !oldpad.BTN_SELECT)
								{
									if(free_play) LoadFPLevel();
									else GoBackToTitle();
								}

								if(paddata.BTN_START && !oldpad.BTN_START)
								{
									if(!free_play)
									{
										int max_lvl = episodes[EPISODE-1].level_count;
										LEVEL++;

										if(LEVEL >= max_lvl)
										{
											EPISODE++;
											LEVEL=0;
											GoBackToTitle();
										}
										else
										{
											StartSave();
											LoadLevel(LEVEL);
										}
									}
									else GoBackToTitle();
								}
							}
							
							if (!disable_control && in_game && !bad_apple)
							{					
								// basic
								if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
								{
									if(LEVEL==8 && !boss9_cutscene)
									{
										if(cursorX==GRID_W/2 && cursorY==GRID_H/2)
										{
											boss9_cutscene = 1;
											the_bg = 4;
											PlayBGM(10, 1);
										}
									}
									else if(LEVEL==9 && !boss10_cutscene)
									{
										if(cursorX==GRID_W/2 && cursorY==GRID_H/2)
										{
											boss10_cutscene = 1;
											the_bg = 9;
											PlayBGM(12, 1);
										}
									}
									else if(LEVEL==10 && !boss11_cutscene)
									{
										if(cursorX==GRID_W/2 && cursorY==GRID_H/2)
										{
											boss11_cutscene = 1;
											the_bg = 14;
											PlayBGM(15, 1);
										}
									}
									else if(LEVEL==11 && !boss12_cutscene)
									{
										if(cursorX==GRID_W/2 && cursorY==GRID_H/2)
										{
											boss12_cutscene = 1;
											the_bg = 14;
											PlayBGM(15, 1);
										}
									}
									else
									{
										if(FIRST_CLICK) timer_running = 1;
										
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
										
										if((LEVEL==8 && !boss9_started) || (free_play && boss9_cutscene && !boss9_started))
										{
											boss9_started = 1;
											
											int i;
											for (i = 0; i < boss9_count; i++)
											{
												boss9_list[i].active = true;
											}
											
											StopAudio();
											PlayBGM(11, 1);
											if(free_play) desired_bgm = 9;
										}
										else if((LEVEL==9 && !boss10_started) || (free_play && boss10_cutscene && !boss10_started))
										{
											boss10_started = 1;
											
											int i;
											for (i = 0; i < boss10_count; i++)
											{
												boss10_list[i].active = true;
											}
											
											StopAudio();
											PlayBGM(13, 1);
											if(free_play) desired_bgm = 10;
										}
										else if((LEVEL==10 && !boss11_started) || (free_play && boss11_cutscene && !boss11_started))
										{
											boss11_started = 1;
											
											int i;
											for (i = 0; i < boss11_count; i++)
											{
												boss11_list[i].active = true;
											}
											
											StopAudio();
											PlayBGM(16, 1);
											if(free_play) desired_bgm = 11;
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
										boss10_list[i_10].tspeed+=0.031f;
										float min_speed = boss10_list[i_10].tspeed;
										
										boss10_list[i_10].hsp = min_speed + (((float)rand() / (float)RAND_MAX) * boss10_list[i_10].tspeed);
										boss10_list[i_10].vsp = min_speed + (((float)rand() / (float)RAND_MAX) * boss10_list[i_10].tspeed);
										
										PlaySFX(&sfx_10reset);
										
										if (rand() % 2) boss10_list[i_10].hsp = -boss10_list[i_10].hsp;
										if (rand() % 2) boss10_list[i_10].vsp = -boss10_list[i_10].vsp;
										
										i_10++;
										if(i_10>boss10_count) i_10=0;
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
							}
							break;
						}
						case ENDING:
						{
							break;
						}
						default: break;
					}
					oldpad = paddata;
				}
			}
		}
		
		

		if(boss9_started && !disable_control)
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
		
		if(timer_running) timer_seconds += 0.018f;
		
		if(boss10_started && !disable_control)
		{
			int i;

			for (i = 0; i < boss10_count; i++)
			{
				boss10_list[i].timer += boss10_list[i].speed;
				if(boss10_list[i].timer > 1.0f)
				{
					boss10_list[i].timer = 0.0f;
					boss10_list[i].time--;
					if(boss10_list[i].time<=0) game_lost();
				}
				
				boss10_update(i);
			}
		}
		
		if(boss11_started && !disable_control)
		{
			int i;
			for (i = 0; i < boss11_count; i++)
			{
				boss11_update(i);
			}
		}
		
		if(disable_control && !gamelose && !gamewin)
		{
			if(explode_time>0) explode_time--;
			else
			{
				DeactivateBosses();
				gamelose = 1;
				timer_running = 0;
				playing_bgm = false;
				StopAudio();
				PlaySFX(&sfx_explosion);
			}
		}
		
		
		if(game_init)
		{
			game_init = 0;
			
			LEVEL = gSave.save_level;
			current_minecoins = gSave.save_current_minecoins;
			global_minecoins = gSave.save_global_minecoins;
			EPISODE = gSave.save_episode;
			fp_width = gSave.save_fp_w;
			fp_height = gSave.save_fp_h;
			fp_mines = gSave.save_fp_m;
			fp_music = gSave.save_fp_mus;
			fp_back = gSave.save_fp_bg;
			printf("loaded!\n");
			
			if(EPISODE<1) EPISODE=1;
			if(fp_width<1) fp_width=9;
			if(fp_height<1) fp_height=9;
			if(fp_mines<1) fp_mines=10;
			if(fp_music<1) fp_music=1;
		}
		else if(!game_loaded)
		{
			ioPadGetInfo(&padinfo);
			for(ip = 0; ip < MAX_PADS; ip++)
			{
				if(padinfo.status[ip])
				{
					ioPadGetData(ip, &paddata);
					if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
					{
						game_loaded = 1;
						start_music = 1;
						printf("starting the game\n");
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