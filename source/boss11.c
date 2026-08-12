#include "boss11.h"

Boss11 boss11_list[MAX_BOSS11];

void SetupBoss11Positions(int count)
{
    int pad = 1;
    int cx = GRID_W / 2;
    int cy = GRID_H / 2;

    if (count > MAX_BOSS11) count = MAX_BOSS11;
    boss11_count = count;

    memset(boss11_list, 0, sizeof(boss11_list));

    switch (count)
    {
        case 1:
            boss11_list[0].x = cx;
            boss11_list[0].y = cy;
            break;

        case 2:
            boss11_list[0].x = pad;
            boss11_list[0].y = pad;

            boss11_list[1].x = GRID_W - pad;
            boss11_list[1].y = GRID_H - pad;
            break;

        case 3:
            boss11_list[0].x = pad;
            boss11_list[0].y = pad;

            boss11_list[1].x = GRID_W - pad;
            boss11_list[1].y = pad;

            boss11_list[2].x = cx;
            boss11_list[2].y = GRID_H - pad;
            break;

        default: // 4+
            boss11_list[0].x = pad;
            boss11_list[0].y = pad;

            boss11_list[1].x = GRID_W - pad;
            boss11_list[1].y = pad;

            boss11_list[2].x = pad;
            boss11_list[2].y = GRID_H - pad;

            boss11_list[3].x = GRID_W - pad;
            boss11_list[3].y = GRID_H - pad;
            break;
    }

    // mark active + reset state
    for (int i = 0; i < count; i++)
    {
        boss11_list[i].active = false;
		boss11_list[i].placed = 0;
		boss11_list[i].alpha = 255;
    }
}

void boss11_update(int i)
{
	if(boss11_list[i].timer>0)
	{
		boss11_list[i].timer -= 0.031f;
		
		if(boss11_list[i].timer < 3 && boss11_list[i].placed == 0 && !boss11_list[i].bonk)
		{
			PlaySFX(&sfx_11open);
			int _x, _y;
			
			_x = boss11_list[i].x;
			_y = boss11_list[i].y;
			
			boss11_list[i].placed = 1;
			board[_x][_y].flagged = 2;
			mines_count--;
		}
		
		if(boss11_list[i].placed == 1)
		{
			if(boss11_list[i].alpha > 5) boss11_list[i].alpha -= 5;
			else boss11_list[i].alpha = 0;
			if(!boss11_list[i].bonk)
			{
				boss11_list[i].vis_x1 += 0.5f;
				boss11_list[i].vis_x2 += 0.5f;
			}
		}
		else
		{
			if(boss11_list[i].alpha < 250) boss11_list[i].alpha += 5;
				else boss11_list[i].alpha = 255;
		}
	}
	else
	{
		int _x, _y;
		int attempts = 0;
		
		boss11_list[i].x = rand() % GRID_W;
		boss11_list[i].y = rand() % GRID_H;
		boss11_list[i].placed = 0;
		boss11_list[i].bonk = false;
		boss11_list[i].vis_x1 = 0.0f;
		boss11_list[i].vis_x2 = 0.0f;
		//boss11_list[i].alpha = 255;
		
		_x = boss11_list[i].x;
		_y = boss11_list[i].y;
		
		do
		{
			boss11_list[i].x = rand() % GRID_W;
			boss11_list[i].y = rand() % GRID_H;
			
			_x = boss11_list[i].x;
			_y = boss11_list[i].y;
			attempts++;
		}
		while((!board[_x][_y].active || board[_x][_y].revealed || board[_x][_y].flagged>0)
			&& attempts < 200);
		
		if(mines_count<=0) game_lost();
		else if(attempts>199) game_lost();
		
		//boss11_list[i].timer = (float)RandRange(4, 5);
		switch(DIFFICULTY)
		{
			case 0: boss11_list[i].timer = 11.2f; break;
			case 1: boss11_list[i].timer = 9.5f; break;
			case 2: boss11_list[i].timer = 6.7f; break;
			case 3: boss11_list[i].timer = 4.2f; break;
		}
	}
}

void Deactivate11()
{
	int i;
	for (i = 0; i < boss11_count; i++) boss11_list[i].active = false;
}