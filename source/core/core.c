#include "core.h"
#include "level_data.h"
#include "episode2_shapes.h"
#include "badapple.h"

RevealNode reveal_queue[MAX_W * MAX_H];
Cell board[MAX_W][MAX_H];

char bad_apple_path[256];

int GRID_W = 16;
int GRID_H = 16;
int MINES  = 40;
int GRID_ID = 00;
int mines_count = 0;

float tnt_alpha = 0.0f;

int the_hat = 0;

int enable_half = 0;
int enable_double = 0;
int enable_anti = 0;

int fp_width = 9;
int fp_height = 9;
int fp_mines = 10;
int fp_doubles = 0;
int fp_halfes = 0;
int fp_antis = 0;
int fp_music = 1;
int fp_back = 0;
int fp_menu = 0;
int fp_lifes = 3;

int boss_menu = 0;

int cursorX = 0;
int cursorY = 0;
int gamewin = 0;
int gamelose = 0;
int in_game = 0;

int adventure_select = 0;
int challenge_select = 0;
int loadout_select = 0;

int settings_select = 0;

int revealed_total = 0;
int valid_total = 0;

int boss9_started = 0;
int boss10_started = 0;
int boss11_started = 0;
int boss12_started = 0;
int bossnan_started = 0;

int boss9_cutscene = 0;
int boss10_cutscene = 0;
int boss11_cutscene = 0;
int boss12_cutscene = 0;
int bossnan_cutscene = 0;

int boss9_count = 1;
int boss10_count = 1;
int boss11_count = 1;
int boss12_count = 1;
int bossnan_count = 1;

bool enable_9 = false;
bool enable_10 = false;
bool enable_11 = false;
bool enable_12 = false;
bool enable_nan = false;

bool CanMoveBetween(int x, int y, int nx, int ny)
{
    if(nx == x + 1 && ny == y) return !board[x][y].wall_right;
    if(nx == x - 1 && ny == y) return !board[x][y].wall_left;
    if(nx == x && ny == y + 1) return !board[x][y].wall_down;
    if(nx == x && ny == y - 1) return !board[x][y].wall_up;

    return true;
}

bool CanChordBetween(int x, int y, int nx, int ny)
{
    int dx = nx - x;
    int dy = ny - y;

    if(dx == 1 && dy == 0) return !board[x][y].wall_right;
    if(dx == -1 && dy == 0) return !board[x][y].wall_left;
    if(dx == 0 && dy == 1) return !board[x][y].wall_down;
    if(dx == 0 && dy == -1) return !board[x][y].wall_up;

    if(dx == 1 && dy == 1) return !board[x][y].wall_right && !board[x][y].wall_down;
    if(dx == -1 && dy == 1)  return !board[x][y].wall_left && !board[x][y].wall_down;
    if(dx == 1 && dy == -1) return !board[x][y].wall_right && !board[x][y].wall_up;
    if(dx == -1 && dy == -1) return !board[x][y].wall_left && !board[x][y].wall_up;

    return true;
}

void LoadBoardWalls(int grd_id)
{
    int x, y;
	const ShapeDef2 *map = &episode3_shapes[grd_id];

	for(y = 0; y < GRID_H; y++)
    {
		for(x = 0; x < GRID_W; x++)
        {
			u8 w = map->data[y * map->width + x];

            board[x][y].wall_up    = (w & 0x01) != 0;
            board[x][y].wall_right = (w & 0x02) != 0;
            board[x][y].wall_down  = (w & 0x04) != 0;
            board[x][y].wall_left  = (w & 0x08) != 0;
        }
    }
}

int FindEpisode2ShapeIndex(int the_id)
{
    int i;
    for (i = 0; i < sizeof(episode2_shapes) / sizeof(episode2_shapes[0]); i++)
    {
        if(episode2_shapes[i].the_id == the_id) return i;
    }
    return -1;
}

int FindEpisode3ShapeIndex(int the_id)
{
    int i;
    for (i = 0; i < sizeof(episode3_shapes) / sizeof(episode3_shapes[0]); i++)
    {
        if(episode3_shapes[i].the_id == the_id) return i;
    }
    return -1;
}

