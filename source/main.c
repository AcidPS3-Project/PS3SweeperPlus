//#include <ppu-lv2.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <io/pad.h>
#include <pngdec/pngdec.h>
#include <sysmodule/sysmodule.h>
#include <sysutil/save.h>
#include <sysutil/msg.h>
#include <sysutil/osk.h>
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
#include "spu_soundmodule.bin.h"

// IMPORT IMAGES
#include "captain_sheet_png_bin.h"
#include "tiles_sheet_png_bin.h"
#include "ui_sheet_png_bin.h"
#include "boss_sheet_png_bin.h"
#include "boss1_sheet_png_bin.h"
#include "icon0_png_bin.h"

//IMPORT MUSIC
#include "title_mp3_bin.h"
#include "bgm1_mp3_bin.h"
#include "bgm2_mp3_bin.h"
#include "bgm3_mp3_bin.h"
#include "bgm4_mp3_bin.h"
#include "win1_mp3_bin.h"
#include "win2_mp3_bin.h"
#include "win3_mp3_bin.h"
#include "win4_mp3_bin.h"
#include "bgm9_mp3_bin.h"

//IMPORT SFX
#include "flag_ogg_bin.h"
#include "unflag_mp3_bin.h"
#include "nine_move_ogg_bin.h"
#include "nine_stop_ogg_bin.h"

typedef struct
{
    int width;
    int height;
    int mines;
    int music;
    int back;
} LevelData;

LevelData levels[] =
{
    {6, 6, 3, 1, 0},
    {8, 8, 6, 1, 0},
    {10, 10, 10, 2, 1},
    {9, 12, 12, 2, 1},
    {18, 9, 20, 3, 2},
    {15, 15, 30, 3, 2},
    {18, 12, 33, 4, 3},
    {16, 16, 42, 4, 3},
    {13, 13, 29, 9, 0}
};

#define LEVEL_COUNT (sizeof(levels)/sizeof(levels[0]))
//#define LEVEL_COUNT 66

int current_level = 0;
int global_route = 0;
int the_bg = 0;

int ttf_inited = 0;
int game_loaded = 0;
int free_play = 0;

FT_Library freetype;
FT_Face face;







char global_name[] = "PS3Sweeper+";
char global_version[] = "v0.1";

const char *splashes_random[] =
{
	"Sweeping on PS3!",
	"This is NOT a port!",
	"DualShock supported",
	"Does not require PS Eye!",
	//"When Full Touhou Route?",
	//"When Custom Sweeper?",
	//"When NaN?",
	"April fools joke was peak",
	"Contains touhouslop",
	"The Bad Apple!!",
	"Why so serious?",
	"Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven\nSix Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven\nSix Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven Six Seven",
	"Join AcidPS3 Server",
	"514 is scrapped.",
	"Clavicord is real?",
	"Play original first!",
	"Also play yawn's minesweeper",
	"Also play Minesweeper Plus Plus",
	"Also play Minesweeper Minus",
	"Also play Second Destination (if it comes out)",
	"Also play MS+ ACE",
	"Also play Minesweeper Times, oh wait. its canceled :(",
	"Also play LandMinesweeper",
	"Also play MS+ Hard Mode",
	"E",
	"The MORSHU is Real!",
	"Also play Minesweeper Plus Roblox",
	"Why it was rewritten?",
	"C is better, than GMS1.4",
	"Couldn't find PSL1GHT, but found PSDK3v2!",
	"When tutorial how to make games with PSDK3v2?",
	"Yay, minecoins! I'm rich!!",
	"Better, than MS+ PS3 Edition",
	"BOO",
	"ChadGPT, please help me fix the saving system!",
	"AI-Assisted code, because I suck at C.",
	"Optimizing is at it's peakest.",
	"Baldi for PS3 re-write coming?",
	"AcidPS3 Store coming as an app?",
	"CELL Programming",
	"The Yellow light of Death Egg",
	"Reality Synthesizer can fail?",
	//"NEC-Tokens included!",
	"I really suck at PS3 games coding.",
	"Languages not coming since technical issues, sorry.",
	"Not compatible with RPCS3 and aPS3e!",
	"Watch no one play this",
	"The definition of Insanity.",
	"F U C K",
	"up up down down left right left right circle cross start.\noh wait, it doesnt work",
	"why the fuck i stayed all night for this shit",
	"Hold START + L3 + R3 for a surprise."
};

int splash_count;
const char *splash;






const unsigned char *bgm[12];
int bgm_size[12];

int current_bgm = 1;
int desired_bgm = 1;

int global_minecoins = 0;
int current_minecoins = 0;
int level_minecoins = 0;

typedef struct
{
    short *pcm;
    int pcm_size;
    int freq;
    int stereo;
} SoundEffect;

SoundEffect sfx_flag;
SoundEffect sfx_unflag;
SoundEffect sfx_9move;
SoundEffect sfx_9stop;

void init_bgm()
{
	bgm[0] = title_mp3_bin;
	bgm_size[0] = title_mp3_bin_size;
	
	bgm[1] = bgm1_mp3_bin;
	bgm_size[1] = bgm1_mp3_bin_size;
	
	bgm[2] = bgm2_mp3_bin;
	bgm_size[2] = bgm2_mp3_bin_size;
	
	bgm[3] = bgm3_mp3_bin;
	bgm_size[3] = bgm3_mp3_bin_size;
	
	bgm[4] = bgm4_mp3_bin;
	bgm_size[4] = bgm4_mp3_bin_size;
	
	bgm[5] = win1_mp3_bin;
	bgm_size[5] = win1_mp3_bin_size;
	
	bgm[6] = win2_mp3_bin;
	bgm_size[6] = win2_mp3_bin_size;
	
	bgm[7] = win3_mp3_bin;
	bgm_size[7] = win3_mp3_bin_size;
	
	bgm[8] = win4_mp3_bin;
	bgm_size[8] = win4_mp3_bin_size;
	
	bgm[9] = bgm9_mp3_bin;
	bgm_size[9] = bgm9_mp3_bin_size;
}

