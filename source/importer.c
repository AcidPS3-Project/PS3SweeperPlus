#include "importer.h"
#include "assets.h"
#include "libraries.h"
#include "soundlib/spu_soundlib.h"
#include "soundlib/audioplayer.h"
#include "spu_soundmodule.bin.h"

char global_name[] = "PS3Sweeper+";
char global_version[] = "v0.4";

int current_bgm = 1;
int desired_bgm = 1;

int smiley_face = 0;

float ship_x = -40.0f;
float ship_y = -22.0f;
int ship_r = 0;
bool ship_a = false;

bool show_shit = false;
int global_route = 0;

int ranks_ep_1[8];
int ranks_ep_2[8];
int ranks_ep_3[8];
int ranks_ep_4[8];

int rank_result = 0;
int ten_arena = 0;

bool easter_egg = false;

bool button_x_hold = false;
bool button_start_hold = false;
bool paused = false;

int global_minecoins = 0;
int current_minecoins = 0;
int level_minecoins = 0;
float RENDER_SIZE;

float time_total = 0.0f;

int RENDERING_PIXELS = 16;
float RENDERING_SIZE = 2.0f;
int FIRST_CLICK = 1;

int i_10 = 0;
int language = 0;

int mistakes = 0;

bool use_custom_sweeper = false;
int custom_sweeper_size = 0;

float timer_seconds = 0;
int timer_running = 0;

int screen_w = 848 / 2;
int screen_h = 512 / 2;

int offset_x;
int offset_y;
int ui_offset_y = 66;

int bad_apple = 0;
int temp_fp_music = 0;

int setting_guarantee = 0;
bool shop_unlocked = 0;

bool playing_bgm = false;
bool custom_music = false;

bool has_custom_main = false;
bool has_custom_win  = false;

bool seen_nine = false;
bool seen_fnine = false;
bool seen_ten = false;
bool seen_nten = false;
bool seen_eleven = false;
bool seen_meleven = false;
bool seen_twelve = false;
// bool seen_touhoutwelve = false;
bool seen_nan = false;

bool show_warning = false;

void SetupTimeSettings()
{
	for(int _t = 0; _t < MAXLVLTEST; _t++)
	{
		level_time[_t] = 99999.9f;
	}
}

int EPISODE = 1;
int EPISODEs = 1; //for showing off
int LEVEL = 0;
int DIFFICULTY = 1;
int DIFFICULTYs = 1; //for showing off
int LIFES = 3;

int the_bg = 0;

int game_loaded = 0;
int free_play = 0;
int draw_grid_outlines = 0;

int explode_time = 0;
int disable_control = 0;
int reveal_mode = 0;
int start_music = 0;
int game_init = 0;

int splash_count = 0;
bool shop_first = false;
bool first_playing = true;

int fps = 0;
static int frames = 0;
static u64 last_time = 0;

float menu_x = 0.0f;
int menu_pos = 0;
float ze_x = 0.0f;
float ze_y = 0.0f;

bool shop_unlock_0 = false;
bool shop_unlock_1 = false;
bool shop_unlock_2 = false;
bool shop_unlock_3 = false;
bool shop_unlock_4 = false;
bool shop_unlock_5 = false;

bool specialthanks = false;

bool route_touhou_unlocked = false;

float lerp(int x, int y, float amount)
{
	return x + amount * (y - x);
}

void UpdateFPS()
{
    u64 now = sysGetSystemTime();
    frames++;

    if(now - last_time >= 1000000ULL)
    {
        fps = frames;
        frames = 0;
        last_time = now;
    }
}

