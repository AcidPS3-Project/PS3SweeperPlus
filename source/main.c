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
#include <np/trophy.h>

#include <ft2build.h>
#include <freetype/freetype.h> 
#include <freetype/ftglyph.h>

#include "asimov_ttf_bin.h"

#include "soundlib/spu_soundlib.h"
#include "soundlib/audioplayer.h"
#include "spu_soundmodule.bin.h"

// IMPORT IMAGES
#include "captain_sheet_png_bin.h"
#include "tiles_sheet_png_bin.h"
#include "ui_sheet_png_bin.h"
#include "boss_sheet_png_bin.h"
#include "boss1_sheet_png_bin.h"
#include "dualboss_sheet_png_bin.h"
#include "cursed_sheet_png_bin.h"
#include "background_sheet_png_bin.h"

#include "icon0_png_bin.h"

//IMPORT MUSIC
#include "title_mp3_bin.h"

#include "bgm1_mp3_bin.h"
#include "bgm2_mp3_bin.h"
#include "bgm3_mp3_bin.h"
#include "bgm4_mp3_bin.h"
#include "bgm5_mp3_bin.h"
#include "bgm6_mp3_bin.h"
#include "bgm7_mp3_bin.h"
#include "bgm8_mp3_bin.h"

#include "win1_mp3_bin.h"
#include "win2_mp3_bin.h"
#include "win3_mp3_bin.h"
#include "win4_mp3_bin.h"
#include "win5_ogg_bin.h"
#include "win6_ogg_bin.h"
#include "win7_ogg_bin.h"
#include "win8_ogg_bin.h"

#include "ocean_mp3_bin.h"

#include "boss9_intro_mp3_bin.h"
#include "boss9_mp3_bin.h"
#include "boss9_win_ogg_bin.h"
#include "boss10_intro_mp3_bin.h"
#include "boss10_mp3_bin.h"
#include "boss10_win_ogg_bin.h"

//IMPORT SFX
#include "flag_ogg_bin.h"
#include "unflag_mp3_bin.h"
#include "nine_move_ogg_bin.h"
#include "nine_stop_ogg_bin.h"
#include "ten_bump_ogg_bin.h"
#include "ten_reset_ogg_bin.h"
#include "game_mine_ogg_bin.h"
#include "game_explosion_ogg_bin.h"

// the fuck is that
#include "episode2_shapes.h"

bool playing_bgm = false;

int EPISODE = 1;

typedef struct
{
    int width;
    int height;
    int mines;
    int music;
    int back;
    int id;
} LevelData;

LevelData levels[] =
{
    {6, 6, 3, 1, 0, 0},
    {8, 8, 6, 1, 0, 0},
    {10, 10, 10, 2, 1, 0},
    {9, 12, 12, 2, 1, 0},
    {18, 9, 20, 3, 2, 0},
    {15, 15, 30, 3, 2, 0},
    {18, 12, 33, 4, 3, 0},
    {16, 16, 42, 4, 3, 0},
    {13, 13, 29, 10, 4, 0},
    {21, 21, 60, 12, 9, 91}
};

#define LEVEL_COUNT (sizeof(levels)/sizeof(levels[0]))
//#define LEVEL_COUNT 66

int current_level = 0;
int global_route = 0;
int the_bg = 0;

int ttf_inited = 0;
int game_loaded = 0;
int free_play = 0;
int draw_grid_outlines = 0;

FT_Library freetype;
FT_Face face;





char global_name[] = "PS3Sweeper+";
char global_version[] = "v0.2";

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
	"Join AcidPS3 Server! https://discord.gg/7XuQDYjyzt",
	"Play the original game first!",
	"Also play yawn's minesweeper",
	"Also play Minesweeper Plus Plus",
	"Also play Minesweeper Minus",
	"Also play Minesweeper Plus: Second Destination (if it comes out)",
	"Also play Minesweeper Plus: Alien Captain Edition",
	"Also play Minesweeper Times- oh wait. its canceled :(",
	"Also play LandMinesweeper Plus",
	"Also play Minesweeper Plus: Hard Mode",
	"Also play Minesweeper Chronicles (if it gets hopefully finished)",
	"Also play Minesweeper Plus Roblox",
	"Also play PolySweep (if it comes out)",
	"Also play Minesweeper Plus: Deluxe (if it comes out)",
	"Also play Minesweeper Plus: Community Edition",
	"The MORSHU.",
	"Why it was rewritten?",
	"C is better, than GMS1.4",
	"PSDK3v2 is good, but the latest PSL1GHT update is better",
	"When tutorial how to make games with PSL1GHT? Oh wait...",
	"Yay, minecoins! I'm rich!!",
	"Better, than Minesweeper Plus: PS3 Edition",
	"AI-Assisted code, because I suck at C.",
	"Optimizing is at it's peakest.",
	"Baldi for PS3 re-write coming?",
	"AcidPS3 Store coming as an app? Oh wait, it is here.",
	"CELL Programming",
	"Yellow light of Death",
	"Reality Synthesizer can fail?",
	//"NEC-Tokens included!",
	"I really suck at PS3 games coding.",
	"Languages not coming since technical encoding issues, sorry.",
	"Definitely does NOT work with aPS3e!",
	"May work with RPCS3, but freezes happen. Be careful!",
	"Watch no one play this",
	"The definition of Insanity.",
	"F U C K",
	"up up down down left right left right circle cross start.\n    oh wait, it doesnt work",
	"why the fuck i stayed all night for this shit",
	"Hold START + L3 + R3 for a surprise.",
	"OK I'm not gonna use the SDL for graphics here...",
	"Eleven and twelve coming soon, trust",
	"TOUHOU KOUMAKYOU IS NOW ON PS3!!!",
	"Nagareteku toki no naka de demo kedarusa ga hora guru guru mowaate watashi kara hanareru kokoro mo mienai waa soo shiranai do you even read this shit?",
	"Oh now there is the title background :D",
	"I wanna try making MS+ for PS1 sometime soon.",
	"What R2 do? BEEP BEEP BEEP",
	"48x36 boards DO NOT GIVE YOU FRAMEDROPS YIPPEEEEE!!!!!!!"
};

int splash_count;
const char *splash;






#define BGM_LIST 200

const unsigned char *bgm[BGM_LIST];
int bgm_size[BGM_LIST];
int bgm_start[BGM_LIST];
int bgm_end[BGM_LIST];

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
SoundEffect sfx_10bump;
SoundEffect sfx_10reset;
SoundEffect sfx_mine;
SoundEffect sfx_explosion;

int sex = 2147483647;

