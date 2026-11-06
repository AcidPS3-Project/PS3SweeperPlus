#include "achievements.h"

AchievementsNameDB AchNDb;
AchievementsDB AchDb[MAX_ACHIEVE] =
{
	{"When the tile is a mine", "Complete First Episode", &AchNDb.Ep1Complete, BRONZE},
	{"No More Radiation!", "Complete Second Episode", &AchNDb.Ep2Complete, BRONZE}, //name requested by s1lverc1ock2763
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
	{"So Retro...", "Complete First Episode of GameBoy route", &AchNDb.GEp1Complete, BRONZE},
	{"RetroToxic1999", "Complete Second Episode of GameBoy route", &AchNDb.GEp2Complete, BRONZE},
	{"Super Minesweeper", "Complete Third Episode of GameBoy route", &AchNDb.GEp3Complete, BRONZE},
	{"Starting from 12:00 AM", "Complete Fourth Episode of GameBoy route", &AchNDb.GEp4Complete, BRONZE},
	{"Reminds me of Level 256", "Complete Fifth Episode of GameBoy route", &AchNDb.GEp5Complete, BRONZE},
	{"Silver Surfer Sucks.", "Complete Sixth Episode of GameBoy route", &AchNDb.GEp6Complete, BRONZE},
	{"The Final Retro", "Complete the GameBoy route", &AchNDb.GGameComplete, BRONZE},
	{"This isn't funny bro.", "Unlock the Weird route.", &AchNDb.UnlockWeird, BRONZE},
	{"What the FUCK was that?", "Complete First Episode of Weird route", &AchNDb.WEp1Complete, BRONZE},
	{"The Devil's WHAT???", "Complete Second Episode of Weird route", &AchNDb.WEp2Complete, BRONZE},
	{"bro i went insane", "Complete Third Episode of Weird route", &AchNDb.WEp3Complete, BRONZE},
	{"why this exists:sob:", "Complete Fourth Episode of Weird route", &AchNDb.WEp4Complete, BRONZE},
	{"nah", "Complete Fifth Episode of Weird route", &AchNDb.WEp5Complete, BRONZE},
	{"*disconnected*", "Complete Sixth Episode of Weird route", &AchNDb.WEp6Complete, BRONZE},
	{"finally this shit is over.", "Complete the Weird route", &AchNDb.WGameComplete, BRONZE},
	{"Five Hundred Mines.", "Explode/Implode 500 mines in total", &AchNDb.Mines500, SILVER}, // requested by 1tslek
	{"BRO ARE YOU KIDDING?? HAHAHAHA", "Fail the first stage of Episode 1 Beginner with Master Mode enabled", &AchNDb.BROWHATLOL, SILVER}, // requested by d_fund3781
	{"The Warrior of the Mines", "Play with all bosses on Master Mode", &AchNDb.Warrior, SILVER}, // requested by alien captain creator
	{"It's Sweeping Time!", "Get Episode 1 M-Rank Any difficulty", &AchNDb.Sweeping, SILVER}, // requested by 1tslek
	{"GAAAAAAHHHHHHHHHH", "Fail Bomb Bay on Master mode with 5 or less mines left", &AchNDb.GAAAHHHH, SILVER}, // requested by jack heredia
	{"The Mine Cluster", "Beat a Freeplay board with 24\% mine density or more", &AchNDb.MineCluster, SILVER}, // requested by alien captain creator
	{"Nah I'm not doing this anymore", "Start the game over a save that's on Ep3 Stage 7 (Veteran+)", &AchNDb.NahFuckThis, SILVER}, // requested by d_fund3781
	{"Absolute Sweeper!", "M-Rank every episode of the Main game", &AchNDb.AbsSweep, GOLD}, // requested by creeper 7925
	{"Absolute Insanity", "Beat Bomb Bay Expert Boss Pursuit on Master Mode under 5 minutes", &AchNDb.Insanity, GOLD}, // requested by creeper 7925
	{"One Thousandth Stare", "Beat Episode 1 Expert on Master mode, no safe guarantees, flagless, hitless, and finish the campaign with an M Rank", &AchNDb.OneKStare, GOLD}, // requested by s1lverc1ock2763
	{"ONE MILLIONTH STARE!", "Beat Episode 1 Boss Pursuit Expert on Master mode, no safe guarantees, flagless, hitless, and finish the campaign with an M Rank", &AchNDb.OneMStare, GOLD}, // requested by creeper 7925, original request by s1lverc1ock2763
	{"HARDCORE RULES!!!!", "Collect all achievements", &AchNDb.HARDCORE, PLATINUM}
};

//static const int AchCount = sizeof(AchDb) / sizeof(AchDb[0]);