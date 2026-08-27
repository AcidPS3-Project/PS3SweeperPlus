#ifndef BOSS12_H
#define BOSS12_H

#include "core.h"

#define MAX_BOSS12 1

typedef struct
{
    bool active;
    float x, y;
    float hsp, vsp;
    float rotation;
    int mistake;
    int tempo;
} Boss12;

extern Boss12 boss12;

int can_move_xd;

void SpawnProjectile(float x, float y, float vx, float vy, int type, float lifetime);

void SetupBoss12Position();
void Deactivate12();
void boss12_update();

#endif