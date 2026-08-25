#ifndef CORE_H
#define CORE_H

#include "libraries.h"
#include "importer.h"

#define MAX_W 72
#define MAX_H 48

typedef struct
{
    int x;
    int y;
} RevealNode;

typedef struct
{
	u8 mine;
	u8 revealed;
	s8 flagged;
	int neighbors;
	bool hasNeighborMines;
	u8 doublemine;
	u8 halfmine;
	u8 antimine;
	u8 active;
	u8 processed;
	
    u8 wall_up;
    u8 wall_down;
    u8 wall_left;
    u8 wall_right;
} Cell; //heh, funny

extern RevealNode reveal_queue[MAX_W * MAX_H];
extern Cell board[MAX_W][MAX_H];

bool CanMoveBetween(int x, int y, int nx, int ny);
bool CanChordBetween(int x, int y, int nx, int ny);
void LoadBoardWalls(int grd_id);

extern int GRID_W;
extern int GRID_H;
extern int MINES;
extern int GRID_ID;
extern int mines_count;
extern int global_route;

extern int enable_half;
extern int enable_double;
extern int enable_anti;

extern int fp_width;
extern int fp_height;
extern int fp_mines;
extern int fp_doubles;
extern int fp_halfes;
extern int fp_antis;
extern int fp_music;
extern int fp_back;
extern int fp_menu;

extern int cursorX;
extern int cursorY;
extern int gamewin;
extern int gamelose;
extern int can_move_xd;

extern int boss9_started;
extern int boss10_started;
extern int boss11_started;
extern int boss12_started;
extern int bossnan_started;

extern int boss9_cutscene;
extern int boss10_cutscene;
extern int boss11_cutscene;
extern int boss12_cutscene;
extern int bossnan_cutscene;

extern int boss9_count;
extern int boss10_count;
extern int boss11_count;
extern int boss12_count;
extern int bossnan_count;

void game_lost();
void GenerateBoard();
void CalculateNumbers();
int calculate_score();
void WinCheck();
void RevealIterative(int startx, int starty);
void RevealRecursive(int x,int y);
void Reveal(int x,int y);
void Chord(int x, int y);
int FlagValue(int flag);
void DeactivateBosses();

void Deactivate9(void);
void Deactivate10(void);
void Deactivate11(void);
void Deactivate12(void);
void DeactivateNaN(void);

extern int adventure_select;
extern int challenge_select;
extern int loadout_select;
extern int settings_select;

void badAppleUpdate();
bool bad_apple_init();
void GoBackToTitle();

void LoadLevel(int levelz);
void LoadFPLevel();
void MoveCursor(int dx, int dy);
void ResetGrid();

int StartSave();

extern int the_hat;

#endif