void init_bgm()
{
	bgm[0] = title_mp3_bin;
	bgm_size[0] = title_mp3_bin_size;
	bgm_start[0] = 0;
	bgm_end[0] = sex;
	
	bgm[1] = bgm1_mp3_bin;
	bgm_size[1] = bgm1_mp3_bin_size;
	bgm_start[1] = 499;
	bgm_end[1] = 45281;
	
	bgm[2] = bgm2_mp3_bin;
	bgm_size[2] = bgm2_mp3_bin_size;
	bgm_start[2] = 7801;
	bgm_end[2] = 54617;
	
	bgm[3] = bgm3_mp3_bin;
	bgm_size[3] = bgm3_mp3_bin_size;
	bgm_start[3] = 0;
	bgm_end[3] = sex;
	
	bgm[4] = bgm4_mp3_bin;
	bgm_size[4] = bgm4_mp3_bin_size;
	bgm_start[4] = 1782;
	bgm_end[4] = 137481;
	
	bgm[5] = bgm5_mp3_bin;
	bgm_size[5] = bgm5_mp3_bin_size;
	bgm_start[5] = 0;
	bgm_end[5] = sex;
	
	bgm[6] = bgm6_mp3_bin;
	bgm_size[6] = bgm6_mp3_bin_size;
	bgm_start[6] = 13439;
	bgm_end[6] = 69080;
	
	bgm[7] = bgm7_mp3_bin;
	bgm_size[7] = bgm7_mp3_bin_size;
	bgm_start[7] = 0;
	bgm_end[7] = sex;
	
	bgm[8] = bgm8_mp3_bin;
	bgm_size[8] = bgm8_mp3_bin_size;
	bgm_start[8] = 6548;
	bgm_end[8] = 104708-10;
	
	bgm[9] = ocean_mp3_bin;
	bgm_size[9] = ocean_mp3_bin_size;
	bgm_start[9] = 0;
	bgm_end[9] = sex;
	
	bgm[10] = boss9_intro_mp3_bin;
	bgm_size[10] = boss9_intro_mp3_bin_size;
	bgm_start[10] = 29632;
	bgm_end[10] = 33763;
	
	bgm[11] = boss9_mp3_bin;
	bgm_size[11] = boss9_mp3_bin_size;
	bgm_start[11] = 10995;
	bgm_end[11] = 100527;
	
	bgm[12] = boss10_intro_mp3_bin;
	bgm_size[12] = boss10_intro_mp3_bin_size;
	bgm_start[12] = 23725;
	bgm_end[12] = 26893;
	
	bgm[13] = boss10_mp3_bin;
	bgm_size[13] = boss10_mp3_bin_size;
	bgm_start[13] = 0;
	bgm_end[13] = sex;
	
	bgm[101] = win1_mp3_bin;
	bgm_size[101] = win1_mp3_bin_size;
	bgm_start[101] = 0;
	bgm_end[101] = sex;
	
	bgm[102] = win2_mp3_bin;
	bgm_size[102] = win2_mp3_bin_size;
	bgm_start[102] = 0;
	bgm_end[102] = sex;
	
	bgm[103] = win3_mp3_bin;
	bgm_size[103] = win3_mp3_bin_size;
	bgm_start[103] = 0;
	bgm_end[103] = sex;
	
	bgm[104] = win4_mp3_bin;
	bgm_size[104] = win4_mp3_bin_size;
	bgm_start[104] = 0;
	bgm_end[104] = sex;
	
	bgm[105] = win5_ogg_bin;
	bgm_size[105] = win5_ogg_bin_size;
	bgm_start[105] = 0;
	bgm_end[105] = sex;
	
	bgm[106] = win6_ogg_bin;
	bgm_size[106] = win6_ogg_bin_size;
	bgm_start[106] = 0;
	bgm_end[106] = sex;
	
	bgm[107] = win7_ogg_bin;
	bgm_size[107] = win7_ogg_bin_size;
	bgm_start[107] = 0;
	bgm_end[107] = sex;
	
	bgm[108] = win8_ogg_bin;
	bgm_size[108] = win8_ogg_bin_size;
	bgm_start[108] = 0;
	bgm_end[108] = sex;
	
	bgm[109] = boss9_win_ogg_bin;
	bgm_size[109] = boss9_win_ogg_bin_size;
	bgm_start[109] = 0;
	bgm_end[109] = sex;
	
	bgm[110] = boss10_win_ogg_bin;
	bgm_size[110] = boss10_win_ogg_bin_size;
	bgm_start[110] = 0;
	bgm_end[110] = sex;
}

int LoadSFX(SoundEffect *sfx, const void *data, int size)
{
    sfx->pcm_size = size * 20;
	sfx->pcm = memalign(32, sfx->pcm_size);

    if(!sfx->pcm) return 0;
    if(DecodeAudio((void *)data,size,sfx->pcm,&sfx->pcm_size,&sfx->freq,&sfx->stereo) != 0)
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
	LoadSFX(&sfx_10bump, ten_bump_ogg_bin, ten_bump_ogg_bin_size);
	LoadSFX(&sfx_10reset, ten_reset_ogg_bin, ten_reset_ogg_bin_size);
	LoadSFX(&sfx_mine, game_mine_ogg_bin, game_mine_ogg_bin_size);
	LoadSFX(&sfx_explosion, game_explosion_ogg_bin, game_explosion_ogg_bin_size);
}








#define MAX_W 72
#define MAX_H 48

#define TOTAL_SHEETS 8

typedef struct
{
    short x;
    short y;
} RevealNode;

static RevealNode reveal_stack[MAX_W * MAX_H];

typedef struct
{
	u8 mine;
	u8 revealed;
	u8 flagged;
	u8 neighbors;
	u8 active;
} Cell; //heh, funny

Cell board[MAX_W][MAX_H];
float RENDER_SIZE;

int GRID_W = 16;
int GRID_H = 16;
int MINES  = 40;
int GRID_ID = 00;
int mines_count = 0;
int RENDERING_PIXELS = 16;
float RENDERING_SIZE = 2.0f;
int FIRST_CLICK = 1;

int fp_width = 9;
int fp_height = 9;
int fp_mines = 10;
int fp_music = 1;
int fp_back = 0;
int fp_menu = 0;

int cursorX;
int cursorY;
int gamewin = 0;
int gamelose = 0;
int in_game = 0;

#define MAX_BOSS9   4
#define MAX_BOSS10  4

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
	int timer;
} Boss9;

typedef struct
{
    bool active;
    float x, y;
    float hsp, vsp;
	float tspeed;
    int time;
	float speed, timer, pulse;
} Boss10;