int LoadSFX(SoundEffect *sfx, const void *data, int size)
{
	//int alloc_size = (estimated_size + 31) & ~31;
    sfx->pcm_size = size * 20; // Temporary buffer size

    //sfx->pcm = malloc(sfx->pcm_size);
	sfx->pcm = memalign(32, sfx->pcm_size);
	//sfx->pcm_size = alloc_size;

    if(!sfx->pcm)
        return 0;

    if( DecodeAudio((void *)data,size,sfx->pcm,&sfx->pcm_size,&sfx->freq,&sfx->stereo) != 0)
    {
        free(sfx->pcm);
        sfx->pcm = NULL;
        return 0;
    }

    return 1;
}

void init_sfx()
{
	LoadSFX(&sfx_flag, flag_ogg_bin, flag_ogg_bin_size);
	LoadSFX(&sfx_unflag, unflag_mp3_bin, unflag_mp3_bin_size);
	LoadSFX(&sfx_9move, nine_move_ogg_bin, nine_move_ogg_bin_size);
	LoadSFX(&sfx_9stop, nine_stop_ogg_bin, nine_stop_ogg_bin_size);
}








#define MAX_W 72
#define MAX_H 48

#define TOTAL_SHEETS 5

typedef struct
{
	u8 mine;
	u8 revealed;
	u8 flagged;
	u8 neighbors;
} Cell;

Cell board[MAX_W][MAX_H];
float RENDER_SIZE;

int GRID_W = 16;
int GRID_H = 16;
int MINES  = 40;
int mines_count = 0;
int RENDERING_PIXELS = 16;
float RENDERING_SIZE = 2.0f;
int FIRST_CLICK = 1;

int fp_width = 9;
int fp_height = 9;
int fp_mines = 10;
int fp_music = 1;
int fp_back = 1;
int fp_menu = 0;

int cursorX;
int cursorY;
int gamewin = 0;
int gamelose = 0;
int in_game = 0;

int boss9_started = 0;
int boss9_timer = 0;
typedef struct
{
    bool active;

    int x, y;

    int dx, dy;

    int toMove, wait;
    int beforeToMove;

    bool isMoving;

    int angle, flip;
	float speed, rotation;

    int tempGrid[3];
    bool tempReveal[3];
    bool tempFlagged[3];

} Boss9;
Boss9 boss9;
//boss9.x = 0;
//boss9.y = 0;




//u64 now;
//u64 timer_start = 0;
float timer_seconds = 0;
int timer_running = 0;

int screen_w = 848 / 2;
int screen_h = 512 / 2;

int offset_x;
int offset_y;
int ui_offset_y = 66;

int revealed_total = 0;
int valid_total = 0;

static int fps = 0;
static int frames = 0;
static u64 last_time = 0;










#define SAVE_DIR "PS3SWEEPERPLUS"

typedef struct
{
	int save_level;
	int save_global_minecoins;
	int save_current_minecoins;
	int save_global_route;
} SaveData;

SaveData gSave;
sysSaveBufferSettings buffer;

enum
{
    SAVE_ICON,
    SAVE_DATA,
    SAVE_DONE
};

static int saveMode;

volatile int save_running = 0;
sys_ppu_thread_t save_thread;






void savestate_init()
{
	memset(&buffer, 0, sizeof(buffer));

	buffer.maxDirectories = 1;
	buffer.maxFiles = 1;
	buffer.bufferSize = 1024 * 1024;
	buffer.buffer = malloc(buffer.bufferSize);
	printf("buffer = %p\n", buffer.buffer);
}

void status_cb(
    sysSaveCallbackResult *result,
    sysSaveStatusIn *in,
    sysSaveStatusOut *out)
{
    static sysSaveSystemFileParam param;

    memset(&param, 0, sizeof(param));

	snprintf(param.title, sizeof(param.title), "%s", global_name);
    snprintf(param.subtitle, sizeof(param.subtitle), "Game Save Data (%s)", global_version);
    snprintf(param.detail, sizeof(param.detail),
		"Current Level: %d\nCurrent Episode: 1", current_level);

    if(out)
	{
		out->setParam = &param;
	}

    result->result = SYS_SAVE_CALLBACK_RESULT_CONTINUE;
}

void save_file_cb(
    sysSaveCallbackResult *result,
    sysSaveFileIn *in,
    sysSaveFileOut *out)
{
	switch(saveMode)
    {
        case SAVE_ICON:
		{
            out->fileOperation = SYS_SAVE_FILE_OPERATION_WRITE;
            out->fileType      = SYS_SAVE_FILETYPE_CONTENT_ICON0;

            out->buffer     = (void *)icon0_png_bin;
            out->bufferSize = icon0_png_bin_size;
            out->size       = icon0_png_bin_size;

            result->result = SYS_SAVE_CALLBACK_RESULT_CONTINUE;
			result->incrementProgress = 67;
            saveMode = SAVE_DATA;
            break;
		}
        case SAVE_DATA:
		{
			static char filename[] = "SAVE.DAT";
			
            out->fileOperation = SYS_SAVE_FILE_OPERATION_WRITE;
            out->fileType      = SYS_SAVE_FILETYPE_STANDARD_FILE;
            out->filename      = filename;

            out->buffer     = &gSave;
            out->bufferSize = sizeof(gSave);
            out->size       = sizeof(gSave);

			result->incrementProgress = 100;
            saveMode = SAVE_ICON;
            result->result = SYS_SAVE_CALLBACK_RESULT_DONE;
            break;
		}
    }
}

