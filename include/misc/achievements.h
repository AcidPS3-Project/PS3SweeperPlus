#ifndef ACHIEVEMENTS_H
#define ACHIEVEMENTS_H

#include "libraries.h"

#define MAX_ACHIEVE 150

#define BRONZE 0
#define SILVER 1
#define GOLD 2
#define PLATINUM 3

typedef struct
{
    const char *name;
    const char *desc;
	bool *achievement;
	int type;
} AchievementsDB;

typedef struct
{
	bool Ep1Complete;
	bool Ep2Complete;
	bool Ep3Complete;
	bool Ep4Complete;
	bool Ep5Complete;
	bool Ep6Complete;
	bool GameComplete;
	bool UnlockTouhou;
	bool TEp1Complete;
	bool TEp2Complete;
	bool TEp3Complete;
	bool TEp4Complete;
	bool TEp5Complete;
	bool TEp6Complete;
	bool TGameComplete;
	bool Warrior;
	bool Insanity;
	bool OneKStare;
	bool HARDCORE;
	bool BROWHATLOL;
	bool Sweeping;
	bool GAAAHHHH;
	bool MineCluster;
	bool AbsSweep;
	bool OneMStare;
} AchievementsNameDB;

extern AchievementsNameDB AchNDb;
AchievementsDB AchDb[MAX_ACHIEVE];

#endif