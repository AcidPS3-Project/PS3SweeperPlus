#include "achievements.h"

AchievementsNameDB AchNDb;
AchievementsDB AchDb[MAX_ACHIEVE] =
{
	{"When the tile is a mine", "Complete First Episode", &AchNDb.Ep1Complete, BRONZE},
	{"No More Radiation!", "Complete Second Episode", &AchNDb.Ep2Complete, BRONZE}, //requested by s1lverc1ock2763
	{"Demonic storm", "Complete Third Episode", &AchNDb.Ep3Complete, BRONZE},
	{"Time stops, bullet hell...", "Complete Fourth Episode", &AchNDb.Ep4Complete, BRONZE},
	{"<null>", "Complete Fifth Episode", &AchNDb.Ep5Complete, BRONZE},
	{"The Game has changed.", "Complete Sixth Episode", &AchNDb.Ep6Complete, BRONZE},
	{"Mission Complete!", "Complete the main game.", &AchNDb.GameComplete, BRONZE},
	{"The Gensokyo awaits", "Unlock the Touhou route.", &AchNDb.UnlockTouhou, BRONZE},
	{"You catched the cold, idiot", "Complete First Episode of Touhou route", &AchNDb.TEp1Complete, BRONZE},
	{"Seriously, no more radiation. This isn't funny!", "Complete Second Episode of Touhou route", &AchNDb.TEp2Complete, BRONZE},
	{"A menace behind the Gardener", "Complete Third Episode of Touhou route", &AchNDb.TEp3Complete, BRONZE},
	{"I KNEW IT WAS THE MAID!", "Complete Fourth Episode of Touhou route", &AchNDb.TEp4Complete, BRONZE},
	{"Basement dweller", "Complete Fifth Episode of Touhou route", &AchNDb.TEp5Complete, BRONZE},
	{"Just like in Touhou 12", "Complete Sixth Episode of Touhou route", &AchNDb.TEp6Complete, BRONZE},
	{"I knew it was her all along!", "Complete the Touhou route", &AchNDb.TGameComplete, BRONZE},
	{"BRO ARE YOU KIDDING?? HAHAHAHA", "Fail the first stage of Episode 1 Beginner with Master Mode enabled", &AchNDb.BROWHATLOL, BRONZE}, // requested by d_fund3781
	{"The Warrior of the Mines", "Play with all bosses on Master Mode", &AchNDb.Warrior, SILVER}, // requested by alien captain creator
	{"It's Sweeping Time!", "Get Episode 1 M-Rank Any difficulty", &AchNDb.Sweeping, SILVER}, // requested by 1tslek
	{"GAAAAAAHHHHHHHHHH", "Fail Bomb Bay on Master mode with 5 or less mines left", &AchNDb.GAAAHHHH, SILVER}, // requested by jack heredia
	{"The Mine Cluster", "Beat a Freeplay board with 24\% mine density or more", &AchNDb.MineCluster, SILVER}, // requested by alien captain creator
	{"Absolute Sweeper!", "M-Rank every episode of the Main game", &AchNDb.AbsSweep, GOLD}, // requested by creeper 7925
	{"Absolute Insanity", "Beat Bomb Bay Expert Boss Pursuit on Master Mode under 5 minutes", &AchNDb.Insanity, GOLD}, // requested by creeper 7925
	{"One Thousandth Stare", "Beat Episode 1 Expert on Master mode, no safe guarantees, flagless, hitless, and finish the campaign with an M Rank", &AchNDb.OneKStare, GOLD}, // requested by s1lverc1ock2763
	{"ONE MILLIONTH STARE!", "Beat Episode 1 Boss Pursuit Expert on Master mode, no safe guarantees, flagless, hitless, and finish the campaign with an M Rank", &AchNDb.OneMStare, GOLD}, // requested by creeper 7925, original request by s1lverc1ock2763
	{"HARDCORE RULES!!!!", "Collect all achievements", &AchNDb.HARDCORE, PLATINUM}
};

//static const int AchCount = sizeof(AchDb) / sizeof(AchDb[0]);