#ifndef DRAWING_H
#define DRAWING_H

#include "libraries.h"
#define MAXLVLTEST 20
float level_time[MAXLVLTEST];

typedef struct
{
    float x, y, z;
    u32 color;
    float u, v;
} RSXVertex;

typedef struct
{
    int active;

    float x;
    float y;

    float vx;
    float vy;

    float lifetime;

    int type;
    float anim;
} Projectile;

#define MAX_PROJECTILES 64

#define PAD_TRIANGLE 0
#define PAD_CIRCLE 1
#define PAD_CROSS 2
#define PAD_SQUARE 3
#define PAD_START 4
#define PAD_SELECT 5
#define PAD_L1 6
#define PAD_R1 7
#define PAD_L2 8
#define PAD_R2 9
#define PAD_L3 10
#define PAD_R3 11

Projectile projectiles[MAX_PROJECTILES];

void UpdateProjectiles(float dt);
void SpawnProjectile(float x, float y, float vx, float vy, int type, float lifetime);

typedef enum GameScreen
{
	LOGO,
	TITLE,
	MENU,
	MAP,
	GAME,
	ENDING,
	GOTOSHOP,
	SHOP,
	CREDITS,
	ERROR_SCREEN,
	ENDING2,
	ROUTE_SELECT,
	VIDEOTEST
}
GameScreen;

extern GameScreen currentScreen;
extern volatile int vdec_picout_pending;

typedef struct
{
    float angle;
    float speed;
    u32 color;
} BossTriangle;

BossTriangle boss_triangles[3];

typedef struct
{
    int x;
    int y;
} LevelSelectPos;

static const LevelSelectPos level_positions[9];
static const LevelSelectPos level_positions2[10];

extern int end_time;

extern u32 *texture_pointer;

int getCaptainState(int anim_z, int anim1_z, int anim2_z);
int getHatState(int hat, int anim_z);
int getShipRot(int rot);

void draw_game();
void drawScene();
void LoadTexture();
int RandRange();
void DrawFString(int wrap, float x, float y, u32 textColor, int BW, int size, const char *fmt, ...);
void zoomCheck(int zoom);
void DrawTextAligned(int wrap, float x1, float x2, float y, int align, u32 tc, int bw, int fsize, int size, const char *text, ...);

int tile_map_num(int _t);

void FadeInOut(GameScreen *variable, GameScreen value);
void UpdateFade(float dt);

int can_move_xd;
int disable_control;

#endif