void creditsInit()
{
	credits[0] = "Jorel Simpson";
	credits[1] = "Axelbastian";
	credits[2] = "cruslan";
	credits[3] = "bucanero";
	credits[4] = "Pren3k";
	credits[5] = "LokiThePlug";
	credits[6] = "EnergyPumpkin";
	credits[7] = "Team Shanghai Alice";
	credits[8] = "Alien Captain's Creator";
	credits[9] = "Cynthia";
	credits[10] = "PSL1GHT";
	credits[11] = "TOCHARA";
	credits[12] = "tiptopik";
	credits[13] = "Kirill9ka";
	credits[14] = "MirrorGuy";
	credits[15] = "artmet17";
	credits[16] = "Giga-T";
	credits[17] = "GemThe14";
	credits[18] = "Twelv";
	credits[19] = "Affocoder";
	credits[20] = "Jack Heredia";
	credits[21] = "SilverClockBFDI";
	credits[22] = "1tsLek";
	credits[23] = "Creeper7925";
}

void SplashesInit()
{
	splash_count = 0;
	int bad_apple_dlc_installed = 0;
	
	splashes_random[0] = "Unused Splash?";
	splashes_random[splash_count++] = "Sweeping on PS3!";
	splashes_random[splash_count++] = "This is NOT a port!";
	splashes_random[splash_count++] = "Does not require PS Eye!";
	splashes_random[splash_count++] = "April fools joke was peak";
	splashes_random[splash_count++] = "Contains touhouslop";
	splashes_random[splash_count++] = "Why so serious?";
	splashes_random[splash_count++] = "Keyboard is real?";
	splashes_random[splash_count++] = "Join AcidPS3DEV Discord Server!";
	splashes_random[splash_count++] = "Play the original game first!";
	splashes_random[splash_count++] = "Also play yawn's minesweeper";
	splashes_random[splash_count++] = "Also play Minesweeper Plus Plus";
	splashes_random[splash_count++] = "Also play Minesweeper Minus";
	splashes_random[splash_count++] = "Also play Minesweeper Plus: Second Destination (if it comes out)";
	splashes_random[splash_count++] = "Also play Minesweeper Plus: Alien Captain Edition";
	splashes_random[splash_count++] = "Also play Minesweeper Times- oh wait. its canceled :(";
	splashes_random[splash_count++] = "Also play LandMinesweeper Plus";
	splashes_random[splash_count++] = "Also play Minesweeper Plus: Hard Mode";
	splashes_random[splash_count++] = "Also play Minesweeper Chronicles (if it gets hopefully finished)";
	splashes_random[splash_count++] = "Also play Minesweeper Plus on Roblox";
	splashes_random[splash_count++] = "Also play PolySweep (if it comes out)";
	splashes_random[splash_count++] = "Also play Minesweeper Plus: Deluxe (if it comes out)";
	splashes_random[splash_count++] = "Also play Minesweeper Plus: Community Edition";
	splashes_random[splash_count++] = "Also play Minesweeper Oddysey (if it comes out)";
	splashes_random[splash_count++] = "Also play Minesweeper Plus: Epilogue";
	splashes_random[splash_count++] = "Also play Minesweeper Plus: Silver's Custom Adventure";
	splashes_random[splash_count++] = "Also play Hero's Advent";
	splashes_random[splash_count++] = "The MORSHU.";
	splashes_random[splash_count++] = "Why it was rewritten?";
	splashes_random[splash_count++] = "Better, than Minesweeper Plus: PS3 Edition";
	splashes_random[splash_count++] = "AI-Assisted code, because I suck at C.";
	splashes_random[splash_count++] = "Optimizing is at it's peakest.";
	splashes_random[splash_count++] = "Baldi for PS3 re-write coming? Wait..";
	splashes_random[splash_count++] = "CELL Programming";
	splashes_random[splash_count++] = "Yellow light of Death";
	splashes_random[splash_count++] = "Reality Synthesizer can fail?";
	//splashes_random[splash_count++] = "NEC-Tokens included!";
	splashes_random[splash_count++] = "I really suck at PS3 games coding.";
	splashes_random[splash_count++] = "Definitely does NOT work with aPS3e!";
	splashes_random[splash_count++] = "May work with RPCS3, but freezes rarely happen. Be careful!";
	splashes_random[splash_count++] = "Watch no one play this";
	splashes_random[splash_count++] = "The definition of Insanity.";
	splashes_random[splash_count++] = "up up down down left right left right circle cross start.\n     oh wait, it doesnt work";
	splashes_random[splash_count++] = "why do i stay all night for this";
	splashes_random[splash_count++] = "I wanna try making MS+ for PS1 sometime soon. Wait a second...";
	splashes_random[splash_count++] = "What R2 do? BEEP BEEP BEEP";
	splashes_random[splash_count++] = "48x36 boards DO NOT GIVE YOU FRAMEDROPS!!!!!!!";
	splashes_random[splash_count++] = "Now with Full Episode 2!";
	splashes_random[splash_count++] = "12 is coming...?";
	splashes_random[splash_count++] = "Minesweeper Plus on Java phones? Heh. Don't make me laugh.";
	splashes_random[splash_count++] = "DoodleBob plays his game on PS3. Neat.";
	splashes_random[splash_count++] = "The great fraction of time definition is... i forgor";
	splashes_random[splash_count++] = "man why everyone focused on bruhsweeper delta -_-";
	splashes_random[splash_count++] = "Привет, Россия!";
	splashes_random[splash_count++] = "Сәлем, Қазақстан!";
	splashes_random[splash_count++] = "01010000 01010011 00110011 !!!";
	splashes_random[splash_count++] = "211208 :>";
	splashes_random[splash_count++] = "Touhou mentioned";
	splashes_random[splash_count++] = "True PS3 Gaming B)";
	splashes_random[splash_count++] = "Bruhsweeper Delta is getting attention again:sob:";
	splashes_random[splash_count++] = "Don't let your dreams be dreams!";
	splashes_random[splash_count++] = "Stay safe, folks!";
	splashes_random[splash_count++] = "High Definition B)";
	splashes_random[splash_count++] = "Ask Jorel to get the PS3 ;)";
	splashes_random[splash_count++] = "MOM GET THE CAMERA";
	splashes_random[splash_count++] = "Share with your friends! The more, the merrier!";
	splashes_random[splash_count++] = "WELCOME TO THE AMAZING DIGITAL CREATION OF HATRED wait wha-";
	splashes_random[splash_count++] = "BRING BACK FLANDREKIDD";
	splashes_random[splash_count++] = "JACK BEATED THE OG GAME WITH EVERY M RANK";
	splashes_random[splash_count++] = "I dare anyone complete Expert 10 without dying";
	splashes_random[splash_count++] = "Sweepers gonna sweep";
	splashes_random[splash_count++] = "Don't forget to feed me with a spoon!";
	//splashes_random[splash_count++] = "Hi, Xbox Original :)";
	//splashes_random[splash_count++] = "I wonder what I can do for Mac OS 9 tho...";
	splashes_random[splash_count++] = "Cutscenes?";
	// splashes_random[splash_count++] = "v0.4 is here. Prepare thyself!!!";
	splashes_random[splash_count++] = "BUGS. I HATE BUGS!!!!";
	splashes_random[splash_count++] = "Hate Lesbian Space Princess. Love PS3Sweeper+.";
	splashes_random[splash_count++] = "Ay, mate! Don't get snowgraved, heh!";
	splashes_random[splash_count++] = "Frozen Nine is not related to Deltarune.";
	splashes_random[splash_count++] = "He's gonna take you back to the past.";
	splashes_random[splash_count++] = "Does not contain too much if else stuff!";
	splashes_random[splash_count++] = "One of the First PS3 games that support Kazakh Language!";
	splashes_random[splash_count++] = "Watch no one play this x2.";
	splashes_random[splash_count++] = "v0.3.5? When v0.4???";
	splashes_random[splash_count++] = "NaN is coming...";
	splashes_random[splash_count++] = "Somebody once told me the world is gonna roll me.";
	
	FILE *za = fopen("/dev_hdd0/game/ACIDSTORE/ICON0.PNG", "rb");
	if(za!=NULL)
	{
		splashes_random[splash_count++] = "Thanks for installing AcidPS3 Store! :D";
		splashes_random[splash_count++] = "Wow dude you have AcidPS3 Store installed? :)";
		fclose(za);
	}
	
	FILE *z1 = fopen("/dev_hdd0/game/MINESWEEP/USRDIR/dlc00/data.dat", "rb");
	FILE *z2 = fopen("/dev_hdd0/game/MINESWEEP/USRDIR/dlc00/mus.mp3", "rb");
	
	if(z1!=NULL)
	{
		bad_apple_dlc_installed++;
		fclose(z1);
	}
	if(z2!=NULL)
	{
		bad_apple_dlc_installed++;
		fclose(z2);
	}
	
	if(bad_apple_dlc_installed==2)
	{
		splashes_random[splash_count++] = "Nagareteku toki no naka de demo kedarusa ga hora guru guru mowaate\nwatashi kara hanareru kokoro mo mienai waa soo shiranai\ndo you even read this?";
		splashes_random[splash_count++] = "Bad Apple!! DLC Initialized!";
		splashes_random[splash_count++] = "I am a Bad Apple!! addict.";
		splashes_random[splash_count++] = "BAD APPLE!! SLOP";
		splashes_random[splash_count++] = "THE. BAD. APPLE. !!.";
		DebugLog("Bad Apple!! DLC Init!");
	}
	else if(bad_apple_dlc_installed==1)
	{
		splashes_random[splash_count++] = "Bad Apple!! DLC not installed correctly.";
		splashes_random[splash_count++] = "Please re-install Bad Apple!! DLC";
		DebugLog("Bad Apple!! DLC Init fail.");
	}
};