void load_file_cb(
    sysSaveCallbackResult *result,
    sysSaveFileIn *in,
    sysSaveFileOut *out)
{
	static char filename[] = "SAVE.DAT";
	
	out->fileOperation = SYS_SAVE_FILE_OPERATION_READ;
	out->fileType      = SYS_SAVE_FILETYPE_STANDARD_FILE;
	out->filename      = filename;

	out->buffer     = &gSave;
	out->bufferSize = sizeof(gSave);
	out->size       = sizeof(gSave);

	result->incrementProgress = 100;
	result->result = SYS_SAVE_CALLBACK_RESULT_DONE;
}







void UpdateFPS()
{
    u64 now = sysGetSystemTime();
    //now = sysGetSystemTime();

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










pngData spritesheet[TOTAL_SHEETS];
u32 sprite_off[TOTAL_SHEETS];

void Load_PNG()
{
	pngLoadFromBuffer(captain_sheet_png_bin, captain_sheet_png_bin_size, &spritesheet[0]);
	pngLoadFromBuffer(tiles_sheet_png_bin, tiles_sheet_png_bin_size, &spritesheet[1]);
	pngLoadFromBuffer(ui_sheet_png_bin, ui_sheet_png_bin_size, &spritesheet[2]);
	pngLoadFromBuffer(boss_sheet_png_bin, boss_sheet_png_bin_size, &spritesheet[3]);
	pngLoadFromBuffer(boss1_sheet_png_bin, boss1_sheet_png_bin_size, &spritesheet[4]);
}






void game_lost()
{
	if(boss9_started)
	{
		boss9_started = 0;
		boss9.active = false;
	}
	gamelose = 1;
	timer_running = 0;
	StopAudio();
}









void PlayBGM(int id, int inf)
{
    StopAudio();

    FILE *fp = (FILE *)mem_open((char *)bgm[id], bgm_size[id]);
    if(inf) PlayAudiofd(fp, 0, AUDIO_INFINITE_TIME); else PlayAudiofd(fp, 0, AUDIO_ONE_TIME);

    current_bgm = id;
}

void PlaySFX(SoundEffect *sfx)
{
	int voice = SND_GetFirstUnusedVoice();

    if (voice < 1) return;
	
    SND_SetVoice(
        voice,
        sfx->stereo ? VOICE_STEREO_16BIT : VOICE_MONO_16BIT,
        sfx->freq,
        0,
        sfx->pcm,
        sfx->pcm_size,
        255,
        255,
        NULL);
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
	
	RENDER_SIZE = RENDERING_SIZE * RENDERING_PIXELS;
	
	float sx = 800.0f / GRID_W;
	float sy = 460.0f / GRID_H;

	RENDER_SIZE = (sx < sy) ? sx : sy;
	
	if(RENDER_SIZE > 32.0f) RENDER_SIZE = 2.0f * RENDERING_PIXELS;
	
	offset_x = screen_w - (RENDER_SIZE * GRID_W / 2);
	offset_y = screen_h - (RENDER_SIZE * GRID_H / 2) + ui_offset_y;
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

int calculate_score()
{
	//if (win_tiles == 1): return 1
	//if (Main.practice): return 0
	//var double_multiplier = 1.0
	//var anti_multiplier = 1.0
	//var larger
	
	// if (doublemines):
		// if (Main.double_method):
			// if (Main.double_number != 0 && mines != 0):
				// if (Main.double_number >= mines - Main.double_number): larger = Main.double_number
				// else: larger = mines - Main.double_number
				// double_multiplier = 1.0 + (float(larger) / float(mines))
		// else:
			// double_multiplier = 1.0 + (0.5 - abs(0.5 - (float(Main.double_number) / 100)))
	// if (antimines):
		// if (Main.anti_method):
			// if (Main.anti_number != 0 && mines != 0):
				// if (Main.anti_number >= mines - Main.anti_number): larger = Main.anti_number
				// else: larger = mines - Main.anti_number
				// anti_multiplier = 1.0 + (float(larger) / float(mines))
		// else:
			// anti_multiplier = 1.0 + (0.5 - abs(0.5 - (float(Main.anti_number) / 100)))
	
	int score = 0;
	int i, j;
	
	for(i = 0; i < GRID_W; i++)
	{
		for(j = 0; j < GRID_H; j++)
		{
			if(!board[i][j].mine && board[i][j].neighbors < 9) //&& tile_array[i][j].mine_value > 0)
			{
				score += pow(2, board[i][j].neighbors - 1);
			}
			//if(tile_array[i][j].antimine)score++;  //< we are not ready for this yet.
			//if(tile_array[i][j].doublemine)score++;
		}
	}
	//if (Main.nine): score += 999
	//if (Main.ten): score += 1000
	//if (Main.eleven): score += 1111
	//if (Main.nine_count > 0): score += 9
	//if (Main.ten_count > 0): score += 10
	//if (Main.eleven_count > 0): score += 11
	score += MINES;
	//if (Main.db["master"]): score \*= 1.5
	//score \*= double_multiplier
	//score \*= anti_multiplier
	return score;
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
		mines_count = 0;
		timer_running = 0;
		StopAudio();
		
		if(current_level==8)
		{
			boss9_started = 0;
			boss9.active = false;
		}
		else PlayBGM(4+desired_bgm, 0);
		
		int fx, fy;
		
		for (fx = 0; fx < GRID_W; fx++)
		{
			for (fy = 0; fy < GRID_H; fy++)
			{
				if (board[fx][fy].mine) board[fx][fy].flagged = 1;
			}
		}
		
		level_minecoins = calculate_score();
		current_minecoins += level_minecoins;
		global_minecoins += current_minecoins;
	}
}

void Reveal(int x,int y)
{
    if(x < 0 || y < 0 || x >= GRID_W || y >= GRID_H) return;
    if(board[x][y].revealed || board[x][y].flagged) return;

    board[x][y].revealed = 1;
	
    if(board[x][y].mine)
	{
		game_lost();
		return;
	}

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

void Chord(int x, int y)
{
    if(x < 0 || y < 0 || x >= GRID_W || y >= GRID_H) return;
    if(!board[x][y].revealed) return;
	if(board[x][y].neighbors == 0) return;

    int required = board[x][y].neighbors;
    int flags = 0;

    int nx, ny;

    for(nx = x - 1; nx <= x + 1; nx++)
    {
        for(ny = y - 1; ny <= y + 1; ny++)
        {
            if(nx >= 0 && ny >= 0 &&
               nx < GRID_W && ny < GRID_H)
            {
                if(board[nx][ny].flagged) flags++;
            }
        }
    }

    if(flags == required)
    {
        for(nx = x - 1; nx <= x + 1; nx++)
        {
            for(ny = y - 1; ny <= y + 1; ny++)
            {
                if(nx >= 0 && ny >= 0 &&
                   nx < GRID_W && ny < GRID_H)
                {
                    if(!board[nx][ny].flagged)
                    {
                        if(board[nx][ny].mine)
                        {
                            game_lost();
                            return;
                        }

                        Reveal(nx, ny);
                    }
                }
            }
        }
    }
}

void LoadLevel(int level)
{
    GRID_W = levels[level].width;
    GRID_H = levels[level].height;
    MINES  = levels[level].mines;
	
	if(current_level==8) MINES-=8;
	
    mines_count = MINES;

    cursorX = GRID_W/2;
    cursorY = GRID_H/2;

    FIRST_CLICK = 1;
    gamewin = 0;
    gamelose = 0;
	level_minecoins = 0;
	
	timer_running = 0;
	timer_seconds = 0;
	
	boss9.x = GRID_W/2;
	boss9.y = GRID_H/2;

    GenerateBoard();
    CalculateNumbers();

    desired_bgm = levels[level].music;
	the_bg = levels[level].back;
    PlayBGM(desired_bgm, 1);
}

void LoadFPLevel()
{
    GRID_W = fp_width;
    GRID_H = fp_height;
    MINES  = fp_mines;
	
    mines_count = MINES;

    cursorX = GRID_W/2;
    cursorY = GRID_H/2;

    FIRST_CLICK = 1;
    gamewin = 0;
    gamelose = 0;
	level_minecoins = 0;
	
	timer_running = 0;
	timer_seconds = 0;
	
	boss9.x = GRID_W/2;
	boss9.y = GRID_H/2;

    GenerateBoard();
    CalculateNumbers();

    desired_bgm = fp_music;
	the_bg = fp_back;
    PlayBGM(desired_bgm, 1);
}

int getCaptainState(int anim_z)
{
	int the_state = 0;
	switch(the_bg)
	{
		case 0:
		{
			if(gamewin) the_state = 0+anim_z; else if(gamelose) the_state = 74; else the_state = 24;
			break;
		}
		case 1:
		{
			if(gamewin) the_state = 4+anim_z; else if(gamelose) the_state = 78; else the_state = 28;
			break;
		}
		case 2:
		{
			if(gamewin) the_state = 8+anim_z; else if(gamelose) the_state = 82+anim_z; else the_state = 32+anim_z;
			break;
		}
		case 3:
		{
			if(gamewin) the_state = 16+anim_z; else if(gamelose) the_state = 90; else the_state = 40;
			break;
		}
	}
	return the_state;
}





int RandRange(int min, int max)
{
	int rndom;
	rndom = rand() % (max-min+1) + min;
	return rndom;
}

int max(int min, int max)
{
	int biggest;
	biggest = (min > max) ? min : max;
	
	return biggest;
}









void Boss9PlaceTiles()
{
	int x, y;
	
	for(x = boss9.x - 1; x <= boss9.x + 1; x++)
    {
        for(y = boss9.y - 1; y <= boss9.y + 1; y++)
        {
			board[x][y].mine = 1;
			board[x][y].revealed = false;
        }
    }
}

void Boss9Stop()
{
    boss9.beforeToMove = 0;
    boss9.toMove = 0;
    boss9.isMoving = false;
    boss9.angle = -boss9.angle;

    if(global_route==1) boss9_timer = RandRange(15,50);
    else boss9_timer = RandRange(20,60);

    PlaySFX(&sfx_9stop);
}

void Boss9SaveStrip()
{
    int sx, sy, i;

    if(boss9.dx == 1)
    {
        sx = boss9.x + 2;

        for(i=0;i<3;i++)
        {
            sy = boss9.y - 1 + i;

            boss9.tempGrid[i] = board[sx][sy].mine;
            boss9.tempReveal[i] = board[sx][sy].revealed;
			//board[sx][sy].flagged = false;
			boss9.tempFlagged[i] = board[sx][sy].flagged;
        }
    }
	else if(boss9.dx == -1)
    {
        sx = boss9.x - 2;

        for(i=0;i<3;i++)
        {
            sy = boss9.y - 1 + i;

            boss9.tempGrid[i] = board[sx][sy].mine;
            boss9.tempReveal[i] = board[sx][sy].revealed;
			//board[sx][sy].flagged = false;
			boss9.tempFlagged[i] = board[sx][sy].flagged;
        }
    }
	else if(boss9.dy == 1)
    {
        sy = boss9.y + 2;

        for(i=0;i<3;i++)
        {
            sx = boss9.x - 1 + i;

            boss9.tempGrid[i] = board[sx][sy].mine;
            boss9.tempReveal[i] = board[sx][sy].revealed;
			//board[sx][sy].flagged = false;
			boss9.tempFlagged[i] = board[sx][sy].flagged;
        }
    }
	else if(boss9.dy == -1)
    {
        sy = boss9.y - 2;

        for(i=0;i<3;i++)
        {
            sx = boss9.x - 1 + i;

            boss9.tempGrid[i] = board[sx][sy].mine;
            boss9.tempReveal[i] = board[sx][sy].revealed;
			//board[sx][sy].flagged = false;
			boss9.tempFlagged[i] = board[sx][sy].flagged;
        }
    }
}

void Boss9RestoreStrip()
{
    int sx, sy, i;

	if (boss9.dx == 1)
	{
		sx = boss9.x - 2;

		for (i = 0; i < 3; i++)
		{
			sy = boss9.y - 1 + i;

			board[sx][sy].mine = boss9.tempGrid[i];
			board[sx][sy].revealed = boss9.tempReveal[i];
			board[sx][sy].flagged = boss9.tempFlagged[i];
		}
	}
	else if (boss9.dx == -1)
	{
		sx = boss9.x + 2;

		for (i = 0; i < 3; i++)
		{
			sy = boss9.y - 1 + i;

			board[sx][sy].mine = boss9.tempGrid[i];
			board[sx][sy].revealed = boss9.tempReveal[i];
			board[sx][sy].flagged = boss9.tempFlagged[i];
		}
	}
	else if (boss9.dy == 1)
	{
		sy = boss9.y - 2;

		for (i = 0; i < 3; i++)
		{
			sx = boss9.x - 1 + i;

			board[sx][sy].mine = boss9.tempGrid[i];
			board[sx][sy].revealed = boss9.tempReveal[i];
			board[sx][sy].flagged = boss9.tempFlagged[i];
		}
	}
	else if (boss9.dy == -1)
	{
		sy = boss9.y + 2;

		for (i = 0; i < 3; i++)
		{
			sx = boss9.x - 1 + i;

			board[sx][sy].mine = boss9.tempGrid[i];
			board[sx][sy].revealed = boss9.tempReveal[i];
			board[sx][sy].flagged = boss9.tempFlagged[i];
		}
	}
}

bool Boss9CanMove(int nx, int ny)
{
	int x, y;
    if(nx < 1 || nx >= GRID_W - 1)
        return false;

    if(ny < 1 || ny >= GRID_H - 1)
        return false;

    for(y=-1;y<=1;y++)
    {
        for(x=-1;x<=1;x++)
        {
            if(x==0 && y==0)
                continue;

            //if(circle_grid[nx+x][ny+y])
            //    return false;
        }
    }

    return true;
}

void Boss9PickDirection()
{
    boss9.toMove = rand() % max(GRID_W, GRID_H) + 1;
    boss9.beforeToMove = boss9.toMove;

    switch(rand() % 4)
    {
        case 0: boss9.dx = 1;  boss9.dy = 0; break;
        case 1: boss9.dx = -1; boss9.dy = 0; break;
        case 2: boss9.dx = 0;  boss9.dy = 1; break;
        case 3: boss9.dx = 0;  boss9.dy = -1; break;
    }

    boss9.isMoving = true;
}

void UpdateBoss9()
{
    if(!boss9.active)
        return;

    if(!boss9.isMoving)
        Boss9PickDirection();
    else
        boss9.toMove--;

    if(boss9.toMove <= 0)
    {
        Boss9Stop();
        return;
    }

    int nx = boss9.x + boss9.dx;
    int ny = boss9.y + boss9.dy;

    if(!Boss9CanMove(nx, ny))
    {
        Boss9Stop();
        return;
    }

    Boss9SaveStrip();

    boss9.x = nx;
    boss9.y = ny;

    Boss9PlaceTiles();

    Boss9RestoreStrip();
	CalculateNumbers();

    PlaySFX(&sfx_9move);
}









int TTFLoadFont(char * path, void * from_memory, int size_from_memory)
{
    if(!ttf_inited) FT_Init_FreeType(&freetype);
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

    // Center of sprite
    float cx = x + wid2 * 0.5f;
    float cy = y + hei2 * 0.5f;

    // Half-size
    float hw = wid2 * 0.5f;
    float hh = hei2 * 0.5f;

    // Rotation
    float rad = angle * (M_PI / 180.0f);
    float c = cosf(rad);
    float s = sinf(rad);

    // Rotated corners
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








void draw_game()
{
	SetTexture(1);
	float x, y;
	int x1, y1 = 0;
	int x0, y0 = 0;
	u32 color = 0xffffffff;
	
    static int count_frames = 0;
    static int anim = 0;
	
	x= 0.0; y = 0.0;
	count_frames++;
	
	anim = (count_frames / 30) % 2;
	
	for(x0 = -99; x0 < (screen_w * 2) + 99; x0 += 32)
	{
		for(y0 = -99; y0 < (screen_h * 2) + 99; y0 += 32)
		{
			DrawSprite2D((the_bg * 20) + anim, x0, y0, 32, 32, 16, 16, 320, 320, 5, 0xffffff88);
		}
	}
	
	for(x1=0;x1<GRID_W;x1++)
	{
		for(y1=0;y1<GRID_H;y1++)
		{
			int tile = 0;
			
			if(board[x1][y1].revealed)
			{
				DrawSprite2D((the_bg * 20) + anim + 2, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
					RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 4, color);
				
				switch(board[x1][y1].neighbors)
				{
					case 1: tile = 105; break;
					case 2: tile = 106; break;
					case 3: tile = 107; break;
					case 4: tile = 124; break;
					case 5: tile = 125; break;
					case 6: tile = 126; break;
					case 7: tile = 127; break;
					case 8: tile = 144; break;
				}
				
				if(board[x1][y1].mine) tile = 114+anim;
				
				if(board[x1][y1].neighbors > 0 || board[x1][y1].mine)
				{
					DrawSprite2D(tile, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
						RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 3, color);
				}
			}
			else
			{
				DrawSprite2D((the_bg * 20) + anim, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
					RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 4, color);
				
				if(board[x1][y1].mine && gamelose == 1)
				{
					DrawSprite2D(114+anim, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
						RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 3, color);
				}
				
				if(board[x1][y1].flagged)
				{
					if(board[x1][y1].mine == 0 && gamelose)
						DrawSprite2D(132, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
							RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 3, color);
					else if(!gamelose)
						DrawSprite2D(112 + anim, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
							RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 3, color);
				}
			}
			
			if(x1 == cursorX && y1 == cursorY)
			{
				DrawSprite2D(240, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
					RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 2, color);
			}
			else
			{
				DrawSprite2D(241 + the_bg, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
					RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 2, 0xffffff88);
			}
		}
	}
	
	if(current_level==8)
	{
		if(global_route==1) SetTexture(4);
		else SetTexture(3);
		
		DrawSprite2DR(0, offset_x+(boss9.x*RENDER_SIZE)-RENDER_SIZE, offset_y+(boss9.y*RENDER_SIZE)-RENDER_SIZE,
			RENDER_SIZE*3, RENDER_SIZE*3, 384, 384, 1536, 1536, boss9.rotation, -1, color);
		DrawSprite2D(1, offset_x+(boss9.x*RENDER_SIZE)-RENDER_SIZE, offset_y+(boss9.y*RENDER_SIZE)-RENDER_SIZE,
			RENDER_SIZE*3, RENDER_SIZE*3, 384, 384, 1536, 1536, -1, color);
	}
}








void drawScene()
{
	u32 color = 0xffffffff;
    tiny3d_Project2D();

    static int count_frames = 0;
    static int anim = 0;
    static int anim1 = 0;
	SetFontSize(28, 32);
	
	if(in_game)
	{
		draw_game();
	}
	
    SetFontColor(0xffffffff, 0x00000000);
    SetCurrentFont(0);
	
	count_frames++;
	
	anim = (count_frames / 30) % 2;
	anim1 = (count_frames / 15) % 2;
	
	if(!in_game)
	{
		if(!free_play)
		{
			//DrawFormatString(-20,256,"%d x %d",Video_Resolution.width,Video_Resolution.height);
			DrawFormatString(-20,0, "%s (%s)", global_name, global_version);
			DrawFormatString(-20,32, "Minesweeper Plus First Anniversary!");
			DrawFormatString(-20,484, "%s", splash);
			DrawFormatString(-20,128,"START - New Game");
			DrawFormatString(-20,160,"SELECT - Continue");
			DrawFormatString(-20,192,"SQUARE - Freeplay");
			DrawFormatString(-20,224,"TRIANGLE - Credits");
		}
		else
		{
			DrawFormatString(-20,0, "Freeplay Menu");
			DrawFormatString(-20,64,"Width: %d", fp_width);
			DrawFormatString(-20,96,"Height: %d", fp_height);
			DrawFormatString(-20,128,"Mines count: %d", fp_mines);
			DrawFormatString(-20,160,"Music Type: %d", fp_music);
			DrawFormatString(-20,192,"Background Type: %d", fp_back);
			DrawFormatString(-20,256,"START - Start the game");
			DrawFormatString(-20,288,"CIRCLE - Go Back");
			
			DrawFormatString(410,64+(fp_menu*32),"<");
		}
	}
	else
	{
		SetTexture(2);
		SetFontSize(84, 96);
		DrawSpriteUI(-34, -22, 0, 0, 384, 192, 1024, 1024, 302, 112, 1, color);
		DrawSpriteUI(576, -22, 0, 0, 384, 192, 1024, 1024, 302, 112, 1, color);
		DrawSpriteUI(268, -22, 384, 0, 768, 192, 1024, 1024, 308, 112, 1, color);
		DrawSpriteUI(-20, -10, 0+(anim*264), 192, 264+(anim*264), 456, 1024, 1024, 94, 94, 1, color);
		DrawSpriteUI(586, -10, 0+(anim*264), 456, 264+(anim*264), 720, 1024, 1024, 94, 94, 1, color);
		DrawFormatString(94,-26, "%03d", mines_count);
		DrawFormatString(714,-26, "%03d", (int)timer_seconds);
		SetTexture(0);
		SetFontSize(28, 32);
		
		int capt_state = getCaptainState(anim1);
		
		DrawSprite2D(capt_state, 375, -14, 94, 94, 40, 40, 960, 560, 0, color);
		//DrawFormatString(-20,32, "TOTAL MINECOINS: %d", global_minecoins);
		//DrawFormatString(-20,64, "MINECOINS: %d", current_minecoins);
		DrawFormatString(-20,88, "FPS: %d", fps);
	}
	SetFontSize(28, 32);
	
	if(gamelose)
	{
		DrawFormatString(-20,160,"GAME OVER");
		DrawFormatString(-20,192,"PRESS START");
	}
	else if(gamewin)
	{
		DrawFormatString(-20,160,"LEVEL CLEAR!");
		DrawFormatString(-20,320,"+ %d Minecoins!", level_minecoins);
		if(!free_play)
		{
			DrawFormatString(-20,192,"START - Next Level");
			DrawFormatString(-20,224,"SELECT - Save Progress");
		}
		else
		{
			DrawFormatString(-20,192,"START - Continue");
			DrawFormatString(-20,224,"SELECT - Reset");
		}
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
	
    for(g = 0; g < TOTAL_SHEETS; g++) {
		memcpy(texture_pointer, spritesheet[g].bmp_out, spritesheet[g].pitch * spritesheet[g].height);
		sprite_off[g] = tiny3d_TextureOffset(texture_pointer);
		
		free(spritesheet[g].bmp_out);
		spritesheet[g].bmp_out = NULL;
		
		texture_pointer += ((spritesheet[g].pitch * spritesheet[g].height + 15) & ~15) / 4;
    }
}






void DemoCompleteCallback(msgButton button, void *usrData)
{
    sysProcessExit(0);
}
void EmptyDialogCallback(msgButton button, void *usrData)
{
    // ehhehe fuck this shit
}
void StartMusicCallback(msgButton button, void *usrData)
{
	game_loaded = 1;
    PlayBGM(0, 1);
}













int main(int argc, const char* argv[])
{
	padInfo padinfo;
	int i, dick;
	static padData oldpad;
	padData paddata;
	
	srand(time(NULL));
	tiny3d_Init(1024*1024);
	
	sysModuleLoad(SYSMODULE_PNGDEC);
	savestate_init();
	Initialize();
	splash_count = sizeof(splashes_random) / sizeof(splashes_random[0]);
	
	for (dick = 0; dick < rand() % 100; dick++)
	{
		splash = splashes_random[rand() % splash_count];
	}
	
	boss9.speed = 2.0f;
	boss9.angle = 1;
	
	if(global_route==1) boss9.speed = 6.0f;
	
	init_bgm();
	init_sfx();
	
	ioPadInit(7);
    LoadTexture();
	
	msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
		"Welcome to PS3Sweeper+ v0.09!\n\nThis game is still under huge development, so please join my discord server to not miss out new updates!\nLINK: https://discord.gg/7XuQDYjyzt"
		, StartMusicCallback, NULL, NULL);
	
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
				
				if(!in_game)
				{
					if(!free_play)
					{
						if(paddata.BTN_START && !oldpad.BTN_START)
						{
							current_level = 0;
							current_minecoins = 0;
							StopAudio();
							
							if(paddata.BTN_L3)
							{
								if(paddata.BTN_R3) global_route = 1;
								current_level=8;
							}
							
							LoadLevel(current_level);
							in_game = 1;
						}
						if(paddata.BTN_SELECT && !oldpad.BTN_SELECT)
						{
							s32 ret;
							
							drawScene();
							DrawFormatString(-20,256,"Loading...");
							tiny3d_Flip();
							
							ret = sysSaveAutoLoad2(SYS_SAVE_CURRENT_VERSION,SAVE_DIR,
								SYS_SAVE_ERROR_DIALOG_SHOW,&buffer,status_cb,
								load_file_cb,0,NULL);
							
							printf("LOAD RET = %08x\n", ret);
							printf("LEVEL = %d\n", gSave.save_level);
							
							if(ret == SYS_SAVE_RETURN_DONE)
							{
								if(gSave.save_level == 0 && gSave.save_current_minecoins == 0 && gSave.save_global_minecoins == 0)
								{
									msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
										"Load Success.", EmptyDialogCallback, NULL, NULL);
									
									current_level = gSave.save_level;
									current_minecoins = gSave.save_current_minecoins;
									global_minecoins = gSave.save_global_minecoins;
									free_play = 0;
									StopAudio();
									LoadLevel(current_level);
									in_game = 1;
								}
							}
							else
							{
								msgDialogOpen2(MSG_DIALOG_ERROR | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
									"Load Failed! Data either non-existent or corrupted.", EmptyDialogCallback, NULL, NULL);
							}
						}
						if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE)
						{
							msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
								"CREDITS\n\nOriginal Minesweeper Plus game made by Jorel Simpson\nXMB Background made by Zeltizy",
								EmptyDialogCallback, NULL, NULL);
						}
						if(paddata.BTN_SQUARE && !oldpad.BTN_SQUARE)
						{
							free_play = 1;
						}
					}
					else
					{
						if(paddata.BTN_START && !oldpad.BTN_START)
						{
							LoadFPLevel();
							in_game = 1;
						}
						
						if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
						{
							splash = splashes_random[rand() % splash_count];
							free_play = 0;
						}
						
						if(paddata.BTN_UP && !oldpad.BTN_UP){
							fp_menu--;
							if (fp_menu<0) fp_menu = 0;
						}
						if(paddata.BTN_DOWN && !oldpad.BTN_DOWN){
							fp_menu++;
							if (fp_menu>4) fp_menu = 4;
						}
						
						if(paddata.BTN_LEFT && !oldpad.BTN_LEFT){
							switch(fp_menu)
							{
								case 0: fp_width--; if(fp_width<3) fp_width=3; break;
								case 1: fp_height--; if(fp_height<3) fp_height=3; break;
								case 2: fp_mines--; if(fp_mines<1) fp_mines=1; break;
								case 3: fp_music--; if(fp_music<1) fp_music=1; break;
								case 4: fp_back--; if(fp_back<1) fp_back=1; break;
							}
							
							if(fp_mines>(fp_width*fp_height)-1) fp_mines=(fp_width*fp_height)-1;
						}
						if(paddata.BTN_RIGHT && !oldpad.BTN_RIGHT){
							switch(fp_menu)
							{
								case 0: fp_width++; if(fp_width>48) fp_width=48; break;
								case 1: fp_height++; if(fp_height>36) fp_height=36; break;
								case 2: fp_mines++; break;
								case 3: fp_music++; if(fp_music>4) fp_music=4; break;
								case 4: fp_back++; if(fp_back>4) fp_back=4; break;
							}
							
							if(fp_mines>(fp_width*fp_height)-1) fp_mines=(fp_width*fp_height)-1;
						}
					}
				}
				
				if(gamelose)
				{
					if(paddata.BTN_START && !oldpad.BTN_START)
					{
						in_game = 0;

						gamewin = 0;
						gamelose = 0;
						current_minecoins = 0;
						
						splash = splashes_random[rand() % splash_count];
						free_play = 0;
						
						PlayBGM(0, 1);
					}
				}
				else if(gamewin)
				{
					if(paddata.BTN_SELECT && !oldpad.BTN_SELECT)
					{
						if(!free_play)
						{
							drawScene();
							DrawFormatString(-20,386,"Saving...");
							tiny3d_Flip();
							
							s32 ret;
							gSave.save_level = current_level + 1;
							gSave.save_current_minecoins = current_minecoins;
							gSave.save_global_minecoins = global_minecoins;
							gSave.save_global_route = global_route;
							
							ret = sysSaveAutoSave2(SYS_SAVE_CURRENT_VERSION,SAVE_DIR,
								SYS_SAVE_ERROR_DIALOG_SHOW,&buffer,status_cb,save_file_cb,
								0,NULL);
							
							printf("SAVE RET = %08x\n", ret);
							
							msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
								"Save Completed.", EmptyDialogCallback, NULL, NULL);
						}
						else LoadFPLevel();
					}

					if(paddata.BTN_START && !oldpad.BTN_START)
					{
						if(!free_play)
						{
							current_level++;

							if(current_level >= LEVEL_COUNT)
							{
								msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
									"The DEMO is completed. Returning to XMB..",
									DemoCompleteCallback, NULL, NULL);
							}
							else
							{
								LoadLevel(current_level);
							}
						}
						else
						{
							in_game = 0;

							gamewin = 0;
							gamelose = 0;
							current_minecoins = 0;
							
							splash = splashes_random[rand() % splash_count];
							free_play = 0;
							
							PlayBGM(0, 1);
						}
					}
				}
				
				if (!gamewin && !gamelose && in_game)
				{					
					// basic
					if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
					{
						if(FIRST_CLICK)
						{
							//timer_start = sysGetSystemTime();
							timer_running = 1;
						}
						
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
						
						if(current_level==8 && !boss9_started)
						{
							boss9_started = 1;
							boss9.active = true;
						}
						
						if(boss9_started && cursorX == boss9.x && cursorY == boss9.y)
						{
							WinCheck();
						}
						else
						{
							if (board[cursorX][cursorY].revealed)
							{
								Chord(cursorX, cursorY);
							}
							else
							{
								Reveal(cursorX, cursorY);
							}
						}
						
						if(!boss9_started) WinCheck();
					}
					if(paddata.BTN_CIRCLE && !oldpad.BTN_CIRCLE)
					{
						if(!board[cursorX][cursorY].revealed)
						{
							board[cursorX][cursorY].flagged = !board[cursorX][cursorY].flagged;
							if(board[cursorX][cursorY].flagged)
							{
								PlaySFX(&sfx_flag);
								mines_count--;
							}
							else
							{
								PlaySFX(&sfx_unflag);
								mines_count++;
							}
						}
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
					
					// zoom check
					if(paddata.BTN_R1 && !oldpad.BTN_R1) zoomCheck(1);
					if(paddata.BTN_L1 && !oldpad.BTN_L1) zoomCheck(0);
				}
				oldpad = paddata;
			}
		}
		
		if(boss9_started)
		{
			boss9_timer--;

			if(boss9_timer <= 0)
			{
				boss9_timer = 6;
				UpdateBoss9();
			}
		}
		
		if(current_level==8)
		{
			boss9.rotation += ((float)boss9.angle * boss9.speed);
		}
		
		if(timer_running)
		{
			timer_seconds += 0.018f;
			
			if(timer_seconds > 999) timer_seconds = 999;
		}
		
		if(game_loaded) drawScene();
		
		tiny3d_Flip();
		usleep(3666);
    }

    return 0;
}