void DeactivateBosses()
{
	if(boss9_started)
	{		
		boss9_started = 0;
		if(gamewin && desired_bgm == 9) PlayBGM(109,0);
		Deactivate9();
	}
	if(boss10_started)
	{
		boss10_started = 0;
		if(gamewin && desired_bgm == 10) PlayBGM(110,0);
		Deactivate10();
	}
	if(boss11_started)
	{
		boss11_started = 0;
		if(gamewin && desired_bgm == 11) PlayBGM(111,0);
		Deactivate11();
	}
	if(boss12_started)
	{
		boss12_started = 0;
		if(gamewin && desired_bgm == 12) PlayBGM(112,0);
		Deactivate12();
	}
	if(bossnan_started)
	{
		bossnan_started = 0;
		//if(gamewin) PlayBGM(112,0);
		DeactivateNaN();
	}
}

void game_lost()
{
	explode_time = 60;
	disable_control = 1;
	PlaySFX(&sfx_mine);
}

void GenerateBoard()
{
	int xx, yy;
	int active_count = 0;
    memset(board, 0, sizeof(board));
	
	valid_total = (GRID_W * GRID_H) - MINES;
	
	for (xx = 0; xx < GRID_W; xx++)
    {
        for (yy = 0; yy < GRID_H; yy++)
        {
            board[xx][yy].active = 1;
        }
    }
	
	if(EPISODE>=2 || free_play)
	{
		if(GRID_ID!=999)
		{
			int grd_id = FindEpisode2ShapeIndex(GRID_ID);
			for (xx = 0; xx < GRID_W; xx++)
			{
				for (yy = 0; yy < GRID_H; yy++)
				{
					if(EPISODE>=2) board[xx][yy].active = (episode2_shapes[grd_id].shape[yy][xx] == '0');
						else board[xx][yy].active = 1;
					
					if(board[xx][yy].active) active_count++;
				}
			}
			if(MINES >= active_count) MINES = active_count - 1;
			valid_total = active_count - MINES;
		}
	}
	
	// if(EPISODE>=3 && !free_play)
	// {
		// if(GRID_ID!=999)
		// {
			// int grd_id = FindEpisode3ShapeIndex(GRID_ID);
			// LoadBoardWalls(grd_id);
		// }
	// }
	
    int placed = 0;
    int f_half = fp_halfes;
    int f_doub = fp_doubles;
    int f_anti = fp_antis;
	
    while(placed < MINES)
    {
        int x = rand() % GRID_W;
        int y = rand() % GRID_H;
		if(!board[x][y].active) continue;
		if(board[x][y].mine) continue;

		board[x][y].mine = 1;
		if(f_doub > 0)
		{
			board[x][y].doublemine = 1;
			enable_double = 1;
			f_doub--;
		}
		else if(f_half > 0)
		{
			board[x][y].halfmine = 1;
			enable_half = 1;
			f_half--;
		}
		else if(f_anti > 0)
		{
			board[x][y].antimine = 1;
			enable_anti = 1;
			f_anti--;
		}
		
		placed++;
    }
	
	RENDER_SIZE = RENDERING_SIZE * RENDERING_PIXELS;
	
	float sx = 800.0f / GRID_W;
	float sy = 460.0f / GRID_H;

	RENDER_SIZE = (sx < sy) ? sx : sy;
	if(RENDER_SIZE > 32.0f) RENDER_SIZE = 2.0f * RENDERING_PIXELS;
	
	offset_x = screen_w - (RENDER_SIZE * GRID_W / 2);
	offset_y = screen_h - (RENDER_SIZE * GRID_H / 2) + ui_offset_y;
}

void CalculateNumbers()
{
	int x, y;
    for(x=0;x<GRID_W;x++)
    {
        for(y=0;y<GRID_H;y++)
        {
            if(board[x][y].mine) continue;
			int ox, oy;
            int count = 0;
			bool hasMine = false;

            for(ox=-1;ox<=1;ox++)
            {
                for(oy=-1;oy<=1;oy++)
                {
					int nx, ny;
					
                    nx = x + ox;
                    ny = y + oy;

                    if(nx < 0 || ny < 0 || nx >= GRID_W || ny >= GRID_H) continue;
					
					if(!board[nx][ny].active) continue;
					
					if(!board[nx][ny].revealed)
					{
						if(board[nx][ny].doublemine)
						{
							count += 4;
							hasMine = true;
						}
						else if(board[nx][ny].halfmine)
						{
							count++;
							hasMine = true;
						}
						else if(board[nx][ny].antimine)
						{
							count -= 2;
							hasMine = true;
						}
						else if(board[nx][ny].mine)
						{
							count += 2;
							hasMine = true;
						}
					}
                }
            }
            board[x][y].neighbors = count;
            board[x][y].hasNeighborMines = hasMine;
        }
    }
}

