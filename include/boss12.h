#ifndef BOSS12_H
#define BOSS12_H

#include "core.h"

#define MAX_BOSS12 4

typedef struct
{
    bool active;
    float x, y;
} Boss12;

extern Boss12 boss12_list[MAX_BOSS12];
void SetupBoss12Positions(int count);
//void boss11_update(int i);
void Deactivate12();

#endif