Boss9 boss9_list[MAX_BOSS9];
Boss10 boss10_list[MAX_BOSS10];

int boss9_started = 0;
int boss10_started = 0;

int boss9_count = 1;
int boss10_count = 1;
int i_10 = 0;




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

void status_cb(sysSaveCallbackResult *result, sysSaveStatusIn *in, sysSaveStatusOut *out)
{
    static sysSaveSystemFileParam param;
	splash = splashes_random[rand() % splash_count];
    memset(&param, 0, sizeof(param));

	snprintf(param.title, sizeof(param.title), "%s", global_name);
    snprintf(param.subtitle, sizeof(param.subtitle), "Game Save Data (%s)", global_version);
    snprintf(param.detail, sizeof(param.detail), "%s\nTotal Minecoins: %d", splash, global_minecoins);
    if(out) out->setParam = &param;

    result->result = SYS_SAVE_CALLBACK_RESULT_CONTINUE;
}

void save_file_cb(sysSaveCallbackResult *result, sysSaveFileIn *in, sysSaveFileOut *out)
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

void load_file_cb(sysSaveCallbackResult *result, sysSaveFileIn *in, sysSaveFileOut *out)
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
    frames++;

    if(now - last_time >= 1000000ULL)
    {
        fps = frames;
        frames = 0;
        last_time = now;
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






void HolyShit()
{
	lv2syscall3(392, 0x1004, 0xA, 0x1B6);
}









void SetupBoss9Positions(int count)
{
    int pad = 1;
    int cx = GRID_W / 2;
    int cy = GRID_H / 2;

    if (count > MAX_BOSS9) count = MAX_BOSS9;
    boss9_count = count;

    memset(boss9_list, 0, sizeof(boss9_list));

    switch (count)
    {
        case 1:
            boss9_list[0].x = cx;
            boss9_list[0].y = cy;
            break;

        case 2:
            boss9_list[0].x = pad;
            boss9_list[0].y = pad;

            boss9_list[1].x = GRID_W - 1 - pad;
            boss9_list[1].y = GRID_H - 1 - pad;
            break;

        case 3:
            boss9_list[0].x = pad;
            boss9_list[0].y = pad;

            boss9_list[1].x = GRID_W - 1 - pad;
            boss9_list[1].y = pad;

            boss9_list[2].x = cx;
            boss9_list[2].y = GRID_H - 1 - pad;
            break;

        default: // 4+
            boss9_list[0].x = pad;
            boss9_list[0].y = pad;

            boss9_list[1].x = GRID_W - 1 - pad;
            boss9_list[1].y = pad;

            boss9_list[2].x = pad;
            boss9_list[2].y = GRID_H - 1 - pad;

            boss9_list[3].x = GRID_W - 1 - pad;
            boss9_list[3].y = GRID_H - 1 - pad;
            break;
    }

    // mark active + reset state
    for (int i = 0; i < count; i++)
    {
        boss9_list[i].active = false;
        boss9_list[i].dx = 0;
        boss9_list[i].dy = 0;
        boss9_list[i].wait = 0;
        boss9_list[i].toMove = 0;
        boss9_list[i].isMoving = false;
		
		boss9_list[i].speed = 2.0f;
		boss9_list[i].angle = 1;
		
		if(global_route==1) boss9_list[i].speed = 6.0f;
    }
}

void SetupBoss10Positions(int count)
{
	float center_x = screen_w-32.0f;
	float center_y = screen_h + ui_offset_y-32.0f;
	
    float margin_x = center_x * 0.30f;
    float margin_y = center_y * 0.30f;

    float left   = margin_x;
    float right  = (center_x * 2.0f) - margin_x;
    float top    = margin_y;
    float bottom = (center_y * 2.0f) - margin_y;
	float dist = 0;

    float cx = center_x;
    float cy = center_y;

    if (count > MAX_BOSS10) count = MAX_BOSS10;
	boss10_count = count;

    memset(boss10_list, 0, sizeof(boss10_list));

    switch (count)
    {
        case 1:
            boss10_list[0].x = cx;
            boss10_list[0].y = cy;
            break;
			
		case 2:
			dist = screen_w * 0.55f;

			boss10_list[0].x = cx - dist;
			boss10_list[0].y = cy;

			boss10_list[1].x = cx + dist;
			boss10_list[1].y = cy;
			break;

        case 3:
            boss10_list[0].x = left;
            boss10_list[0].y = top;

            boss10_list[1].x = right;
            boss10_list[1].y = top;

            boss10_list[2].x = cx;
            boss10_list[2].y = bottom;
            break;

        default: // 4+
            boss10_list[0].x = left;
            boss10_list[0].y = top;

            boss10_list[1].x = right;
            boss10_list[1].y = top;

            boss10_list[2].x = left;
            boss10_list[2].y = bottom;

            boss10_list[3].x = right;
            boss10_list[3].y = bottom;
            break;
    }

    for (int i = 0; i < count; i++)
    {
        boss10_list[i].active = false;
        boss10_list[i].pulse = 0.0f;
        boss10_list[i].time = 10;

        // give each one different movement so they don't mirror perfectly
        // switch (i)
        // {
            // case 0: boss10_list[i].hsp =  2.0f; boss10_list[i].vsp =  1.5f; break;
            // case 1: boss10_list[i].hsp = -2.0f; boss10_list[i].vsp = -1.5f; break;
            // case 2: boss10_list[i].hsp =  1.6f; boss10_list[i].vsp = -2.0f; break;
            // case 3: boss10_list[i].hsp = -1.6f; boss10_list[i].vsp =  2.0f; break;
        // }
    }
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











pngData spritesheet[TOTAL_SHEETS];
u32 sprite_off[TOTAL_SHEETS];

void Load_PNG()
{
	pngLoadFromBuffer(captain_sheet_png_bin, captain_sheet_png_bin_size, &spritesheet[0]);
	pngLoadFromBuffer(tiles_sheet_png_bin, tiles_sheet_png_bin_size, &spritesheet[1]);
	pngLoadFromBuffer(ui_sheet_png_bin, ui_sheet_png_bin_size, &spritesheet[2]);
	pngLoadFromBuffer(boss_sheet_png_bin, boss_sheet_png_bin_size, &spritesheet[3]);
	pngLoadFromBuffer(boss1_sheet_png_bin, boss1_sheet_png_bin_size, &spritesheet[4]);
	pngLoadFromBuffer(dualboss_sheet_png_bin, dualboss_sheet_png_bin_size, &spritesheet[5]);
	pngLoadFromBuffer(cursed_sheet_png_bin, cursed_sheet_png_bin_size, &spritesheet[6]);
	pngLoadFromBuffer(background_sheet_png_bin, background_sheet_png_bin_size, &spritesheet[7]);
}
















void PlayBGM(int id, int inf)
{
    //StopAudio();
    FILE *fp = (FILE *)mem_open((char *)bgm[id], bgm_size[id]);
    if(inf) PlayAudiofd(fp, 0, AUDIO_INFINITE_TIME); else PlayAudiofd(fp, 0, AUDIO_ONE_TIME);

    current_bgm = id;
}

void UpdateBGM(void)
{
	s32 temp_t = GetTimeAudio();
	int tempzz = 50;
	if(temp_t >= bgm_end[current_bgm]-tempzz) SetTimeAudio(bgm_start[current_bgm]-tempzz);
}

void PlaySFX(SoundEffect *sfx)
{
	int voice = SND_GetFirstUnusedVoice();
    if (voice < 1) return;
	
    SND_SetVoice(voice, sfx->stereo ? VOICE_STEREO_16BIT : VOICE_MONO_16BIT,
        sfx->freq, 0, sfx->pcm, sfx->pcm_size, 255, 255, NULL);
}













void DeactivateBosses()
{
	if(boss9_started)
	{
		boss9_started = 0;
		int i;
		for (i = 0; i < boss9_count; i++) boss9_list[i].active = false;
		if(gamewin) PlayBGM(109,0);
	}
	if(boss10_started)
	{
		boss10_started = 0;
		int i;
		for (i = 0; i < boss10_count; i++) boss10_list[i].active = false;
		if(gamewin) PlayBGM(110,0);
	}
}

int explode_time = 0;
int disable_control = 0;

void game_lost()
{
	explode_time = 60;
	disable_control = 1;
	PlaySFX(&sfx_mine);
}

















int FindEpisode2ShapeIndex(int the_id)
{
    int i;
    for (i = 0; i < sizeof(episode2_shapes) / sizeof(episode2_shapes[0]); i++)
    {
        if (episode2_shapes[i].the_id == the_id) return i;
    }
    return -1;
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
	
	if(EPISODE==2)
	{
		int grd_id = FindEpisode2ShapeIndex(GRID_ID);
		for (xx = 0; xx < GRID_W; xx++)
		{
			for (yy = 0; yy < GRID_H; yy++)
			{
				// im gonna set the id 91 for examples
				if(EPISODE==2) board[xx][yy].active = (episode2_shapes[grd_id].shape[yy][xx] == '0');
					else board[xx][yy].active = 1;
				
				if (board[xx][yy].active) active_count++;
			}
		}
		if (MINES >= active_count) MINES = active_count - 1;
		valid_total = active_count - MINES;
	}
	
    int placed = 0;
    while(placed < MINES)
    {
        int x = rand() % GRID_W;
        int y = rand() % GRID_H;
		if (!board[x][y].active) continue;
		if (board[x][y].mine) continue;

		board[x][y].mine = 1;
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

            for(ox=-1;ox<=1;ox++)
            {
                for(oy=-1;oy<=1;oy++)
                {
					int nx, ny;
					
                    nx = x + ox;
                    ny = y + oy;

                    if(nx < 0 || ny < 0 || nx >= GRID_W || ny >= GRID_H) continue;
					
					if (!board[nx][ny].active) continue;
                    if(board[nx][ny].mine) count++;
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
			if(!board[i][j].mine && board[i][j].neighbors < 9)
			{
				score += pow(2, board[i][j].neighbors - 1);
			}
			//if(tile_array[i][j].antimine)score++;  //< we are not ready for this yet.
			//if(tile_array[i][j].doublemine)score++;
		}
	}
	if(current_level==8) score += 999;
	if(current_level==9) score += 1000;
	//if(current_level==9) score += 1111;
	if (boss9_count > 0) score += 9;
	if (boss10_count > 0) score += 10;
	//if (boss11_count > 0) score += 11;
	score += MINES;
	score *= 1.5; // cuz we are practically ALWAYS in master LMAO
	//if (Main.db["master"]): score \*= 1.5
	//score \*= double_multiplier
	//score \*= anti_multiplier
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
	
	if(current_level==8) revealed_total+=9;
	
	if(revealed_total >= valid_total)
	{
		gamewin = 1;
		mines_count = 0;
		timer_running = 0;
		disable_control = 1;
		playing_bgm = false;
		StopAudio();
		
		DeactivateBosses();
		if(current_level<8) PlayBGM(100+desired_bgm, 0);
		
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
	else if(current_level==8) game_lost();
}

int reveal_mode = 0;

void RevealIterative(int x, int y)
{
    int top = 0;

    reveal_stack[top].x = x;
    reveal_stack[top].y = y;
    top++;

    while(top > 0)
    {
        top--;

        x = reveal_stack[top].x;
        y = reveal_stack[top].y;

        if(x < 0 || y < 0 || x >= GRID_W || y >= GRID_H) continue;

        if(!board[x][y].active) continue;

        if(board[x][y].revealed || board[x][y].flagged) continue;

        board[x][y].revealed = 1;

        if(board[x][y].mine)
        {
            game_lost();
            return;
        }

        if(board[x][y].neighbors != 0) continue;

        int ox, oy;

        for(ox = -1; ox <= 1; ox++)
        {
            for(oy = -1; oy <= 1; oy++)
            {
                if(ox == 0 && oy == 0) continue;

                if(top < MAX_W * MAX_H)
                {
                    reveal_stack[top].x = x + ox;
                    reveal_stack[top].y = y + oy;
                    top++;
                }
            }
        }
    }
}

void RevealRecursive(int x,int y)
{
    if(x < 0 || y < 0 || x >= GRID_W || y >= GRID_H) return;
	if (!board[x][y].active) return;
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
            RevealRecursive(x+ox,y+oy);
        }
    }
}

void Reveal(int x,int y)
{
	if(reveal_mode) RevealRecursive(x, y); else RevealIterative(x, y);
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
            if(nx >= 0 && ny >= 0 && nx < GRID_W && ny < GRID_H)
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
                if(nx >= 0 && ny >= 0 && nx < GRID_W && ny < GRID_H)
                {
					if (!board[nx][ny].active) continue;
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

void ResetGrid()
{
    mines_count = MINES;

    cursorX = GRID_W/2;
    cursorY = GRID_H/2;

    FIRST_CLICK = 1;
    gamewin = 0;
    gamelose = 0;
	level_minecoins = 0;
	
	timer_running = 0;
	timer_seconds = 0;
	
	if(current_level==8)
	{
		SetupBoss9Positions(boss9_count);
		msgDialogOpen(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
			"There is a boss battle in this game. The battle works the same as normal Minesweeper, but with some key distinctions:\n"
			"you won't know the number of mines hidden on the grid, you have limited time to complete the level, and you won't be able to use the sonar or pause the game.\n"
			"Clicking on a mine will make the time run out faster. To win, you must click SQUARE ONLY AFTER revealing all the tiles that don't contain mines.\n"
			"If the timer hits 0, or you click on the boss before revealing the entire grid, it's game over.", EmptyDialogCallback, NULL, NULL);
	}
	else if(current_level==9)
	{
		SetupBoss10Positions(boss10_count);
		msgDialogOpen(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
			"There is another boss battle in this game. As soon as the game starts, this boss will start counting down from 10.\n"
			"If it counts all the way down to 0, it's game over. Click SQUARE to reset the countdown back to 10, but be careful.\n"
			"The boss will move faster and faster every time you click. Clicking on a mine will cause the boss to count down more quickly.\n"
			"To win, reveal the entire grid before the boss is able to finish its countdown.", EmptyDialogCallback, NULL, NULL);
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
	
	playing_bgm = true;
    PlayBGM(desired_bgm, 1);
}

void LoadLevel(int level)
{
    GRID_W = levels[level].width;
    GRID_H = levels[level].height;
    MINES  = levels[level].mines;
	GRID_ID= levels[level].id;
    desired_bgm = levels[level].music;
	the_bg = levels[level].back;
	
	if(level==9) EPISODE = 2;
	if(current_level==8) MINES-=8;
	
	ResetGrid();
}

void LoadFPLevel()
{
    GRID_W = fp_width;
    GRID_H = fp_height;
    MINES  = fp_mines;
    desired_bgm = fp_music;
	the_bg = fp_back;
	
	ResetGrid();
}

int getCaptainState(int anim_z, int anim1_z)
{
	int the_state = 0;
	switch(current_level)
	{
		case 0: case 1:
		{
			if(gamewin) the_state = 0+anim_z; else if(gamelose) the_state = 74; else the_state = 24;
			break;
		}
		case 2: case 3:
		{
			if(gamewin) the_state = 4+anim_z; else if(gamelose) the_state = 78; else the_state = 28;
			break;
		}
		case 4: case 5:
		{
			if(gamewin) the_state = 8+anim_z; else if(gamelose) the_state = 82+anim_z; else the_state = 32+anim_z;
			break;
		}
		case 6: case 7:
		{
			if(gamewin) the_state = 16+anim_z; else if(gamelose) the_state = 90; else the_state = 40;
			break;
		}
		case 8:
		{
			if(gamewin) the_state = 96+anim_z; else if(gamelose) the_state = 218; else the_state = 120+anim1_z;
			break;
		}
		case 9:
		{
			if(gamewin) the_state = 102+anim_z; else if(gamelose) the_state = 176; else the_state = 126;
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









void Boss9PlaceTiles(int i)
{
	int x, y;
	
	for(x = boss9_list[i].x - 1; x <= boss9_list[i].x + 1; x++)
    {
        for(y = boss9_list[i].y - 1; y <= boss9_list[i].y + 1; y++)
        {
			board[x][y].mine = 1;
			board[x][y].revealed = false;
        }
    }
}

void Boss9Stop(int i)
{
    boss9_list[i].beforeToMove = 0;
    boss9_list[i].toMove = 0;
    boss9_list[i].isMoving = false;
    boss9_list[i].angle = -boss9_list[i].angle;

    if(global_route==1) boss9_list[i].timer = RandRange(15,50);
    else boss9_list[i].timer = RandRange(20,60);

    PlaySFX(&sfx_9stop);
}

void Boss9SaveStrip(int i)
{
    int sx, sy, i2;

    if(boss9_list[i].dx == 1)
    {
        sx = boss9_list[i].x + 2;

        for(i2=0;i2<3;i2++)
        {
            sy = boss9_list[i].y - 1 + i2;

            boss9_list[i].tempGrid[i2] = board[sx][sy].mine;
            boss9_list[i].tempReveal[i2] = board[sx][sy].revealed;
			boss9_list[i].tempFlagged[i2] = board[sx][sy].flagged;
        }
    }
	else if(boss9_list[i].dx == -1)
    {
        sx = boss9_list[i].x - 2;

        for(i2=0;i2<3;i2++)
        {
            sy = boss9_list[i].y - 1 + i2;

            boss9_list[i].tempGrid[i2] = board[sx][sy].mine;
            boss9_list[i].tempReveal[i2] = board[sx][sy].revealed;
			boss9_list[i].tempFlagged[i2] = board[sx][sy].flagged;
        }
    }
	else if(boss9_list[i].dy == 1)
    {
        sy = boss9_list[i].y + 2;

        for(i2=0;i2<3;i2++)
        {
            sx = boss9_list[i].x - 1 + i2;

            boss9_list[i].tempGrid[i2] = board[sx][sy].mine;
            boss9_list[i].tempReveal[i2] = board[sx][sy].revealed;
			boss9_list[i].tempFlagged[i2] = board[sx][sy].flagged;
        }
    }
	else if(boss9_list[i].dy == -1)
    {
        sy = boss9_list[i].y - 2;

        for(i2=0;i2<3;i2++)
        {
            sx = boss9_list[i].x - 1 + i2;

            boss9_list[i].tempGrid[i2] = board[sx][sy].mine;
            boss9_list[i].tempReveal[i2] = board[sx][sy].revealed;
			boss9_list[i].tempFlagged[i2] = board[sx][sy].flagged;
        }
    }
}

void Boss9RestoreStrip(int i)
{
    int sx, sy, i2;

	if (boss9_list[i].dx == 1)
	{
		sx = boss9_list[i].x - 2;

		for (i2 = 0; i2 < 3; i2++)
		{
			sy = boss9_list[i].y - 1 + i2;

			board[sx][sy].mine = boss9_list[i].tempGrid[i2];
			board[sx][sy].revealed = boss9_list[i].tempReveal[i2];
			board[sx][sy].flagged = boss9_list[i].tempFlagged[i2];
		}
	}
	else if (boss9_list[i].dx == -1)
	{
		sx = boss9_list[i].x + 2;

		for (i2 = 0; i2 < 3; i2++)
		{
			sy = boss9_list[i].y - 1 + i2;

			board[sx][sy].mine = boss9_list[i].tempGrid[i2];
			board[sx][sy].revealed = boss9_list[i].tempReveal[i2];
			board[sx][sy].flagged = boss9_list[i].tempFlagged[i2];
		}
	}
	else if (boss9_list[i].dy == 1)
	{
		sy = boss9_list[i].y - 2;

		for (i2 = 0; i2 < 3; i2++)
		{
			sx = boss9_list[i].x - 1 + i2;

			board[sx][sy].mine = boss9_list[i].tempGrid[i2];
			board[sx][sy].revealed = boss9_list[i].tempReveal[i2];
			board[sx][sy].flagged = boss9_list[i].tempFlagged[i2];
		}
	}
	else if (boss9_list[i].dy == -1)
	{
		sy = boss9_list[i].y + 2;

		for (i2 = 0; i2 < 3; i2++)
		{
			sx = boss9_list[i].x - 1 + i2;

			board[sx][sy].mine = boss9_list[i].tempGrid[i2];
			board[sx][sy].revealed = boss9_list[i].tempReveal[i2];
			board[sx][sy].flagged = boss9_list[i].tempFlagged[i2];
		}
	}
}

bool Boss9CanMove(int nx, int ny)
{
	int x, y;
    if(nx < 1 || nx >= GRID_W - 1) return false;
    if(ny < 1 || ny >= GRID_H - 1) return false;

    for(y=-1;y<=1;y++)
    {
        for(x=-1;x<=1;x++)
        {
            if(x==0 && y==0) continue;
            if(!board[nx+x][ny+y].active) return false;
        }
    }

    return true;
}

void Boss9PickDirection(int i)
{
    boss9_list[i].toMove = rand() % max(GRID_W, GRID_H) + 1;
    boss9_list[i].beforeToMove = boss9_list[i].toMove;

    switch(rand() % 4)
    {
        case 0: boss9_list[i].dx = 1;  boss9_list[i].dy = 0; break;
        case 1: boss9_list[i].dx = -1; boss9_list[i].dy = 0; break;
        case 2: boss9_list[i].dx = 0;  boss9_list[i].dy = 1; break;
        case 3: boss9_list[i].dx = 0;  boss9_list[i].dy = -1; break;
    }

    boss9_list[i].isMoving = true;
}

void UpdateBoss9(int i)
{
    if(!boss9_list[i].active) return;

    if(!boss9_list[i].isMoving) Boss9PickDirection(i);
		else boss9_list[i].toMove--;

    if(boss9_list[i].toMove <= 0)
    {
        Boss9Stop(i);
		return;
    }

    int nx = boss9_list[i].x + boss9_list[i].dx;
    int ny = boss9_list[i].y + boss9_list[i].dy;

    if(!Boss9CanMove(nx, ny))
    {
        Boss9Stop(i);
        return;
    }

    Boss9SaveStrip(i);

    boss9_list[i].x = nx;
    boss9_list[i].y = ny;

    Boss9PlaceTiles(i);

    Boss9RestoreStrip(i);
	CalculateNumbers();

    PlaySFX(&sfx_9move);
}









void boss10_update(int i)
{
    float TMP_SIZ = 3.0f * RENDERING_PIXELS;
    float boss_half = (TMP_SIZ * 3.0f * 1.1f) * 0.5f;
	float boundary = 384.0f / 4;

    float left   = boss_half - boundary;
    float right  = (screen_w*2) - boss_half + boundary - TMP_SIZ;
    float top    = boss_half - boundary;
    float bottom = (screen_h*2) - boss_half + boundary - TMP_SIZ;

    boss10_list[i].x += boss10_list[i].hsp;
    boss10_list[i].y += boss10_list[i].vsp;

    if (boss10_list[i].x < left)
    {
        boss10_list[i].x = left;
        boss10_list[i].hsp = -boss10_list[i].hsp;
		PlaySFX(&sfx_10bump);
    }
    else if (boss10_list[i].x > right)
    {
        boss10_list[i].x = right;
        boss10_list[i].hsp = -boss10_list[i].hsp;
		PlaySFX(&sfx_10bump);
    }
    if (boss10_list[i].y < top)
    {
        boss10_list[i].y = top;
        boss10_list[i].vsp = -boss10_list[i].vsp;
		PlaySFX(&sfx_10bump);
    }
    else if (boss10_list[i].y > bottom)
    {
        boss10_list[i].y = bottom;
        boss10_list[i].vsp = -boss10_list[i].vsp;
		PlaySFX(&sfx_10bump);
    }
}


















int TTFLoadFont(char * path, void * from_memory, int size_from_memory)
{
    if(!ttf_inited) FT_Init_FreeType(&freetype);
    ttf_inited = 1;

    if(path) {if(FT_New_Face(freetype, path, 0, &face)) return -1;}
		else {if(FT_New_Memory_Face(freetype, from_memory, size_from_memory, 0, &face)) return -1;}

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

    for(n = 0; n < slot->bitmap.rows; n++)
	{
        for (m = 0; m < slot->bitmap.width; m++)
		{
            if(m >= (*w) || n >= (*h)) continue;
            bitmap[m] = (u8) slot->bitmap.buffer[ww + m];
        }
		bitmap += *w;
		ww += slot->bitmap.width;
    }

    *w = ((slot->advance.x + 31) >> 6) + ((slot->bitmap_left < 0) ? -slot->bitmap_left : 0);
    *h = slot->bitmap.rows;
}










void DrawFString(float x, float y, u32 textColor, int BW, const char *fmt, ...)
{
    char buffer[1024];
	u32 shadowColor = 0x000000ff;
	
	if(BW) shadowColor = 0xffffffff;
	
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    SetFontColor(shadowColor, 0x00000000);
	DrawFormatString(x + 2, y + 2, buffer);
    SetFontColor(textColor, 0x00000000);
    DrawFormatString(x, y, buffer);
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











void MoveCursor(int dx, int dy)
{
    int nx = cursorX;
    int ny = cursorY;

    do
    {
        nx += dx; ny += dy;

        if (nx < 0 || ny < 0 || nx >= GRID_W || ny >= GRID_H) return;
    }
    while (!board[nx][ny].active);

    cursorX = nx;
    cursorY = ny;
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
	int x1, y1 = 0;
	int x0, y0 = 0;
	
	int tile_bg = the_bg * 20;
	int grid_bg = the_bg;
	int bg_offs = the_bg % 5;
	
	if(the_bg>=5)
	{
		tile_bg = 4+(bg_offs * 20);
		grid_bg = bg_offs+20;
	}
	
	u32 color = 0xffffffff;
	
    static int count_frames = 0;
    static int anim = 0;
	
	count_frames++;
	anim = (count_frames / 30) % 2;
	
	for(x0 = -99; x0 < (screen_w * 2) + 99; x0 += 64)
	{
		for(y0 = -99; y0 < (screen_h * 2) + 99; y0 += 64)
		{
			DrawSprite2D(tile_bg + anim, x0, y0, 64, 64, 16, 16, 320, 320, 5, 0xffffff88);
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
				DrawSprite2D(tile_bg + anim + 2, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
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
				DrawSprite2D(tile_bg + anim, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
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
			
			if(draw_grid_outlines)
			{
				DrawSprite2D(241 + grid_bg, offset_x+(x1*RENDER_SIZE), offset_y+(y1*RENDER_SIZE),
					RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, 2, 0xffffff88);
			}
		}
	}
	
	DrawSprite2D(240, offset_x+(cursorX*RENDER_SIZE), offset_y+(cursorY*RENDER_SIZE),
		RENDER_SIZE, RENDER_SIZE, 16, 16, 320, 320, -4, color);
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
	SetFontSize(28, 32);
	SetTexture(7);
	
	int ui_x = 4;
	
	if(in_game) draw_game();
	
    SetFontColor(0xffffffff, 0x00000000);
    SetCurrentFont(0);
	
	count_frames++;
	
	anim = (count_frames / 30) % 2;
	anim1 = (count_frames / 15) % 2;
	anim2 = (count_frames / 15) % 3;
	anim3 = (count_frames / 15) % 4;
	
	if(!in_game)
	{
		DrawSprite2D(anim3, -40, -22, 916, 582, 1152, 960, 2304, 1920, 0, color);
		if(!free_play)
		{
			//DrawFString(ui_x,256, 0xffffffff, 0, "%d x %d", Video_Resolution.width,Video_Resolution.height);
			DrawFString(ui_x,4, 0xffffffff, 0, "%s (%s)", global_name, global_version);
			DrawFString(ui_x,484, 0xffffffff, 0, "%s", splash);
			DrawFString(ui_x,128, 0xffffffff, 0, "START - New Game");
			DrawFString(ui_x,160, 0xffffffff, 0, "SELECT - Continue");
			DrawFString(ui_x,192, 0xffffffff, 0, "SQUARE - Freeplay");
			DrawFString(ui_x,224, 0xffffffff, 0, "TRIANGLE - Credits");
		}
		else
		{
			DrawFString(ui_x,4, 0xffffffff, 0, "Freeplay Menu");
			DrawFString(ui_x,64, 0xffffffff, 0, "Width: %d", fp_width);
			DrawFString(ui_x,96, 0xffffffff, 0, "Height: %d", fp_height);
			DrawFString(ui_x,128, 0xffffffff, 0, "Mines count: %d", fp_mines);
			DrawFString(ui_x,160, 0xffffffff, 0, "Music Type: %d", fp_music);
			DrawFString(ui_x,192, 0xffffffff, 0, "Background Type: %d", (fp_back+1));
			DrawFString(ui_x,256, 0xffffffff, 0, "START - Start the game");
			DrawFString(ui_x,288, 0xffffffff, 0, "CIRCLE - Go Back");
			
			DrawFString(ui_x+256,64+(fp_menu*32), 0xffffffff, 0, "<");
		}
	}
	else
	{
		int the_timer = (int)timer_seconds;
		if(the_timer>998) the_timer=999;
		
		SetTexture(2);
		SetFontSize(84, 96);
		DrawSpriteUI(-34, -22, 0, 0, 384, 192, 1024, 1024, 302, 112, 1, color);
		DrawSpriteUI(576, -22, 0, 0, 384, 192, 1024, 1024, 302, 112, 1, color);
		DrawSpriteUI(268, -22, 384, 0, 768, 192, 1024, 1024, 308, 112, 1, color);
		DrawSpriteUI(-20, -10, 0+(anim*264), 192, 264+(anim*264), 456, 1024, 1024, 94, 94, 1, color);
		DrawSpriteUI(586, -10, 0+(anim*264), 456, 264+(anim*264), 720, 1024, 1024, 94, 94, 1, color);
		DrawFString(94, -26, 0xff0000ff, 1, "%03d", mines_count);
		DrawFString(714,-26, 0xff0000ff, 1, "%03d", the_timer);
		SetTexture(0);
		SetFontSize(28, 32);
		
		int capt_state = getCaptainState(anim1, anim2);
		
		DrawSprite2D(capt_state, 375, -14, 94, 94, 40, 40, 960, 560, 0, color);
		//DrawFString(-20,32, 0xffffffff, 0, "TOTAL MINECOINS: %d", global_minecoins);
		//DrawFString(-20,64, 0xffffffff, 0, "MINECOINS: %d", current_minecoins);
		DrawFString(-20,88, 0xffffffff, 0, "FPS: %d", fps);
		
		if(global_route==1) SetTexture(4); else SetTexture(3);
		
		if(current_level==8)
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
	
		if(current_level==9)
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
	}
	SetFontSize(28, 32);
	
	if(gamelose)
	{
		DrawFString(-20,160, 0xffffffff, 0, "GAME OVER");
		DrawFString(-20,192, 0xffffffff, 0, "PRESS START");
	}
	else if(gamewin)
	{
		DrawFString(-20,160, 0xffffffff, 0, "LEVEL CLEAR!");
		DrawFString(-20,320, 0xffffffff, 0, "+ %d Minecoins!", level_minecoins);
		if(!free_play)
		{
			DrawFString(-20,192, 0xffffffff, 0, "START - Next Level");
			DrawFString(-20,224, 0xffffffff, 0, "SELECT - Save Progress");
		}
		else
		{
			DrawFString(-20,192, 0xffffffff, 0, "START - Continue");
			DrawFString(-20,224, 0xffffffff, 0, "SELECT - Reset");
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
	
    for(g = 0; g < TOTAL_SHEETS; g++)
	{
		memcpy(texture_pointer, spritesheet[g].bmp_out, spritesheet[g].pitch * spritesheet[g].height);
		sprite_off[g] = tiny3d_TextureOffset(texture_pointer);
		free(spritesheet[g].bmp_out);
		spritesheet[g].bmp_out = NULL;
		
		texture_pointer += ((spritesheet[g].pitch * spritesheet[g].height + 15) & ~15) / 4;
    }
}






int start_music = 0;

void StartMusicCallback(msgButton button, void *usrData)
{
	game_loaded = 1;
	start_music = 1;
}

void GoBackToTitle()
{
	in_game = 0;

	gamewin = 0;
	gamelose = 0;
	current_minecoins = 0;
	EPISODE = 1;
	disable_control = 0;

	splash = splashes_random[rand() % splash_count];
	free_play = 0;
	i_10 = 0;

	PlayBGM(0, 0);
}













int main(int argc, const char* argv[])
{
	padInfo padinfo;
	int dick, ip;
	static padData oldpad;
	padData paddata;
	
	srand(time(NULL));
	tiny3d_Init(1024*1024);
	//tiny3d_Init(2*1024*1024);
	
	sysModuleLoad(SYSMODULE_PNGDEC);
	savestate_init();
	Initialize();
	splash_count = sizeof(splashes_random) / sizeof(splashes_random[0]);
	
	for (dick = 0; dick < rand() % 100; dick++) splash = splashes_random[rand() % splash_count];
	
	boss9_started = 0;
	boss10_started = 0;
	
	init_bgm();
	init_sfx();
	
	ioPadInit(7);
    LoadTexture();
	
	msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
		"Welcome to PS3Sweeper+!\n\nThis game is still under VERY HUGE development, so please join my discord server to not miss out new updates!\nLINK: https://discord.gg/7XuQDYjyzt",
		StartMusicCallback, NULL, NULL);
	
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
		
		ioPadGetInfo(&padinfo);
		for(ip = 0; ip < MAX_PADS; ip++)
		{
			if(padinfo.status[ip])
			{
				ioPadGetData(ip, &paddata);
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
							
							if(paddata.BTN_L2)
							{
								EPISODE = 2;
								if(paddata.BTN_R3) global_route = 1;
								current_level=9;
							}
							
							LoadLevel(current_level);
							in_game = 1;
						}
						if(paddata.BTN_SELECT && !oldpad.BTN_SELECT)
						{
							s32 ret;
							
							drawScene();
							DrawFString(4,256,0xffffffff,0,"Loading...");
							tiny3d_Flip();
							
							ret = sysSaveAutoLoad2(SYS_SAVE_CURRENT_VERSION,SAVE_DIR,
								SYS_SAVE_ERROR_DIALOG_SHOW,&buffer,status_cb,
								load_file_cb,0,NULL);
							
							printf("LOAD RET = %08x\n", ret);
							printf("LEVEL = %d\n", gSave.save_level);
							
							if(ret == SYS_SAVE_RETURN_DONE)
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
						if(paddata.BTN_TRIANGLE && !oldpad.BTN_TRIANGLE)
						{
							msgDialogOpen2(MSG_DIALOG_NORMAL | MSG_DIALOG_BTN_TYPE_OK | MSG_DIALOG_DISABLE_CANCEL_ON,
								"CREDITS\n\nOriginal Minesweeper Plus game made by Jorel Simpson\nXMB Background made by Zeltizy",
								EmptyDialogCallback, NULL, NULL);
						}
						if(paddata.BTN_SQUARE && !oldpad.BTN_SQUARE) free_play = 1;
						if(paddata.BTN_R2 && !oldpad.BTN_R2) HolyShit();
					}
					else
					{
						if(paddata.BTN_START && !oldpad.BTN_START)
						{
							current_level = 0;
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
								case 4: fp_back--; if(fp_back<0) fp_back=0; break;
							}
							
							if(fp_mines>(fp_width*fp_height)-1) fp_mines=(fp_width*fp_height)-1;
						}
						if(paddata.BTN_RIGHT && !oldpad.BTN_RIGHT){
							switch(fp_menu)
							{
								case 0: fp_width++; if(fp_width>48) fp_width=48; break;
								case 1: fp_height++; if(fp_height>36) fp_height=36; break;
								case 2: fp_mines++; break;
								case 3: fp_music++; if(fp_music>8) fp_music=8; break;
								case 4: fp_back++; if(fp_back>10) fp_back=10; break;
							}
							
							if(fp_mines>(fp_width*fp_height)-1) fp_mines=(fp_width*fp_height)-1;
						}
					}
				}
				
				if(gamelose && paddata.BTN_START && !oldpad.BTN_START) GoBackToTitle();
				else if(gamewin)
				{
					if(paddata.BTN_SELECT && !oldpad.BTN_SELECT)
					{
						if(!free_play)
						{
							drawScene();
							DrawFString(4,386,0xffffffff,0,"Saving...");
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
							else LoadLevel(current_level);
						}
						else GoBackToTitle();
					}
				}
				
				if (!disable_control && in_game)
				{					
					// basic
					if(paddata.BTN_CROSS && !oldpad.BTN_CROSS)
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
						
						if(current_level==8 && !boss9_started)
						{
							boss9_started = 1;
							
							int i;
							for (i = 0; i < boss9_count; i++)
							{
								boss9_list[i].active = true;
							}
							
							StopAudio();
							PlayBGM(11, 1);
						}
						if(current_level==9 && !boss10_started)
						{
							boss10_started = 1;
							
							int i;
							for (i = 0; i < boss10_count; i++)
							{
								boss10_list[i].active = true;
							}
							
							StopAudio();
							PlayBGM(13, 1);
						}
						if (board[cursorX][cursorY].revealed) Chord(cursorX, cursorY);
							else Reveal(cursorX, cursorY);
						
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
					if(paddata.BTN_SQUARE && !oldpad.BTN_SQUARE)
					{
						if(current_level==8 && boss9_started) WinCheck();
						else if(current_level==9 && boss10_started)
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
					
					// movement
					if(paddata.BTN_LEFT && !oldpad.BTN_LEFT) MoveCursor(-1, 0);
					if(paddata.BTN_RIGHT && !oldpad.BTN_RIGHT) MoveCursor(1, 0);
					if(paddata.BTN_UP && !oldpad.BTN_UP) MoveCursor(0, -1);
					if(paddata.BTN_DOWN && !oldpad.BTN_DOWN) MoveCursor(0, 1);
					
					// zoom check
					if(paddata.BTN_R1 && !oldpad.BTN_R1) zoomCheck(1);
					if(paddata.BTN_L1 && !oldpad.BTN_L1) zoomCheck(0);
				}
				oldpad = paddata;
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
		
		if(current_level==8)
		{
			int i;
			for (i = 0; i < boss9_count; i++)
			{
				boss9_list[i].rotation += ((float)boss9_list[i].angle * boss9_list[i].speed);
			}
		}
		
		if(timer_running)
		{
			timer_seconds += 0.018f;
			if(timer_seconds > 999) timer_seconds = 999;
		}
		
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
		
		if(disable_control && !gamelose)
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
		
		if(game_loaded) drawScene();
		if(playing_bgm) UpdateBGM();
		
		tiny3d_Flip();
		usleep(3666);
    }

    return 0;
}