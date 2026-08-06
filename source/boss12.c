#include "boss12.h"

Boss12 boss12_list[MAX_BOSS12];

void SetupBoss12Positions(int count)
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

    if (count > MAX_BOSS12) count = MAX_BOSS12;
	boss12_count = count;

    memset(boss12_list, 0, sizeof(boss12_list));

    switch (count)
    {
        case 1:
            boss12_list[0].x = cx;
            boss12_list[0].y = cy;
            break;
			
		case 2:
			dist = screen_w * 0.55f;

			boss12_list[0].x = cx - dist;
			boss12_list[0].y = cy;

			boss12_list[1].x = cx + dist;
			boss12_list[1].y = cy;
			break;

        case 3:
            boss12_list[0].x = left;
            boss12_list[0].y = top;

            boss12_list[1].x = right;
            boss12_list[1].y = top;

            boss12_list[2].x = cx;
            boss12_list[2].y = bottom;
            break;

        default: // 4+
            boss12_list[0].x = left;
            boss12_list[0].y = top;

            boss12_list[1].x = right;
            boss12_list[1].y = top;

            boss12_list[2].x = left;
            boss12_list[2].y = bottom;

            boss12_list[3].x = right;
            boss12_list[3].y = bottom;
            break;
    }

    for (int i = 0; i < count; i++)
    {
        boss12_list[i].active = false;
    }
}

void Deactivate12()
{
	int i;
	for (i = 0; i < boss12_count; i++) boss12_list[i].active = false;
}