int calculate_score()
{
	//if(Main.practice) return 0;
	float anti_multiplier = 1.0f;
	float double_multiplier = 1.0f;
	float half_multiplier = 1.0f;
	int larger;
	int doubles = 0; int halfs = 0;
	int antis = 0;
	
	for (int l = 0; l < GRID_W; l++)
	{
		for (int j = 0; j < GRID_H; j++)
		{
			if(board[l][j].doublemine) doubles++;
			if(board[l][j].halfmine) halfs++;
			if(board[l][j].antimine) antis++;
		}
	}
	
	if(enable_double)
	{
		if(doubles != 0 && MINES != 0)
		{
			if(doubles >= MINES - doubles) larger = doubles; else larger = MINES - doubles;
			double_multiplier = 1.0 + (larger / MINES);
		}
		//else double_multiplier = 1.0 + (0.5 - abs(0.5 - (float)(doubles / 100)));
	}
	
	if(enable_half)
	{
		if(halfs != 0 && MINES != 0)
		{
			if(halfs >= MINES - halfs) larger = halfs; else larger = MINES - halfs;
			half_multiplier = 1 + (larger / MINES);
		}
		//else half_multiplier = 1.0 + (0.5 - abs(0.5 - (float)(halfs / 100)));
	}
	
	if(enable_anti)
	{
		if(antis != 0 && MINES != 0)
		{
			if(antis >= MINES - antis) larger = antis; else larger = MINES - antis;
			anti_multiplier = 1 + (larger / MINES);
		}
		//else anti_multiplier = 1.0 + (0.5 - abs(0.5 - (float)(antis / 100)));
	}
	
	int score = 0;
	int i, j;
	
	for(i = 0; i < GRID_W; i++)
	{
		for(j = 0; j < GRID_H; j++)
		{
			if(!board[i][j].mine && board[i][j].neighbors < 16)
			{
				score += pow(2, (board[i][j].neighbors/2) - 1);
			}
			if(board[i][j].antimine) score++;  // < ok now we are ready >:)
			if(board[i][j].doublemine) score++;
			if(board[i][j].halfmine) score++;
		}
	}
	if(LEVEL==8) score += 999;
	if(LEVEL==9) score += 1000;
	if(LEVEL==10) score += 1111;
	if(LEVEL==11) score += 1212;
	if(LEVEL==12) score += 1500;
	if(boss9_count > 0) score += 9;
	if(boss10_count > 0) score += 10;
	if(boss11_count > 0) score += 11;
	if(boss12_count > 0) score += 12;
	if(bossnan_count > 0) score += 15;
	score += MINES;
	//score *= 1.5; // cuz we are practically ALWAYS in master LMAO
	//if(Main.db["master"]) score \*= 1.5
	score *= double_multiplier;
	score *= half_multiplier;
	score *= anti_multiplier;
	return score;
}

void WinCheck()
{
	revealed_total = 0;
	valid_total = 0;
	int wx, wy;

	for (wx = 0; wx < GRID_W; wx++)
	{
		for (wy = 0; wy < GRID_H; wy++)
		{
			if(board[wx][wy].active && !board[wx][wy].mine)
			{
				valid_total++;
				if(board[wx][wy].revealed) revealed_total++;
			}
		}
	}
	
	// if(enable_9) revealed_total+=9;
	
	if(revealed_total >= valid_total)
	{
		DebugLog("game won!");
		gamewin = 1;
		mines_count = 0;
		timer_running = 0;
		disable_control = 1;
		playing_bgm = false;
		StopAudio();
		
		DeactivateBosses();
		if(!enable_9 && !enable_10 && !enable_11 && !enable_12 && !enable_nan) PlayBGM(100+desired_bgm, 0);
		if(enable_9 && enable_10 && enable_11 && enable_12) PlayBGM(196, 0);
		
		int fx, fy;
		
		for (fx = 0; fx < GRID_W; fx++)
		{
			for (fy = 0; fy < GRID_H; fy++)
			{
				if(board[fx][fy].mine)
				{
					board[fx][fy].flagged = 2;
					if(board[fx][fy].doublemine) board[fx][fy].flagged++;
					if(board[fx][fy].halfmine) board[fx][fy].flagged--;
					if(board[fx][fy].antimine) board[fx][fy].flagged+=2;
				}
			}
		}
		
		level_minecoins = calculate_score();
		current_minecoins += level_minecoins;
		
		if(!free_play)
		{
			if(LEVEL==66)
			{
				level_time[9] += timer_seconds;
			}
			else level_time[LEVEL] = timer_seconds;
		}
		else global_minecoins += current_minecoins;
	}
	else if(enable_9)
	{
		if(free_play) fp_lifes=0; else LIFES=0;
		game_lost();
	}
}

