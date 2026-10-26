#ifndef BOSSNAN_H
#define BOSSNAN_H

#include "core.h"

#define MAX_BOSSNAN 1

typedef struct
{
    bool active;
    int x, y;
    float pos_x, pos_y;
} BossNaN;

extern BossNaN bossnan_list[MAX_BOSSNAN];

void SetupBossNaNPositions(int count);
void DeactivateNaN();
void bossnan_update();

#endif