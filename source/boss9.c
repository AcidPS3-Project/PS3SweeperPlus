#include "boss9.h"

Boss9 boss9_list[MAX_BOSS9];

void SetupBoss9Positions(int count)
{
    int pad = 1;
    int cx = GRID_W / 2;
    int cy = GRID_H / 2;

    if (count > MAX_BOSS9) count = MAX_BOSS9;
    boss9_count = count;

    memset(boss9_list, 0, sizeof(boss9_list));

    switch (count)
    {
        case 1:
            boss9_list[0].x = cx;
            boss9_list[0].y = cy;
            break;

        case 2:
            boss9_list[0].x = pad;
            boss9_list[0].y = pad;

            boss9_list[1].x = GRID_W - 1 - pad;
            boss9_list[1].y = GRID_H - 1 - pad;
            break;

        case 3:
            boss9_list[0].x = pad;
            boss9_list[0].y = pad;

            boss9_list[1].x = GRID_W - 1 - pad;
            boss9_list[1].y = pad;

            boss9_list[2].x = cx;
            boss9_list[2].y = GRID_H - 1 - pad;
            break;

        case 4:
            boss9_list[0].x = pad;
            boss9_list[0].y = pad;

            boss9_list[1].x = GRID_W - 1 - pad;
            boss9_list[1].y = pad;

            boss9_list[2].x = pad;
            boss9_list[2].y = GRID_H - 1 - pad;

            boss9_list[3].x = GRID_W - 1 - pad;
            boss9_list[3].y = GRID_H - 1 - pad;
            break;
		default:
		{
			for (int i = 0; i < count; i++)
			{
				boss9_list[i].x = RandRange(pad, GRID_W - 1 - pad);
				boss9_list[i].y = RandRange(pad, GRID_H - 1 - pad);
			}
			break;
		}
    }

    // mark active + reset state
    for (int i = 0; i < count; i++)
    {
        boss9_list[i].active = false;
        boss9_list[i].dx = 0;
        boss9_list[i].dy = 0;
        boss9_list[i].wait = 0;
        boss9_list[i].toMove = 0;
        boss9_list[i].isMoving = false;
		
		boss9_list[i].speed = 2.0f;
		boss9_list[i].angle = 1;
		
		if(global_route==1) boss9_list[i].speed = 6.0f;
    }
}
bool Boss9Collides(int self, int nx, int ny)
{
    int i;

    for(i = 0; i < MAX_BOSS9; i++)
    {
        if(i == self) continue;
        if(!boss9_list[i].active) continue;

        // 3x3 squares overlap?
        if(abs(nx - boss9_list[i].x) <= 2 && abs(ny - boss9_list[i].y) <= 2)
        {
            return true;
        }
    }
    return false;
}
void Boss9PlaceTiles(int i)
{
	int x, y;
	
	for(x = boss9_list[i].x - 1; x <= boss9_list[i].x + 1; x++)
    {
        for(y = boss9_list[i].y - 1; y <= boss9_list[i].y + 1; y++)
        {
			board[x][y].mine = 1;
			board[x][y].revealed = false;
        }
    }
}
void Boss9Stop(int i)
{
    boss9_list[i].beforeToMove = 0;
    boss9_list[i].toMove = 0;
    boss9_list[i].isMoving = false;
    boss9_list[i].angle = -boss9_list[i].angle;

    if(global_route==1) boss9_list[i].timer = RandRange(14/(1+DIFFICULTY),100/(1+DIFFICULTY));
    else boss9_list[i].timer = RandRange(40/(1+DIFFICULTY),120/(1+DIFFICULTY));

    PlaySFX(&sfx_9stop);
}
void Boss9SaveStrip(int i)
{
    int sx, sy, i2;

    if(boss9_list[i].dx == 1)
    {
        sx = boss9_list[i].x + 2;

        for(i2=0;i2<3;i2++)
        {
            sy = boss9_list[i].y - 1 + i2;

            boss9_list[i].tempGrid[i2] = board[sx][sy].mine;
            boss9_list[i].tempReveal[i2] = board[sx][sy].revealed;
			boss9_list[i].tempFlagged[i2] = board[sx][sy].flagged;
        }
    }
	else if(boss9_list[i].dx == -1)
    {
        sx = boss9_list[i].x - 2;

        for(i2=0;i2<3;i2++)
        {
            sy = boss9_list[i].y - 1 + i2;

            boss9_list[i].tempGrid[i2] = board[sx][sy].mine;
            boss9_list[i].tempReveal[i2] = board[sx][sy].revealed;
			boss9_list[i].tempFlagged[i2] = board[sx][sy].flagged;
        }
    }
	else if(boss9_list[i].dy == 1)
    {
        sy = boss9_list[i].y + 2;

        for(i2=0;i2<3;i2++)
        {
            sx = boss9_list[i].x - 1 + i2;

            boss9_list[i].tempGrid[i2] = board[sx][sy].mine;
            boss9_list[i].tempReveal[i2] = board[sx][sy].revealed;
			boss9_list[i].tempFlagged[i2] = board[sx][sy].flagged;
        }
    }
	else if(boss9_list[i].dy == -1)
    {
        sy = boss9_list[i].y - 2;

        for(i2=0;i2<3;i2++)
        {
            sx = boss9_list[i].x - 1 + i2;

            boss9_list[i].tempGrid[i2] = board[sx][sy].mine;
            boss9_list[i].tempReveal[i2] = board[sx][sy].revealed;
			boss9_list[i].tempFlagged[i2] = board[sx][sy].flagged;
        }
    }
}
void Boss9RestoreStrip(int i)
{
    int sx, sy, i2;

	if (boss9_list[i].dx == 1)
	{
		sx = boss9_list[i].x - 2;

		for (i2 = 0; i2 < 3; i2++)
		{
			sy = boss9_list[i].y - 1 + i2;

			board[sx][sy].mine = boss9_list[i].tempGrid[i2];
			board[sx][sy].revealed = boss9_list[i].tempReveal[i2];
			board[sx][sy].flagged = boss9_list[i].tempFlagged[i2];
		}
	}
	else if (boss9_list[i].dx == -1)
	{
		sx = boss9_list[i].x + 2;

		for (i2 = 0; i2 < 3; i2++)
		{
			sy = boss9_list[i].y - 1 + i2;

			board[sx][sy].mine = boss9_list[i].tempGrid[i2];
			board[sx][sy].revealed = boss9_list[i].tempReveal[i2];
			board[sx][sy].flagged = boss9_list[i].tempFlagged[i2];
		}
	}
	else if (boss9_list[i].dy == 1)
	{
		sy = boss9_list[i].y - 2;

		for (i2 = 0; i2 < 3; i2++)
		{
			sx = boss9_list[i].x - 1 + i2;

			board[sx][sy].mine = boss9_list[i].tempGrid[i2];
			board[sx][sy].revealed = boss9_list[i].tempReveal[i2];
			board[sx][sy].flagged = boss9_list[i].tempFlagged[i2];
		}
	}
	else if (boss9_list[i].dy == -1)
	{
		sy = boss9_list[i].y + 2;

		for (i2 = 0; i2 < 3; i2++)
		{
			sx = boss9_list[i].x - 1 + i2;

			board[sx][sy].mine = boss9_list[i].tempGrid[i2];
			board[sx][sy].revealed = boss9_list[i].tempReveal[i2];
			board[sx][sy].flagged = boss9_list[i].tempFlagged[i2];
		}
	}
}
bool Boss9CanMove(int nx, int ny)
{
	int x, y;
    if(nx < 1 || nx >= GRID_W - 1) return false;
    if(ny < 1 || ny >= GRID_H - 1) return false;

    for(y=-1;y<=1;y++)
    {
        for(x=-1;x<=1;x++)
        {
            if(x==0 && y==0) continue;
            if(!board[nx+x][ny+y].active) return false;
        }
    }

    return true;
}
void Boss9PickDirection(int i)
{
    boss9_list[i].toMove = rand() % max(GRID_W, GRID_H) + 1;
    boss9_list[i].beforeToMove = boss9_list[i].toMove;

    switch(rand() % 4)
    {
        case 0: boss9_list[i].dx = 1;  boss9_list[i].dy = 0; break;
        case 1: boss9_list[i].dx = -1; boss9_list[i].dy = 0; break;
        case 2: boss9_list[i].dx = 0;  boss9_list[i].dy = 1; break;
        case 3: boss9_list[i].dx = 0;  boss9_list[i].dy = -1; break;
    }

    boss9_list[i].isMoving = true;
}
void UpdateBoss9(int i)
{
    if(!boss9_list[i].active) return;

    if(!boss9_list[i].isMoving) Boss9PickDirection(i);
		else boss9_list[i].toMove--;

    if(boss9_list[i].toMove <= 0)
    {
        Boss9Stop(i);
		return;
    }

    int nx = boss9_list[i].x + boss9_list[i].dx;
    int ny = boss9_list[i].y + boss9_list[i].dy;

    if(!Boss9CanMove(nx, ny) || Boss9Collides(i, nx, ny))
    {
        Boss9Stop(i);
        return;
    }

    Boss9SaveStrip(i);

    boss9_list[i].x = nx;
    boss9_list[i].y = ny;

    Boss9PlaceTiles(i);

    Boss9RestoreStrip(i);
	CalculateNumbers();

    PlaySFX(&sfx_9move);
}

void Deactivate9()
{
	int i;
	for (i = 0; i < boss9_count; i++) boss9_list[i].active = false;
}