void RevealIterative(int startx, int starty)
{
    int head = 0;
    int tail = 0;

    if(startx < 0 || starty < 0 || startx >= GRID_W || starty >= GRID_H) return;
    if(!board[startx][starty].active) return;
    board[startx][starty].processed = 1;

    reveal_queue[tail].x = startx;
    reveal_queue[tail].y = starty;
    tail++;

    while(head < tail)
    {
        int x = reveal_queue[head].x;
        int y = reveal_queue[head].y;
        head++;

        board[x][y].processed = 0;

        if(board[x][y].revealed || board[x][y].flagged>0) continue;

        board[x][y].revealed = 1;

        if(board[x][y].mine)
        {
            game_lost();
			mines_count--;
            return;
        }

        if(board[x][y].hasNeighborMines) continue;
		
		int ox, oy;

        for(ox = -1; ox <= 1; ox++)
        {
            for(oy = -1; oy <= 1; oy++)
            {
                if(ox == 0 && oy == 0) continue;

                int nx = x + ox;
                int ny = y + oy;

                if(nx < 0 || ny < 0 || nx >= GRID_W || ny >= GRID_H) continue;
                if(!board[nx][ny].active) continue;
                if(board[nx][ny].revealed) continue;
                if(board[nx][ny].flagged>0) continue;
				if(board[nx][ny].ice_block) continue;
                if(board[nx][ny].processed) continue;

                board[nx][ny].processed = 1;

                reveal_queue[tail].x = nx;
                reveal_queue[tail].y = ny;
                tail++;
            }
        }
    }
}

void RevealRecursive(int x,int y)
{
    if(x < 0 || y < 0 || x >= GRID_W || y >= GRID_H) return;
	if(!board[x][y].active) return;
	if(board[x][y].ice_block) return;
    if(board[x][y].revealed || board[x][y].flagged>0) return;

    board[x][y].revealed = 1;
	
    if(board[x][y].mine)
	{
		game_lost();
		mines_count--;
		return;
	}

    if(board[x][y].hasNeighborMines) return;
	
	int ox, oy;
    for(ox=-1;ox<=1;ox++)
    {
        for(oy=-1;oy<=1;oy++)
        {
            if(ox==0 && oy==0) continue;
            RevealRecursive(x+ox,y+oy);
        }
    }
}

void Reveal(int x,int y){if(reveal_mode) RevealRecursive(x, y); else RevealIterative(x, y);}

void Chord(int x, int y)
{
    if(x < 0 || y < 0 || x >= GRID_W || y >= GRID_H) return;
    if(!board[x][y].revealed) return;
    if(board[x][y].mine) return;
    if(board[x][y].ice_block) return;
	if(board[x][y].neighbors == 0 && !board[x][y].hasNeighborMines) return;
	
    int required = board[x][y].neighbors;
    int flags = 0;

    int nx, ny;

    for(nx = x - 1; nx <= x + 1; nx++)
    {
        for(ny = y - 1; ny <= y + 1; ny++)
        {
            if(nx >= 0 && ny >= 0 && nx < GRID_W && ny < GRID_H)
            {
                flags += FlagValue(board[nx][ny].flagged);
            }
        }
    }
	
    if(flags == required)
    {
        for(nx = x - 1; nx <= x + 1; nx++)
        {
            for(ny = y - 1; ny <= y + 1; ny++)
            {
                if(nx >= 0 && ny >= 0 && nx < GRID_W && ny < GRID_H)
                {
					if(!board[nx][ny].active) continue;
					if(board[nx][ny].ice_block) continue;
                    if(board[nx][ny].flagged==0)
                    {
                        if(board[nx][ny].mine && !board[nx][ny].revealed)
                        {
							board[nx][ny].revealed = 1;
                            game_lost();
							mines_count--;
                            return;
                        }

                        Reveal(nx, ny);
                    }
                }
            }
        }
    }
}

int FlagValue(int flag)
{
    switch(flag)
    {
        case 1: return 1;   // Half
        case 2: return 2;   // Normal
        case 3: return 4;   // Double
        case 4: return -2;  // Anti
		default: return 0;  // Not found!
    }
}

