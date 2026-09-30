#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

typedef struct
{
	bool MissionCompleteTier1;
	bool MissionCompleteTier2;
	bool MissionCompleteTier3;
	bool MissionCompleteTier4;
	bool MissionCompleteTier5;
	bool MissionCompleteTier6;
	bool MissionCompleteTier7;
} AchievementsDataBase;

extern AchievementsDataBase AchDb;

#endif