#ifndef DRAWING_H
#define DRAWING_H

#include "libraries.h"

typedef enum GameScreen
{
	TITLE,
	MENU,
	MAP,
	GAME,
	ENDING
}
GameScreen;

extern GameScreen currentScreen;

int getCaptainState(int anim_z, int anim1_z);
void DrawSprite2D(int id, float x, float y, int wid2, int hei2, int wid1, int hei1, int wid0, int hei0, float layer, u32 color);
void DrawSpriteUI(float x, float y, int wid1, int hei1, int wid2, int hei2, int wid0, int hei0, int wid3, int hei3, float layer, u32 color);
void DrawSprite2DR(int id, float x, float y, int wid2, int hei2, int wid1, int hei1, int wid0, int hei0, float angle, float layer,  u32 color);
void SetTexture(int id);
void draw_game();
void drawScene();
void LoadTexture();
void DrawFString(float x, float y, u32 textColor, int BW, const char *fmt, ...);
void zoomCheck(int zoom);

#endif