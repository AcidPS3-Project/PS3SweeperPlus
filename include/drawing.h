#ifndef DRAWING_H
#define DRAWING_H

#include "libraries.h"
#define MAXLVLTEST 20
float level_time[MAXLVLTEST];

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
	CREDITS
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

void DrawRectangle2D(float x, float y, float w, float h, u32 rgba, float l);

int getCaptainState(int anim_z, int anim1_z, int anim2_z);
int getHatState(int hat, int anim_z);
int getShipRot(int rot);

void DrawSprite2D(int id, float x, float y, int wid2, int hei2, int wid1, int hei1, int wid0, int hei0, float layer, u32 color);
void DrawSpriteUI(float x, float y, int wid1, int hei1, int wid2, int hei2, int wid0, int hei0, int wid3, int hei3, float layer, u32 color);
void DrawSprite2DR(int id, float x, float y, int wid2, int hei2, int wid1, int hei1, int wid0, int hei0, float angle, float layer,  u32 color);
void SetTexture(int id);
void draw_game();
void drawScene();
void LoadTexture();
int RandRange();
void DrawFString(int wrap, float x, float y, u32 textColor, int BW, const char *fmt, ...);
void zoomCheck(int zoom);
void DrawTextAligned(int wrap, float x1, float x2, float y, int align, u32 tc, int bw, int fsize, const char *text, ...);

int tile_map_num(int _t);

void FadeInOut(GameScreen *variable, GameScreen value);
void UpdateFade(float dt);

int can_move_xd;
int disable_control;

#endif