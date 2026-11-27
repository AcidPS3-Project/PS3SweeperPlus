#include "boss12.h"

Boss12 boss12;

void SetupBoss12Position()
{
	float center_x = screen_w-32.0f;
	float center_y = screen_h + ui_offset_y-32.0f;
	
    float cx = center_x;
    float cy = center_y;
	
	if(free_play)
	{
		float TMP_SIZ = 3.0f * RENDERING_PIXELS;
		float boss_half = (TMP_SIZ * 3.0f * 1.1f) * 0.5f;
		float boundary = 384.0f / 4;
		
		float leftz   = boss_half - boundary;
		float rightz  = (screen_w*2) - boss_half + boundary - TMP_SIZ;
		float topz    = boss_half - boundary;
		float bottomz = (screen_h*2) - boss_half + boundary - TMP_SIZ;

		boss12.x = RandRange(leftz, rightz);
		boss12.y = RandRange(topz, bottomz);
	}
	else
	{
		boss12.x = cx;
		boss12.y = cy;
	}

	boss12.hsp = 1.0f;
	boss12.vsp = 1.0f;
	boss12.active = false;
	boss12.mistake = 0;
	boss12.tempo = 0;
	boss12.rotation = 1;
}

bool lost = false;

void Deactivate12()
{
	lost = false;
	boss12.active = false;
}

void boss12_update(int mute, int local_mistakes)
{
	static int tempoz = 0;
	
    float TMP_SIZ = 3.0f * RENDERING_PIXELS;
    float boss_half = (TMP_SIZ * 3.0f * 1.1f) * 0.5f;
	float boundary = 384.0f / 4;

    float left   = boss_half - boundary;
    float right  = (screen_w*2) - boss_half + boundary - TMP_SIZ;
    float top    = boss_half - boundary;
    float bottom = (screen_h*2) - boss_half + boundary - TMP_SIZ;
	
	if(boss12.tempo>(500-(local_mistakes*20)))
	{
		can_move_xd = 0;
		tempoz++;
		if(tempoz>100)
		{
			if(local_mistakes>=15 && !lost)
			{
				can_move_xd = 0;
				lost = true;
				if(free_play) fp_lifes=0; else LIFES=0;
				game_lost();
			}
			else
			{
				tempoz=0;
				boss12.tempo=0;
				can_move_xd = 1;
				
				if(global_route==3)
				{
					SpawnProjectile(boss12.x, boss12.y, 10.0f, 0, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, 7.5f, 2.5f, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, 5.0f, 5.0f, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, 2.5f, 7.5f, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, 0, 10.0f, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, -2.5f, 7.5f, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, -5.0f, 5.0f, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, -7.5f, 2.5f, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, -10.0f, 0, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, -7.5f, -2.5f, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, -5.0f, -5.0f, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, -2.5f, -7.5f, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, 0, -10.0f, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, 2.5f, -7.5f, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, 5.0f, -5.0f, 1, 10.0f);
					SpawnProjectile(boss12.x, boss12.y, 7.5f, -2.5f, 1, 10.0f);
				}
				else
				{
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
		}
	}
	else
	{
		if(!mute) boss12.tempo++;
		
		boss12.rotation += ((float)boss12.tempo/50);
		
		float tester = 1.0f;
		if(global_route==3)
		{
			tester = 2.0f;
			if((boss12.tempo % 120)==26)
			{
				SpawnProjectile(boss12.x, boss12.y, 10.0f, 0, 1, 10.0f);
				//SpawnProjectile(boss12.x, boss12.y, 6.67f, 3.33f, 1, 10.0f);
				//SpawnProjectile(boss12.x, boss12.y, 3.33f, 6.67f, 1, 10.0f);
				SpawnProjectile(boss12.x, boss12.y, 0, 10.0f, 1, 10.0f);
				//SpawnProjectile(boss12.x, boss12.y, -3.33f, 6.67f, 1, 10.0f);
				//SpawnProjectile(boss12.x, boss12.y, -6.67f, 3.33f, 1, 10.0f);
				SpawnProjectile(boss12.x, boss12.y, -10.0f, 0, 1, 10.0f);
				//SpawnProjectile(boss12.x, boss12.y, -6.67f, -3.33f, 1, 10.0f);
				//SpawnProjectile(boss12.x, boss12.y, -3.33f, -6.67f, 1, 10.0f);
				SpawnProjectile(boss12.x, boss12.y, 0, -10.0f, 1, 10.0f);
				//SpawnProjectile(boss12.x, boss12.y, 3.33f, -6.67f, 1, 10.0f);
				//SpawnProjectile(boss12.x, boss12.y, 6.67f, -3.33f, 1, 10.0f);
			}
		}
		
		boss12.x += boss12.hsp * ((float)boss12.tempo/50)*tester;
		boss12.y += boss12.vsp * ((float)boss12.tempo/50)*tester;

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