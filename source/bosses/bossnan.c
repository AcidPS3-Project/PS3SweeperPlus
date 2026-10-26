#include "bossnan.h"

BossNaN bossnan_list[MAX_BOSSNAN];

void SetupBossNaNPositions(int count)
{
    int pad = 1;
    int cx = GRID_W / 2;
    int cy = GRID_H / 2;

    if (count > MAX_BOSSNAN) count = MAX_BOSSNAN;
    bossnan_count = count;

    memset(bossnan_list, 0, sizeof(bossnan_list));

    switch (count)
    {
        case 1:
            bossnan_list[0].x = cx;
            bossnan_list[0].y = cy;
            break;

        case 2:
            bossnan_list[0].x = pad;
            bossnan_list[0].y = pad;

            bossnan_list[1].x = GRID_W - 1 - pad;
            bossnan_list[1].y = GRID_H - 1 - pad;
            break;

        case 3:
            bossnan_list[0].x = pad;
            bossnan_list[0].y = pad;

            bossnan_list[1].x = GRID_W - 1 - pad;
            bossnan_list[1].y = pad;

            bossnan_list[2].x = cx;
            bossnan_list[2].y = GRID_H - 1 - pad;
            break;

        case 4:
            bossnan_list[0].x = pad;
            bossnan_list[0].y = pad;

            bossnan_list[1].x = GRID_W - 1 - pad;
            bossnan_list[1].y = pad;

            bossnan_list[2].x = pad;
            bossnan_list[2].y = GRID_H - 1 - pad;

            bossnan_list[3].x = GRID_W - 1 - pad;
            bossnan_list[3].y = GRID_H - 1 - pad;
            break;
		default:
		{
			for (int i = 0; i < count; i++)
			{
				bossnan_list[i].x = RandRange(pad, GRID_W - 1 - pad);
				bossnan_list[i].y = RandRange(pad, GRID_H - 1 - pad);
			}
			break;
		}
    }

    // mark active + reset state
    for (int i = 0; i < count; i++)
    {
        bossnan_list[i].active = false;
        bossnan_list[i].pos_x = 0.0f;
        bossnan_list[i].pos_y = 0.0f;
    }
}

void DeactivateNaN()
{
    for (int i = 0; i < MAX_BOSSNAN; i++)
    {
        bossnan_list[i].active = false;
    }
}

void bossnan_update()
{
	// hahhahaha
}