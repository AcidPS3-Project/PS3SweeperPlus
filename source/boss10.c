#include "boss10.h"

Boss10 boss10_list[MAX_BOSS10];

void SetupBoss10Positions(int count)
{
	float center_x = screen_w-32.0f;
	float center_y = screen_h + ui_offset_y-32.0f;
	
    float margin_x = center_x * 0.30f;
    float margin_y = center_y * 0.30f;

    float left   = margin_x;
    float right  = (center_x * 2.0f) - margin_x;
    float top    = margin_y;
    float bottom = (center_y * 2.0f) - margin_y;
	float dist = 0;

    float cx = center_x;
    float cy = center_y;

    if (count > MAX_BOSS10) count = MAX_BOSS10;
	boss10_count = count;

    memset(boss10_list, 0, sizeof(boss10_list));

    switch (count)
    {
        case 1:
            boss10_list[0].x = cx;
            boss10_list[0].y = cy;
            break;
			
		case 2:
			dist = screen_w * 0.55f;

			boss10_list[0].x = cx - dist;
			boss10_list[0].y = cy;

			boss10_list[1].x = cx + dist;
			boss10_list[1].y = cy;
			break;

        case 3:
            boss10_list[0].x = left;
            boss10_list[0].y = top;

            boss10_list[1].x = right;
            boss10_list[1].y = top;

            boss10_list[2].x = cx;
            boss10_list[2].y = bottom;
            break;

        default: // 4+
            boss10_list[0].x = left;
            boss10_list[0].y = top;

            boss10_list[1].x = right;
            boss10_list[1].y = top;

            boss10_list[2].x = left;
            boss10_list[2].y = bottom;

            boss10_list[3].x = right;
            boss10_list[3].y = bottom;
            break;
    }

    for (int i = 0; i < count; i++)
    {
        boss10_list[i].active = false;
        boss10_list[i].pulse = 0.0f;
        boss10_list[i].time = 10;

        // give each one different movement so they don't mirror perfectly
        // switch (i)
        // {
            // case 0: boss10_list[i].hsp =  2.0f; boss10_list[i].vsp =  1.5f; break;
            // case 1: boss10_list[i].hsp = -2.0f; boss10_list[i].vsp = -1.5f; break;
            // case 2: boss10_list[i].hsp =  1.6f; boss10_list[i].vsp = -2.0f; break;
            // case 3: boss10_list[i].hsp = -1.6f; boss10_list[i].vsp =  2.0f; break;
        // }
    }
}
void boss10_update(int i)
{
    float TMP_SIZ = 3.0f * RENDERING_PIXELS;
    float boss_half = (TMP_SIZ * 3.0f * 1.1f) * 0.5f;
	float boundary = 384.0f / 4;

    float left   = boss_half - boundary;
    float right  = (screen_w*2) - boss_half + boundary - TMP_SIZ;
    float top    = boss_half - boundary;
    float bottom = (screen_h*2) - boss_half + boundary - TMP_SIZ;

    boss10_list[i].x += boss10_list[i].hsp;
    boss10_list[i].y += boss10_list[i].vsp;

    if (boss10_list[i].x < left)
    {
        boss10_list[i].x = left;
        boss10_list[i].hsp = -boss10_list[i].hsp;
		PlaySFX(&sfx_10bump);
    }
    else if (boss10_list[i].x > right)
    {
        boss10_list[i].x = right;
        boss10_list[i].hsp = -boss10_list[i].hsp;
		PlaySFX(&sfx_10bump);
    }
    if (boss10_list[i].y < top)
    {
        boss10_list[i].y = top;
        boss10_list[i].vsp = -boss10_list[i].vsp;
		PlaySFX(&sfx_10bump);
    }
    else if (boss10_list[i].y > bottom)
    {
        boss10_list[i].y = bottom;
        boss10_list[i].vsp = -boss10_list[i].vsp;
		PlaySFX(&sfx_10bump);
    }
}

void Deactivate10()
{	
	int i;
	for (i = 0; i < boss10_count; i++) boss10_list[i].active = false;
}