int SplashesCount()
{
	return splash_count;
}

int sex = 2147483647;

void init_bgm()
{
	for(int z = 0; z < BGM_LIST; z++)
	{
		bgm[z] = "";
		bgm_start[z] = 0;
		bgm_end[z] = sex;
	}
	
	bgm[0] = MUS_DIR "title.mp3";
	bgm[1] = MUS_DIR "bgm1.mp3";
	bgm_start[1] = 499;
	bgm_end[1] = 45281;
	
	bgm[2] = MUS_DIR "bgm2.mp3";
	bgm_start[2] = 7801;
	bgm_end[2] = 54617;
	
	bgm[3] = MUS_DIR "bgm3.mp3";
	bgm[4] = MUS_DIR "bgm4.mp3";
	bgm_start[4] = 1782;
	bgm_end[4] = 137481;
	
	bgm[5] = MUS_DIR "bgm5.mp3";
	bgm[6] = MUS_DIR "bgm6.mp3";
	bgm_start[6] = 13439;
	bgm_end[6] = 69080;
	
	bgm[7] = MUS_DIR "bgm7.mp3";
	bgm[8] = MUS_DIR "bgm8.mp3";
	bgm_start[8] = 6548;
	bgm_end[8] = 104708-10;
	
	bgm[9] = MUS_DIR "ocean.mp3";
	bgm[10] = MUS_DIR "boss9_intro.mp3";
	bgm_start[10] = 29632;
	bgm_end[10] = 33763;
	
	bgm[11] = MUS_DIR "boss9.mp3";
	bgm_start[11] = 10995;
	bgm_end[11] = 100527;
	
	bgm[12] = MUS_DIR "boss10_intro.mp3";
	bgm_start[12] = 23725;
	bgm_end[12] = 26893;
	
	bgm[13] = MUS_DIR "boss10.mp3";
	bgm[14] = MUS_DIR "boss11_2.mp3";
	bgm[15] = MUS_DIR "boss11_intro.mp3";
	bgm_start[15] = 42881;
	bgm_end[15] = 70250-10;
	
	bgm[16] = MUS_DIR "boss11.mp3";
	bgm_start[16] = 3438;
	bgm_end[16] = 127540-10;
	
	bgm[17] = MUS_DIR "bgmrs.ogg";
	bgm_start[17] = 44300;
	bgm_end[17] = 103380;
	
	bgm[18] = MUS_DIR "bgms3m.ogg";
	bgm_start[18] = 44300;
	bgm_end[18] = 103380;
	
	bgm[19] = MUS_DIR "boss12_intro.ogg";
	bgm_start[19] = 23283;
	bgm_end[19] = 35027-10;
	
	bgm[20] = MUS_DIR "boss12.ogg";
	bgm[21] = MUS_DIR "boss12_2.ogg";
	
	bgm[51] = MUS_DIR "Ags1.mp3";
	bgm[52] = MUS_DIR "Ags2.mp3";
	bgm[53] = MUS_DIR "Ags3.mp3";
	bgm[54] = MUS_DIR "Ags4.mp3";
	bgm[55] = MUS_DIR "Ags5.mp3";
	bgm[56] = MUS_DIR "Ags6.mp3";
	bgm[57] = MUS_DIR "Ags7.mp3";
	bgm[58] = MUS_DIR "Ags8.mp3";
	bgm[59] = MUS_DIR "Ags9.mp3";
	bgm[60] = MUS_DIR "Ags10.mp3";
	bgm[61] = MUS_DIR "Ags11.mp3";
	bgm[62] = MUS_DIR "Agsrs.ogg";
	bgm[63] = MUS_DIR "Agss3m.ogg";
	bgm[64] = MUS_DIR "Ags12.ogg";
	
	// 99 - custom music
	// 100 - bad apple!!
	
	bgm[101] = MUS_DIR "win1.mp3";
	bgm[102] = MUS_DIR "win2.mp3";
	bgm[103] = MUS_DIR "win3.mp3";
	bgm[104] = MUS_DIR "win4.mp3";
	bgm[105] = MUS_DIR "win5.ogg";
	bgm[106] = MUS_DIR "win6.ogg";
	bgm[107] = MUS_DIR "win7.ogg";
	bgm[108] = MUS_DIR "win8.ogg";
	bgm[109] = MUS_DIR "boss9_win.ogg";
	bgm[110] = MUS_DIR "boss10_win.ogg";
	bgm[111] = MUS_DIR "boss11_win.ogg";
	bgm[112] = MUS_DIR "boss12_win.ogg";
	bgm[117] = MUS_DIR "winrs.ogg";
	bgm[118] = MUS_DIR "wins3m.ogg";
	
	bgm[151] = MUS_DIR "ls1.mp3";
	bgm[152] = MUS_DIR "ls2.mp3";
	bgm[153] = MUS_DIR "ls3.mp3";
	bgm[154] = MUS_DIR "ls4.mp3";
	bgm[155] = MUS_DIR "ls5.mp3";
	bgm[156] = MUS_DIR "ls6.mp3";
	bgm[157] = MUS_DIR "ls7.mp3";
	bgm[158] = MUS_DIR "ls8.mp3";
	bgm[159] = MUS_DIR "ls9.mp3";
	bgm[160] = MUS_DIR "ls10.mp3";
	bgm[161] = MUS_DIR "ls11.mp3";
	bgm[162] = MUS_DIR "lsrs.ogg";
	bgm[163] = MUS_DIR "lss3m.ogg";
	bgm[164] = MUS_DIR "ls12.ogg";
	
	bgm[190] = MUS_DIR "wind.ogg";
	bgm[191] = MUS_DIR "untitled.ogg";
	bgm[192] = MUS_DIR "credits.ogg";
	bgm[193] = MUS_DIR "ending.ogg";
	bgm[194] = MUS_DIR "fourofakind_intro.ogg";
	bgm[195] = MUS_DIR "fourofakind.ogg";
	bgm[197] = MUS_DIR "shop.ogg";
	bgm[198] = MUS_DIR "logo.mp3";
	// 199 - custom win music
	
	// Touhou route shit
	
	bgm[201] = MUS_DIR "touhou/bgm1.ogg";
	bgm_start[201] = 18849;
	bgm_end[201] = 87395;
	
	bgm[202] = MUS_DIR "touhou/bgm2.ogg";
	bgm_start[202] = 414;
	bgm_end[202] = 139373;
	
	bgm[203] = MUS_DIR "touhou/bgm3.ogg";
	bgm_start[203] = 775;
	bgm_end[203] = 75084;
	
	bgm[204] = MUS_DIR "touhou/bgm4.ogg";
	bgm_start[204] = 25600;
	bgm_end[204] = 115160;
	
	bgm[205] = MUS_DIR "touhou/bgm5.ogg";
	bgm_start[205] = 17504;
	bgm_end[205] = 88304;
	
	// bgm[206] = MUS_DIR "touhou/bgm6.ogg";
	// bgm[207] = MUS_DIR "touhou/bgm7.ogg";
	// bgm[208] = MUS_DIR "touhou/bgm8.ogg";
	
	// what the fuck
	// bgm[209] = MUS_DIR "touhou/boss9_2.ogg";
	// bgm_start[209] = 23337;
	// bgm_end[209] = 68479;
	
	bgm[210] = MUS_DIR "touhou/boss9_intro.ogg";
	bgm_start[210] = 29632;
	bgm_end[210] = 33763;
	
	bgm[211] = MUS_DIR "touhou/boss9.ogg";
	bgm_start[211] = 14172;
	bgm_end[211] = 129339;
	
	bgm[212] = MUS_DIR "touhou/boss10_intro.ogg";
	bgm_start[212] = 23725;
	bgm_end[212] = 26893;
	
	bgm[213] = MUS_DIR "touhou/boss10.ogg";
	bgm[214] = MUS_DIR "touhou/boss11_2.ogg";
	bgm[215] = MUS_DIR "touhou/boss11_intro.ogg";
	bgm_start[215] = 42881;
	bgm_end[215] = 70250-10;
	
	bgm[216] = MUS_DIR "touhou/boss11.ogg";
	bgm_start[216] = 3438;
	bgm_end[216] = 145504;
	
	bgm[251] = MUS_DIR "touhou/Ags1.ogg";
	bgm[252] = MUS_DIR "touhou/Ags2.ogg";
	bgm[253] = MUS_DIR "touhou/Ags3.ogg";
	bgm[254] = MUS_DIR "touhou/Ags4.ogg";
	bgm[255] = MUS_DIR "touhou/Ags5.ogg";
	// bgm[256] = MUS_DIR "touhou/Ags6.ogg";
	// bgm[257] = MUS_DIR "touhou/Ags7.ogg";
	// bgm[258] = MUS_DIR "touhou/Ags8.ogg";
	bgm[259] = MUS_DIR "touhou/Ags9.ogg";
	bgm[260] = MUS_DIR "touhou/Ags10.ogg";
	bgm[261] = MUS_DIR "touhou/Ags11.ogg";
	
	bgm[309] = MUS_DIR "touhou/boss9_win.ogg";
	bgm[310] = MUS_DIR "touhou/boss10_win.ogg";
	bgm[311] = MUS_DIR "touhou/boss11_win.ogg";
	
	bgm[351] = MUS_DIR "touhou/ls1.ogg";
	bgm[352] = MUS_DIR "touhou/ls2.ogg";
	bgm[353] = MUS_DIR "touhou/ls3.ogg";
	bgm[354] = MUS_DIR "touhou/ls4.ogg";
	bgm[355] = MUS_DIR "touhou/ls5.ogg";
	// bgm[356] = MUS_DIR "touhou/ls6.ogg";
	// bgm[357] = MUS_DIR "touhou/ls7.ogg";
	// bgm[358] = MUS_DIR "touhou/ls8.ogg";
	bgm[359] = MUS_DIR "touhou/ls9.ogg";
	bgm[360] = MUS_DIR "touhou/ls10.ogg";
	bgm[361] = MUS_DIR "touhou/ls11.ogg";
	
	// coming soon 0_0
}

