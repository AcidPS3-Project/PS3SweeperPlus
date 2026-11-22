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
    for (int i = 0; i < MAX_BOSSNAN; i++)
    {
		int zx = bossnan_list[i].x;
		int zy = bossnan_list[i].y;
		float fx = bossnan_list[i].pos_x;
		float fy = bossnan_list[i].pos_y;
		
		float distance_x = fabs((float)(fx-zx));
		float distance_y = fabs((float)(fy-zy));
		
		if(distance_x>0.1f || distance_y>0.1f)
		{
			int mp = local_mistakes+1;
			fx = flerp(fx, zx, 0.035f * mp);
			fy = flerp(fy, zy, 0.035f * mp);
			bossnan_list[i].pos_x = (float)fx;
			bossnan_list[i].pos_y = (float)fy;
		}
		else
		{
			bossnan_list[i].pos_x = (float)zx;
			bossnan_list[i].pos_y = (float)zy;
			bossnan_list[i].x = RandRange(1, GRID_W - 2);
			bossnan_list[i].y = RandRange(1, GRID_H - 2);
			board[zx][zy].revealed = 0;
			board[zx][zy].corrupted = true;
			
			int xx, yy;
			int corrupted_count = 0;
			int total_count = 0;
			for (xx = 0; xx < GRID_W; xx++)
			{
				for (yy = 0; yy < GRID_H; yy++)
				{
					if(board[xx][yy].corrupted) corrupted_count++;
					if(board[xx][yy].active) total_count++;
				}
			}
			
			corruption = (float)corrupted_count / total_count;
			corruption *= 100;
			
			board[zx][zy].active = 0;
			CalculateNumbers();
			DebugLog("NaN Position change\n");
		}
    }
}