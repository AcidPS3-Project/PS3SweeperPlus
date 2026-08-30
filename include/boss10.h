#ifndef BOSS10_H
#define BOSS10_H

#include "core.h"

#define MAX_BOSS10 4

typedef struct
{
    bool active;
    float x, y;
    float hsp, vsp;
	float tspeed, beep_timer;
    int time;
	float speed, timer, pulse;
} Boss10;

extern Boss10 boss10_list[MAX_BOSS10];
void SetupBoss10Positions(int count);
void boss10_update(int i);
void Deactivate10();

#endif