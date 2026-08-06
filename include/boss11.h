#ifndef BOSS11_H
#define BOSS11_H

#include "core.h"

#define MAX_BOSS11 4

typedef struct
{
    bool active, bonk;
    float x, y, timer;
	int flags, rushing;
	int placed, alpha;
	float vis_x1, vis_x2;
} Boss11;

extern Boss11 boss11_list[MAX_BOSS11];
void SetupBoss11Positions(int count);
void boss11_update(int i);
void Deactivate11();

#endif