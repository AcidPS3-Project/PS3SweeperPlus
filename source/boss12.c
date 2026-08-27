#include "boss12.h"

Boss12 boss12;

void SetupBoss12Position()
{
	float center_x = screen_w-32.0f;
	float center_y = screen_h + ui_offset_y-32.0f;
	
    float cx = center_x;
    float cy = center_y;

	boss12.x = cx;
	boss12.y = cy;
	boss12.hsp = 1.0f;
	boss12.vsp = 1.0f;
	boss12.active = false;
	boss12.mistake = 0;
	boss12.tempo = 0;
	boss12.rotation = 1;
}

void Deactivate12()
{
	boss12.active = false;
}

void boss12_update()
{
	static int tempoz = 0;
	
    float TMP_SIZ = 3.0f * RENDERING_PIXELS;
    float boss_half = (TMP_SIZ * 3.0f * 1.1f) * 0.5f;
	float boundary = 384.0f / 4;

    float left   = boss_half - boundary;
    float right  = (screen_w*2) - boss_half + boundary - TMP_SIZ;
    float top    = boss_half - boundary;
    float bottom = (screen_h*2) - boss_half + boundary - TMP_SIZ;
	
	if(boss12.tempo>(500-boss12.mistake))
	{
		can_move_xd = 0;
		tempoz++;
		if(tempoz>100)
		{
			tempoz=0;
			boss12.tempo=0;
			can_move_xd = 1;
			
			SpawnProjectile(boss12.x, boss12.y, 10.0f, 0, 1, 10.0f);
			SpawnProjectile(boss12.x, boss12.y, 6.67f, 3.33f, 1, 10.0f);
			SpawnProjectile(boss12.x, boss12.y, 3.33f, 6.67f, 1, 10.0f);
			SpawnProjectile(boss12.x, boss12.y, 0, 10.0f, 1, 10.0f);
			SpawnProjectile(boss12.x, boss12.y, -3.33f, 6.67f, 1, 10.0f);
			SpawnProjectile(boss12.x, boss12.y, -6.67f, 3.33f, 1, 10.0f);
			SpawnProjectile(boss12.x, boss12.y, -10.0f, 0, 1, 10.0f);
			SpawnProjectile(boss12.x, boss12.y, -6.67f, -3.33f, 1, 10.0f);
			SpawnProjectile(boss12.x, boss12.y, -3.33f, -6.67f, 1, 10.0f);
			SpawnProjectile(boss12.x, boss12.y, 0, -10.0f, 1, 10.0f);
			SpawnProjectile(boss12.x, boss12.y, 3.33f, -6.67f, 1, 10.0f);
			SpawnProjectile(boss12.x, boss12.y, 6.67f, -3.33f, 1, 10.0f);
		}
	}
	else
	{
		boss12.tempo++;
		boss12.rotation += ((float)boss12.tempo/50);
		
		boss12.x += boss12.hsp * ((float)boss12.tempo/50);
		boss12.y += boss12.vsp * ((float)boss12.tempo/50);

		if (boss12.x < left)
		{
			boss12.x = left;
			boss12.hsp = -boss12.hsp;
		}
		else if (boss12.x > right)
		{
			boss12.x = right;
			boss12.hsp = -boss12.hsp;
		}
		if (boss12.y < top)
		{
			boss12.y = top;
			boss12.vsp = -boss12.vsp;
		}
		else if (boss12.y > bottom)
		{
			boss12.y = bottom;
			boss12.vsp = -boss12.vsp;
		}
	}
}