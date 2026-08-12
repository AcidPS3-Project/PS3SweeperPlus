#ifndef IMPORTER_H
#define IMPORTER_H

#include "libraries.h"

extern char global_name[];
extern char global_version[];

const char *langStr[1024];

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
#include "menu_sheet_png_bin.h"
#include "coin_png_bin.h"
#include "buttons_png_bin.h"
#include "hats_sheet_png_bin.h"

#include "trust_me_bro_png_bin.h"
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

#include "bgmrs_mp3_bin.h"

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
#include "boss11_intro_mp3_bin.h"
#include "boss11_mp3_bin.h"
#include "boss11_2_mp3_bin.h"
#include "boss11_win_ogg_bin.h"

#include "Ags1_mp3_bin.h"
#include "Ags2_mp3_bin.h"
#include "Ags3_mp3_bin.h"
#include "Ags4_mp3_bin.h"
#include "Ags5_mp3_bin.h"
#include "Ags6_mp3_bin.h"
#include "Ags7_mp3_bin.h"
#include "Ags8_mp3_bin.h"
#include "Ags9_mp3_bin.h"
#include "Ags10_mp3_bin.h"
#include "Ags11_mp3_bin.h"
#include "Agsrs_mp3_bin.h"

//IMPORT SFX
#include "flag_ogg_bin.h"
#include "unflag_mp3_bin.h"
#include "nine_move_ogg_bin.h"
#include "nine_stop_ogg_bin.h"
#include "ten_bump_ogg_bin.h"
#include "ten_reset_ogg_bin.h"
#include "game_mine_ogg_bin.h"
#include "game_explosion_ogg_bin.h"
#include "eleven_open_ogg_bin.h"
#include "eleven_bonk_ogg_bin.h"

extern int current_bgm;
extern int desired_bgm;

extern int global_minecoins;
extern int current_minecoins;
extern int level_minecoins;
extern float RENDER_SIZE;

extern int RENDERING_PIXELS;
extern float RENDERING_SIZE;
extern int FIRST_CLICK;

extern int i_10;
extern int language;

extern float timer_seconds;
extern int timer_running;

extern int screen_w;
extern int screen_h;

extern int offset_x;
extern int offset_y;
extern int ui_offset_y;

extern int bad_apple;
extern int temp_fp_music;

extern bool playing_bgm;
extern bool custom_music;

extern bool has_custom_main;
extern bool has_custom_win;

extern int EPISODE;
extern int LEVEL;
extern int DIFFICULTY;
extern int DIFFICULTYs;

extern int the_bg;

extern int ttf_inited;
extern int game_loaded;
extern int free_play;
extern int draw_grid_outlines;

extern int explode_time;
extern int disable_control;
extern int reveal_mode;
extern int start_music;
extern int game_init;

extern float ze_x;
extern float ze_y;

const char *splashes_random[1024];
const char *splash;

void SplashesInit();
int SplashesCount();

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
SoundEffect sfx_11open;
SoundEffect sfx_11bonk;

#define BGM_LIST 200

#define TOTAL_SHEETS 20
pngData spritesheet[TOTAL_SHEETS];
u32 sprite_off[TOTAL_SHEETS];

const unsigned char *bgm[BGM_LIST];
int bgm_size[BGM_LIST];
int bgm_start[BGM_LIST];
int bgm_end[BGM_LIST];

int LoadSFX();
void init_sfx();
void init_bgm();
void LangInit();

int RandRange(int min, int max);
int max(int min, int max);

void PlayBGM(int id, int inf);
void UpdateBGM(void);
void PlaySFX(SoundEffect *sfx);

void Load_PNG();

void ReplaceBGM(int music);
void UpdateFPS();
extern int fps;
bool FindMusic(const char *base, char *out_path);

void DemoCompleteCallback(msgButton button, void *usrData);
void EmptyDialogCallback(msgButton button, void *usrData);
void Initialize();
void HolyShit();

extern float menu_x;
extern int menu_pos;

float lerp(int x, int y, float amount);

void DebugLog(const char *text);

const char *menu_title[10];
const char *menu_desc[10];

#endif