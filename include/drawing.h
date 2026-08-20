#ifndef DRAWING_H
#define DRAWING_H

#include "libraries.h"

typedef enum GameScreen
{
	LOGO,
	TITLE,
	MENU,
	MAP,
	GAME,
	ENDING
}
GameScreen;

extern GameScreen currentScreen;
extern int vdec_picout_pending;

int getCaptainState(int anim_z, int anim1_z);
int getHatState(int hat, int anim_z);
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

#endif