int LoadSFX(SoundEffect *sfx, const void *data, int size)
{
    sfx->pcm_size = size * 20;
	sfx->pcm = memalign(32, sfx->pcm_size);

    if(!sfx->pcm) return 0;
    if(DecodeAudio((void *)data,size,sfx->pcm,&sfx->pcm_size,&sfx->freq,&sfx->stereo) != 0)
    {
        free(sfx->pcm);
        sfx->pcm = NULL;
        return 0;
    }

    return 1;
}

void init_sfx()
{
	LoadSFX(&sfx_flag, flag_bin, flag_bin_size);
	LoadSFX(&sfx_unflag, unflag_bin, unflag_bin_size);
	LoadSFX(&sfx_9move, nine_move_bin, nine_move_bin_size);
	LoadSFX(&sfx_9stop, nine_stop_bin, nine_stop_bin_size);
	LoadSFX(&sfx_10bump, ten_bump_bin, ten_bump_bin_size);
	LoadSFX(&sfx_10reset, ten_reset_bin, ten_reset_bin_size);
	LoadSFX(&sfx_mine, game_mine_bin, game_mine_bin_size);
	LoadSFX(&sfx_explosion, game_explosion_bin, game_explosion_bin_size);
	LoadSFX(&sfx_11open, eleven_open_bin, eleven_open_bin_size);
	LoadSFX(&sfx_11bonk, eleven_bonk_bin, eleven_bonk_bin_size);
	LoadSFX(&sfx_buy, cash_register_bin, cash_register_bin_size);
	LoadSFX(&sfx_level, level_start_bin, level_start_bin_size);
	LoadSFX(&sfx_level2, level_start2_bin, level_start2_bin_size);
	LoadSFX(&sfx_gamestart, game_start_bin, game_start_bin_size);
	LoadSFX(&sfx_beep, beep_bin, beep_bin_size);
	LoadSFX(&sfx_blackout, blackout_bin, blackout_bin_size);
	LoadSFX(&sfx_f9ice, fnine_ice_bin, fnine_ice_bin_size);
	LoadSFX(&sfx_siren, siren_bin, siren_bin_size);
}

