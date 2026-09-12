#ifndef BOSS9_H
#define BOSS9_H

#include "core.h"

#define MAX_BOSS9 4

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
    u8 tempFlagged[3];
	int timer;
} Boss9;

extern Boss9 boss9_list[MAX_BOSS9];
void SetupBoss9Positions(int count);
void UpdateBoss9(int i);
void Deactivate9();

void UpdateFrozenNine();

#endif