void badAppleUpdate()
{
	int x, y;
	static float videoFrame = 0.0f;
	static int lastFrame = -1;

	videoFrame += 0.5f;

	float audioFrame = (GetTimeAudio() * video_header.fps) / 1000.0f;
	float error = audioFrame - videoFrame;
	
	if(videoFrame > 6562.0f)
	{
		StopAudio();
		GoBackToTitle();
	}

	if(fabsf(error) > 10.0f) videoFrame = audioFrame; else videoFrame += error * 0.01f;
	
	int frame = (int)videoFrame;
	
	if(frame != lastFrame)
	{
		lastFrame = frame;
		Video_ReadFrame(frame);
		
		for(y = 0; y < GRID_H; y++)
		{
			for(x = 0; x < GRID_W; x++)
			{
				board[x][y].mine = frame_buffer[y * video_header.width + x];
				board[x][y].revealed = !frame_buffer[y * video_header.width + x];
			}
		}
		
		CalculateNumbers();
	}
}

bool bad_apple_init()
{
	if(Video_Open("/dev_hdd0/game/MINESWEEP/USRDIR/dlc00/data.dat") && FindMusic("/dev_hdd0/game/MINESWEEP/USRDIR/dlc00/mus", bad_apple_path))
	{
		fp_width = 64;
		fp_height = 48;
		fp_mines = 1;
		temp_fp_music = fp_music;
		fp_music = 100;
		bgm[100] = bad_apple_path;
		LEVEL = 0;
		in_game = 1;
		bad_apple = 1;
		return true;
	}
	else
	{
		msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
			"Bad Apple!! DLC is not installed. Please, install it from AcidPS3 Store v0.2 or higher!",
			EmptyDialogCallback, NULL, NULL);
		return false;
	}
}

void LoadLevel(int levelz)
{
	fp_doubles = 0;
	fp_halfes = 0;
	fp_antis = 0;
	
	DifficultyData *lvl = &episodes[EPISODE-1].levels[levelz].difficulties[DIFFICULTY];
	
    GRID_W = lvl->width;
    GRID_H = lvl->height;
    MINES  = lvl->mines;
	GRID_ID= lvl->id;
    desired_bgm = lvl->music;
	the_bg = lvl->back;
	
	if(levelz==8) MINES-=8;
	
	if(EPISODE>=3) fp_doubles = MINES / 2;
	
	if(EPISODE>=6)
	{
		fp_doubles = MINES / 3;
		fp_halfes = MINES / 3;
	}
	
	ResetGrid();
}

void LoadFPLevel()
{
	fp_lifes = 3;
	StartSave();
	
    GRID_W = fp_width;
    GRID_H = fp_height;
    MINES  = fp_mines;
    desired_bgm = fp_music;
	
	if(desired_bgm==12) desired_bgm = 17;
	if(desired_bgm==13) desired_bgm = 18;
	
	if(enable_9) MINES-=8;
	
	the_bg = fp_back;
	
	ResetGrid();
}

void MoveCursor(int dx, int dy)
{
    int nx = cursorX;
    int ny = cursorY;

    do
    {
        nx += dx; ny += dy;

        if(nx < 0 || ny < 0 || nx >= GRID_W || ny >= GRID_H) return;
    }
    while (!board[nx][ny].active);

    cursorX = nx;
    cursorY = ny;
}