int RandRange(int min, int max)
{
	int rndom;
	rndom = rand() % (max-min+1) + min;
	return rndom;
}

int max(int min, int max)
{
	int biggest;
	biggest = (min > max) ? min : max;
	
	return biggest;
}

void PlayBGM(int id, int inf)
{
	StopAudio();
	
	int temp = 0;
	if(global_route == 1) temp = 200;
	
	if(id>=190 || id==99 || id==100 || id==9 || id==0) temp = 0;
	
	FILE *fp = fopen(bgm[temp+id], "rb");
	
	if(!fp)
    {
        printf("Error! No music found.\n");
        return;
    }
	
    PlayAudiofd(fp, 0, inf ? AUDIO_INFINITE_TIME : AUDIO_ONE_TIME);
    current_bgm = id;
}

void UpdateBGM(void)
{
	s32 temp_t = GetTimeAudio();
	int tempzz = 50;
	if(temp_t >= bgm_end[current_bgm]-tempzz) SetTimeAudio(bgm_start[current_bgm]-tempzz);
}

void PlaySFX(SoundEffect *sfx)
{
	int voice = SND_GetFirstUnusedVoice();
    if (voice < 1) return;
	
    SND_SetVoice(voice, sfx->stereo ? VOICE_STEREO_16BIT : VOICE_MONO_16BIT,
        sfx->freq, 0, sfx->pcm, sfx->pcm_size, 255, 255, NULL);
	
	sfx->voice = voice;
}

