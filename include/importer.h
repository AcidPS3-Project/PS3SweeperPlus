#ifndef IMPORTER_H
#define IMPORTER_H

#include <pngdec/pngdec.h>
#include <sysutil/msg.h>

extern char global_name[];
extern char global_version[];

const char *langStr[1024];
const char *credits[256];

extern int current_bgm;
extern int desired_bgm;

extern int smiley_face;

extern float ship_x;
extern float ship_y;
extern int ship_r;
extern bool ship_a;

extern float time_total;
extern int ten_arena;

extern bool show_shit;
extern int rank_result;

extern bool easter_egg;

extern int global_minecoins;
extern int current_minecoins;
extern int level_minecoins;
extern float RENDER_SIZE;

extern int RENDERING_PIXELS;
extern float RENDERING_SIZE;
extern int FIRST_CLICK;

extern int i_10;
extern int language;

extern bool specialthanks;

extern bool paused;
extern int mistakes;

extern float timer_seconds;
extern int timer_running;

#define MAXLVLTEST 20
extern float level_time[MAXLVLTEST];

extern bool button_x_hold;
extern bool button_start_hold;

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
extern int EPISODEs;
extern int LEVEL;
extern int DIFFICULTY;
extern int DIFFICULTYs;
extern int LIFES;

extern int the_bg;

extern int game_loaded;
extern int free_play;
extern int draw_grid_outlines;

extern int explode_time;
extern int disable_control;
extern int reveal_mode;
extern int start_music;
extern int game_init;

extern bool use_custom_sweeper;
extern bool shop_unlocked;
extern bool shop_first;
extern bool first_playing;

extern float ze_x;
extern float ze_y;

const char *splashes_random[1024];
const char *splash;

extern bool shop_unlock_0;
extern bool shop_unlock_1;
extern bool shop_unlock_2;
extern bool shop_unlock_3;
extern bool shop_unlock_4;
extern bool shop_unlock_5;

extern int ranks_ep_1[8];
extern int ranks_ep_2[8];
extern int ranks_ep_3[8];
extern int ranks_ep_4[8];

extern bool route_touhou_unlocked;

void SplashesInit();
int SplashesCount();

typedef struct
{
    short *pcm;
    int pcm_size;
    int freq;
    int stereo;
    int voice;
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
SoundEffect sfx_buy;
SoundEffect sfx_gamestart;
SoundEffect sfx_level;
SoundEffect sfx_level2;
SoundEffect sfx_beep;
SoundEffect sfx_blackout;

#define BGM_LIST 400
#define MUS_DIR "/dev_hdd0/game/MINESWEEP/USRDIR/music/"

#define TOTAL_SHEETS 25
pngData spritesheet[TOTAL_SHEETS];
u32 sprite_off[TOTAL_SHEETS];

const char *bgm[BGM_LIST];
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
void creditsInit();

void SetupTimeSettings();

extern float menu_x;
extern int menu_pos;

extern int setting_guarantee;

float lerp(int x, int y, float amount);

void DebugLog(const char *text);

int LoadImageA(const char *path, pngData *texture);

const char *menu_title[10];
const char *menu_desc[10];

#endif