int calculate_rank(float time)
{
	int rank = 7;
	
	switch(EPISODE)
	{
		case 1:
		{
			if(DIFFICULTY == 0)
			{
				if(time > 180.0f) rank -= 1;
				if(time > 360.0f) rank -= 1;
				if(time > 540.0f) rank -= 1;
				if(current_minecoins < 1000) rank -= 1;
				if(current_minecoins < 1750) rank -= 1;
				if(mistakes >= 1) rank -= 1;
				if(mistakes >= 3) rank -= 1;
				if(mistakes >= 5) rank -= 1;
			}
			else if(DIFFICULTY == 1)
			{
				if(time > 300.0f) rank -= 1;
				if(time > 600.0f) rank -= 1;
				if(time > 900.0f) rank -= 1;
				if(current_minecoins < 1500) rank -= 1;
				if(current_minecoins < 2500) rank -= 1;
				if(mistakes >= 1) rank -= 1;
				if(mistakes >= 4) rank -= 1;
				if(mistakes >= 7) rank -= 1;
			}
			else if(DIFFICULTY == 2)
			{
				if(time > 540.0f) rank -= 1;
				if(time > 1080.0f) rank -= 1;
				if(time > 1620.0f) rank -= 1;
				if(current_minecoins < 2000) rank -= 1;
				if(current_minecoins < 3000) rank -= 1;
				if(mistakes >= 2) rank -= 1;
				if(mistakes >= 5) rank -= 1;
				if(mistakes >= 8) rank -= 1;
			}
			else
			{
				if(time > 840.0f) rank -= 1;
				if(time > 1680.0f) rank -= 1;
				if(time > 2520.0f) rank -= 1;
				if(current_minecoins < 3000) rank -= 1;
				if(current_minecoins < 4000) rank -= 1;
				if(mistakes >= 3) rank -= 1;
				if(mistakes >= 6) rank -= 1;
				if(mistakes >= 9) rank -= 1;
			}
			break;
		}
		case 2:
		{
			if(DIFFICULTY == 0)
			{
				if(time > 360.0f) rank -= 1;
				if(time > 720.0f) rank -= 1;
				if(time > 1080.0f) rank -= 1;
				if(current_minecoins < 4000) rank -= 1;
				if(current_minecoins < 2500) rank -= 1;
				if(mistakes >= 1) rank -= 1;
				if(mistakes >= 4) rank -= 1;
				if(mistakes >= 7) rank -= 1;
			}
			if(DIFFICULTY == 1)
			{
				if(time > 660.0f) rank -= 1;
				if(time > 1320.0f) rank -= 1;
				if(time > 1980.0f) rank -= 1;
				if(current_minecoins < 5500) rank -= 1;
				if(current_minecoins < 3500) rank -= 1;
				if(mistakes >= 3) rank -= 1;
				if(mistakes >= 6) rank -= 1;
				if(mistakes >= 9) rank -= 1;
			}
			else if(DIFFICULTY == 2)
			{
				if(time > 1080.0f) rank -= 1;
				if(time > 2160.0f) rank -= 1;
				if(time > 3240.0f) rank -= 1;
				if(current_minecoins < 6500) rank -= 1;
				if(current_minecoins < 4500) rank -= 1;
				if(mistakes >= 4) rank -= 1;
				if(mistakes >= 9) rank -= 1;
				if(mistakes >= 14) rank -= 1;
			}
			else
			{
				if(time > 1620.0f) rank -= 1;
				if(time > 3240.0f) rank -= 1;
				if(time > 4860.0f) rank -= 1;
				if(current_minecoins < 7500) rank -= 1;
				if(current_minecoins < 5000) rank -= 1;
				if(mistakes >= 5) rank -= 1;
				if(mistakes >= 12) rank -= 1;
				if(mistakes >= 19) rank -= 1;
			}
			break;
		}
		case 3:
		{
			if(DIFFICULTY == 0)
			{
				if(time > 660.0f) rank -= 1;
				if(time > 1320.0f) rank -= 1;
				if(time > 1980.0f) rank -= 1;
				if(current_minecoins < 6000) rank -= 1;
				if(current_minecoins < 4500) rank -= 1;
				if(mistakes >= 2) rank -= 1;
				if(mistakes >= 5) rank -= 1;
				if(mistakes >= 8) rank -= 1;
			}
			else if(DIFFICULTY == 1)
			{
				if(time > 960.0f) rank -= 1;
				if(time > 1920.0f) rank -= 1;
				if(time > 2880.0f) rank -= 1;
				if(current_minecoins < 7000) rank -= 1;
				if(current_minecoins < 5500) rank -= 1;
				if(mistakes >= 4) rank -= 1;
				if(mistakes >= 7) rank -= 1;
				if(mistakes >= 10) rank -= 1;
			}
			else if(DIFFICULTY == 2)
			{
				if(time > 1200.0f) rank -= 1;
				if(time > 2400.0f) rank -= 1;
				if(time > 3600.0f) rank -= 1;
				if(current_minecoins < 8000) rank -= 1;
				if(current_minecoins < 6000) rank -= 1;
				if(mistakes >= 5) rank -= 1;
				if(mistakes >= 9) rank -= 1;
				if(mistakes >= 13) rank -= 1;
			}
			else
			{
				if(time > 1500.0f) rank -= 1;
				if(time > 3000.0f) rank -= 1;
				if(time > 4500.0f) rank -= 1;
				if(current_minecoins < 9000) rank -= 1;
				if(current_minecoins < 7000) rank -= 1;
				if(mistakes >= 6) rank -= 1;
				if(mistakes >= 11) rank -= 1;
				if(mistakes >= 16) rank -= 1;
			}
			break;
		}
	}
	
    if(rank < 0) rank = 0;
	
	return rank;
}