void StopSFX(SoundEffect *sfx)
{
    if(sfx->voice >= 1)
    {
        SND_StopVoice(sfx->voice);
        sfx->voice = -1;
    }
}

void Load_PNG()
{
	pngLoadFromBuffer(captain_sheet_bin, captain_sheet_bin_size, &spritesheet[0]);
	pngLoadFromBuffer(tiles_sheet_bin, tiles_sheet_bin_size, &spritesheet[1]);
	pngLoadFromBuffer(ui_sheet_bin, ui_sheet_bin_size, &spritesheet[2]);
	pngLoadFromBuffer(boss_sheet_bin, boss_sheet_bin_size, &spritesheet[3]);
	pngLoadFromBuffer(boss1_sheet_bin, boss1_sheet_bin_size, &spritesheet[4]);
	pngLoadFromBuffer(dualboss_sheet_bin, dualboss_sheet_bin_size, &spritesheet[5]);
	//pngLoadFromBuffer(cursed_sheet_bin, cursed_sheet_bin_size, &spritesheet[6]); unused
	pngLoadFromBuffer(background_sheet_bin, background_sheet_bin_size, &spritesheet[7]);
	pngLoadFromBuffer(explosion_bin, explosion_bin_size, &spritesheet[8]);
	pngLoadFromBuffer(menu_sheet_bin, menu_sheet_bin_size, &spritesheet[9]);
	pngLoadFromBuffer(coin_bin, coin_bin_size, &spritesheet[10]);
	pngLoadFromBuffer(buttons_bin, buttons_bin_size, &spritesheet[11]);
	pngLoadFromBuffer(hats_sheet_bin, hats_sheet_bin_size, &spritesheet[12]);
	// WE ARE SKIPPING 13 cuz its reserved for custom captain spritesheet.
	pngLoadFromBuffer(explosion_anti_bin, explosion_anti_bin_size, &spritesheet[14]);
	pngLoadFromBuffer(shopkeeper_sheet_bin, shopkeeper_sheet_bin_size, &spritesheet[15]);
	pngLoadFromBuffer(end1_bin, end1_bin_size, &spritesheet[16]);
	pngLoadFromBuffer(end2_bin, end2_bin_size, &spritesheet[17]);
	pngLoadFromBuffer(end3_bin, end3_bin_size, &spritesheet[18]);
	pngLoadFromBuffer(shopbg_bin, shopbg_bin_size, &spritesheet[19]);
	// something for 20?
	pngLoadFromBuffer(tiles_sheet2_bin, tiles_sheet2_bin_size, &spritesheet[21]);
}

void ReplaceBGM(int music)
{
	StopAudio();
	usleep(3666);
	PlayBGM(50+music, 1);
}

bool FindMusic(const char *base, char *out_path)
{
    sprintf(out_path, "%s.mp3", base);
    FILE *fp = fopen(out_path, "rb");

    if(!fp)
    {
        sprintf(out_path, "%s.ogg", base);
        fp = fopen(out_path, "rb");
    }

    if(fp)
    {
        fclose(fp);
        return true;
    }

    return false;
}

void DemoCompleteCallback(msgButton button, void *usrData)
{
    sysProcessExit(0);
}

void EmptyDialogCallback(msgButton button, void *usrData)
{
    // ehhehe fuck this shit
}

void Initialize()
{
	DebugLog("Game initialization...");
	u32 spu;
	sysSpuImage spu_image;
	sysSpuInitialize(6, 5);
	sysSpuRawCreate(&spu, NULL);
	sysSpuImageImport(&spu_image, spu_soundmodule_bin, 0);
	sysSpuRawImageLoad(spu, &spu_image);
	SND_Init(spu);
	
	for(int i = 0; i < 8; i++)
	{
		ranks_ep_1[i] = -1;
		ranks_ep_2[i] = -1;
		ranks_ep_3[i] = -1;
		ranks_ep_4[i] = -1;
	}
}

void HolyShit()
{
	lv2syscall3(392, 0x1004, 0xA, 0x1B6);
}

void DebugLog(const char *text)
{
	printf("%s\n", text);
    FILE *fp = fopen("/dev_hdd0/game/MINESWEEP/USRDIR/logstuff.txt", "ab");

    if(fp)
    {
        fprintf(fp, "%s\n", text);
        fclose(fp);
    }
}

int LoadImageA(const char *path, pngData *texture)
{
    FILE *fp;
    u8 *buffer;
    long size;
    s32 ret;

    fp = fopen(path, "rb");
    if(!fp)
    {
        DebugLog("LoadImageA: fopen failed");
        return 0;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    buffer = malloc(size);
    if(!buffer)
    {
        DebugLog("LoadImageA: malloc failed");
        fclose(fp);
        return 0;
    }

    fread(buffer, 1, size, fp);
    fclose(fp);

    ret = pngLoadFromBuffer(buffer, (u32)size, texture);

    free(buffer);
    return ret == 0;
}