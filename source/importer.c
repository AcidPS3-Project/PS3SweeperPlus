#include "importer.h"

char global_name[] = "PS3Sweeper+";
char global_version[] = "v0.3";

int current_bgm = 1;
int desired_bgm = 1;

int global_minecoins = 0;
int current_minecoins = 0;
int level_minecoins = 0;
float RENDER_SIZE;

int RENDERING_PIXELS = 16;
float RENDERING_SIZE = 2.0f;
int FIRST_CLICK = 1;

int i_10 = 0;
int language = 0;

float timer_seconds = 0;
int timer_running = 0;

int screen_w = 848 / 2;
int screen_h = 512 / 2;

int offset_x;
int offset_y;
int ui_offset_y = 66;

int bad_apple = 0;
int temp_fp_music = 0;

bool playing_bgm = false;
bool custom_music = false;

bool has_custom_main = false;
bool has_custom_win  = false;

int EPISODE = 1;
int LEVEL = 0;
int DIFFICULTY = 1;
int DIFFICULTYs = 1; //for showing off

int the_bg = 0;

int ttf_inited = 0;
int game_loaded = 0;
int free_play = 0;
int draw_grid_outlines = 0;

int explode_time = 0;
int disable_control = 0;
int reveal_mode = 0;
int start_music = 0;
int game_init = 0;

int splash_count = 0;

int fps = 0;
static int frames = 0;
static u64 last_time = 0;

float menu_x = 0.0f;
int menu_pos = 0;
float ze_x = 0.0f;
float ze_y = 0.0f;

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

void LangInit()
{
	switch(language)
	{
		default:
		{
			menu_title[0] = "Minesweeper Adventure";
			menu_desc[0] = "Test your minesweeping skills in a sequence of progressively difficult Minesweeper stages.";
			menu_title[1] = "Free Play";
			menu_desc[1] = "Create your own custom Minesweeper stages and sweep mines to your heart's content!";
			menu_title[2] = "Bonus Games";
			menu_desc[2] = "Enjoy a selection of bonus game modes that will test your minesweeping limits.";
			menu_title[3] = "LoadOut";
			menu_desc[3] = "Customize the captain and stock up on supplies.";
			
			langStr[0] = "Loading...";
			langStr[1] = "Saving...";
			langStr[2] = "Made by AcidNT3.1 Productions";
			langStr[3] = "Welcome to PS3Sweeper+!";
			langStr[4] = "Warning! This game uses auto-save feature.";
			langStr[5] = "When you see the \"Saving...\" text, it saves your data.";
			langStr[6] = "Please, do NOT turn off your PlayStation 3 system while the \"Saving...\"";
			langStr[7] = "text is on-screen.";
			langStr[8] = "I understand.";
			langStr[9] = "This game is in early development stage.";
			langStr[10] = "Please join my discord server to not miss out new updates!";
			langStr[11] = "LINK: https://discord.gg/7XuQDYjyzt";
			langStr[12] = "There is nothing to load.";
			langStr[13] = "Start the Game.";
			langStr[14] = "Credits";
			langStr[15] = "Continue";
			langStr[16] = "Main Menu";
			langStr[17] = "Adventure Menu";
			langStr[18] = "Width";
			langStr[19] = "Height";
			langStr[20] = "Mines count";
			langStr[21] = "Doubles count";
			langStr[22] = "Halves count";
			langStr[23] = "Antis count";
			langStr[24] = "EPISODE";
			langStr[25] = "Music Type";
			langStr[26] = "CUSTOM";
			langStr[27] = "Freeplay Menu";
			langStr[28] = "Disable Custom Music";
			langStr[29] = "Enable Custom Music";
			langStr[30] = "Background Type";
			langStr[31] = "Start your game";
			langStr[32] = "Go Back";
			langStr[33] = "The Big One";
			langStr[34] = "Bad Apple!! (Requires DLC)";
			langStr[35] = "Challenges Menu";
			langStr[36] = "This menu is currently in works :)";
			langStr[37] = "GAME OVER";
			langStr[38] = "LEVEL CLEAR!";
			langStr[39] = "Minecoins";
			langStr[40] = "seconds";
			langStr[41] = "Next Level";
			langStr[42] = "Save and Quit";
			langStr[43] = "Reset game";
			break;
		}
		case 1: // Русский Язык
		{
			menu_title[0] = "Приключения Сапера";
			menu_desc[0] = "Проверь свои навыки сапера в усложняющей последовательности уровней сапера.";
			menu_title[1] = "Своя игра";
			menu_desc[1] = "Создавайте свой уровень сапера и разминируйте мины как душе угодно!";
			menu_title[2] = "Испытания";
			menu_desc[2] = "Насладитесь выбором испытаний которые проверят ваши знания сапера.";
			menu_title[3] = "Гардероб";
			menu_desc[3] = "Измените вид капитана и пополните запасы.";
			
			langStr[0] = "Загружаю...";
			langStr[1] = "Сохраняю...";
			langStr[2] = "Разработано AcidNT3.1 Productions";
			langStr[3] = "Приветствую вас в PS3Sweeper+!";
			langStr[4] = "Внимание! У этой игры есть функция авто-сохранения.";
			langStr[5] = "Когда вы видите текст \"Сохраняю...\", оно сохраняет игру.";
			langStr[6] = "Пожалуйста, не выключайте ваш PlayStation 3 когда текст";
			langStr[7] = "\"Сохраняю...\" находится на экране.";
			langStr[8] = "Хорошо.";
			langStr[9] = "Эта игра находится в раннем этапе разработки.";
			langStr[10] = "Пожалуйста, присоединяйтесь к моему дискорд серверу, чтобы не";
			langStr[11] = "пропустить обновления! ССЫЛКА: https://discord.gg/7XuQDYjyzt";
			langStr[12] = "Здесь нечего загружать.";
			langStr[13] = "Начать игру";
			langStr[14] = "Благодарность";
			langStr[15] = "Продолжить";
			langStr[16] = "Главное меню";
			langStr[17] = "Меню приключений";
			langStr[18] = "Ширина";
			langStr[19] = "Высота";
			langStr[20] = "Кол-во мин";
			langStr[21] = "Кол-во двойных мин";
			langStr[22] = "Кол-во полу-мин";
			langStr[23] = "Кол-во анти-мин";
			langStr[24] = "ЭПИЗОД";
			langStr[25] = "Стиль музыки";
			langStr[26] = "СВОЯ";
			langStr[27] = "Меню своей игры";
			langStr[28] = "Выключить свою музыку";
			langStr[29] = "Включить свою музыку";
			langStr[30] = "Стиль фона";
			langStr[31] = "Начать свою игру";
			langStr[32] = "Назад";
			langStr[33] = "Большевик";
			langStr[34] = "Плохое Яблоко!! (Потребуется Адд-он)";
			langStr[35] = "Меню испытаний";
			langStr[36] = "Это меню находится в разработке :)";
			langStr[37] = "ИГРА ОКОНЧЕНА";
			langStr[38] = "ПРОЙДЕНО!";
			langStr[39] = "Бомба-коины";
			langStr[40] = "секунд";
			langStr[41] = "Следующий уровень";
			langStr[42] = "Сохранить и выйти";
			langStr[43] = "Сбросить игру";
			break;
		}
	}
}

void SplashesInit()
{
	splash_count = 0;
	int bad_apple_dlc_installed = 0;
	
	splashes_random[splash_count++] = "Sweeping on PS3!";
	splashes_random[splash_count++] = "This is NOT a port!";
	splashes_random[splash_count++] = "Does not require PS Eye!";
	splashes_random[splash_count++] = "April fools joke was peak";
	splashes_random[splash_count++] = "Contains touhouslop";
	splashes_random[splash_count++] = "Why so serious?";
	splashes_random[splash_count++] = "Keyboard is real?";
	splashes_random[splash_count++] = "Removed 514 Boss";
	splashes_random[splash_count++] = "Join AcidPS3 Server! https://discord.gg/7XuQDYjyzt";
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
	splashes_random[splash_count++] = "up up down down left right left right circle cross start.\n   oh wait, it doesnt work";
	splashes_random[splash_count++] = "why do i stay all night for this";
	splashes_random[splash_count++] = "I wanna try making MS+ for PS1 sometime soon. Wait a second...";
	splashes_random[splash_count++] = "What R2 do? BEEP BEEP BEEP";
	splashes_random[splash_count++] = "48x36 boards DO NOT GIVE YOU FRAMEDROPS!!!!!!!";
	splashes_random[splash_count++] = "The Big LAG is no longer lagging?";
	splashes_random[splash_count++] = "Now with Full Episode 2!";
	splashes_random[splash_count++] = "12 is coming...";
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
	splashes_random[splash_count++] = "Sweepers gonna sweep";
	
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
	bgm[0] = title_mp3_bin;
	bgm_size[0] = title_mp3_bin_size;
	bgm_start[0] = 0;
	bgm_end[0] = sex;
	
	bgm[1] = bgm1_mp3_bin;
	bgm_size[1] = bgm1_mp3_bin_size;
	bgm_start[1] = 499;
	bgm_end[1] = 45281;
	
	bgm[2] = bgm2_mp3_bin;
	bgm_size[2] = bgm2_mp3_bin_size;
	bgm_start[2] = 7801;
	bgm_end[2] = 54617;
	
	bgm[3] = bgm3_mp3_bin;
	bgm_size[3] = bgm3_mp3_bin_size;
	bgm_start[3] = 0;
	bgm_end[3] = sex;
	
	bgm[4] = bgm4_mp3_bin;
	bgm_size[4] = bgm4_mp3_bin_size;
	bgm_start[4] = 1782;
	bgm_end[4] = 137481;
	
	bgm[5] = bgm5_mp3_bin;
	bgm_size[5] = bgm5_mp3_bin_size;
	bgm_start[5] = 0;
	bgm_end[5] = sex;
	
	bgm[6] = bgm6_mp3_bin;
	bgm_size[6] = bgm6_mp3_bin_size;
	bgm_start[6] = 13439;
	bgm_end[6] = 69080;
	
	bgm[7] = bgm7_mp3_bin;
	bgm_size[7] = bgm7_mp3_bin_size;
	bgm_start[7] = 0;
	bgm_end[7] = sex;
	
	bgm[8] = bgm8_mp3_bin;
	bgm_size[8] = bgm8_mp3_bin_size;
	bgm_start[8] = 6548;
	bgm_end[8] = 104708-10;
	
	bgm[9] = ocean_mp3_bin;
	bgm_size[9] = ocean_mp3_bin_size;
	bgm_start[9] = 0;
	bgm_end[9] = sex;
	
	bgm[10] = boss9_intro_mp3_bin;
	bgm_size[10] = boss9_intro_mp3_bin_size;
	bgm_start[10] = 29632;
	bgm_end[10] = 33763;
	
	bgm[11] = boss9_mp3_bin;
	bgm_size[11] = boss9_mp3_bin_size;
	bgm_start[11] = 10995;
	bgm_end[11] = 100527;
	
	bgm[12] = boss10_intro_mp3_bin;
	bgm_size[12] = boss10_intro_mp3_bin_size;
	bgm_start[12] = 23725;
	bgm_end[12] = 26893;
	
	bgm[13] = boss10_mp3_bin;
	bgm_size[13] = boss10_mp3_bin_size;
	bgm_start[13] = 0;
	bgm_end[13] = sex;
	
	bgm[15] = boss11_intro_mp3_bin;
	bgm_size[15] = boss11_intro_mp3_bin_size;
	bgm_start[15] = 42881;
	bgm_end[15] = 70250-10;
	
	bgm[16] = boss11_mp3_bin;
	bgm_size[16] = boss11_mp3_bin_size;
	bgm_start[16] = 3438;
	bgm_end[16] = 127540-10;
	
	bgm[17] = bgmrs_mp3_bin;
	bgm_size[17] = bgmrs_mp3_bin_size;
	bgm_start[17] = 44300;
	bgm_end[17] = 103380;
	
	bgm[51]=Ags1_mp3_bin;bgm_size[51]=Ags1_mp3_bin_size;bgm_start[51]=0;bgm_end[51]=sex;
	bgm[52]=Ags2_mp3_bin;bgm_size[52]=Ags2_mp3_bin_size;bgm_start[52]=0;bgm_end[52]=sex;
	bgm[53]=Ags3_mp3_bin;bgm_size[53]=Ags3_mp3_bin_size;bgm_start[53]=0;bgm_end[53]=sex;
	bgm[54]=Ags4_mp3_bin;bgm_size[54]=Ags4_mp3_bin_size;bgm_start[54]=0;bgm_end[54]=sex;
	bgm[55]=Ags5_mp3_bin;bgm_size[55]=Ags5_mp3_bin_size;bgm_start[55]=0;bgm_end[55]=sex;
	bgm[56]=Ags6_mp3_bin;bgm_size[56]=Ags6_mp3_bin_size;bgm_start[56]=0;bgm_end[56]=sex;
	bgm[57]=Ags7_mp3_bin;bgm_size[57]=Ags7_mp3_bin_size;bgm_start[57]=0;bgm_end[57]=sex;
	bgm[58]=Ags8_mp3_bin;bgm_size[58]=Ags8_mp3_bin_size;bgm_start[58]=0;bgm_end[58]=sex;
	bgm[59]=Ags9_mp3_bin;bgm_size[59]=Ags9_mp3_bin_size;bgm_start[59]=0;bgm_end[59]=sex;
	bgm[60]=Ags10_mp3_bin;bgm_size[60]=Ags10_mp3_bin_size;bgm_start[60]=0;bgm_end[60]=sex;
	bgm[61]=Ags11_mp3_bin;bgm_size[61]=Ags11_mp3_bin_size;bgm_start[61]=0;bgm_end[61]=sex;
	bgm[62]=Agsrs_mp3_bin;bgm_size[62]=Agsrs_mp3_bin_size;bgm_start[62]=0;bgm_end[62]=sex;
	
	bgm[99] = (unsigned char *)""; // custom music
	bgm_size[99] = 0;
	bgm_start[99] = 0;
	bgm_end[99] = sex;
	
	bgm[100] = (unsigned char *)""; // bad apple!!
	bgm_size[100] = 0;
	bgm_start[100] = 0;
	bgm_end[100] = sex;
	
	bgm[101] = win1_mp3_bin;
	bgm_size[101] = win1_mp3_bin_size;
	bgm_start[101] = 0;
	bgm_end[101] = sex;
	
	bgm[102] = win2_mp3_bin;
	bgm_size[102] = win2_mp3_bin_size;
	bgm_start[102] = 0;
	bgm_end[102] = sex;
	
	bgm[103] = win3_mp3_bin;
	bgm_size[103] = win3_mp3_bin_size;
	bgm_start[103] = 0;
	bgm_end[103] = sex;
	
	bgm[104] = win4_mp3_bin;
	bgm_size[104] = win4_mp3_bin_size;
	bgm_start[104] = 0;
	bgm_end[104] = sex;
	
	bgm[105] = win5_ogg_bin;
	bgm_size[105] = win5_ogg_bin_size;
	bgm_start[105] = 0;
	bgm_end[105] = sex;
	
	bgm[106] = win6_ogg_bin;
	bgm_size[106] = win6_ogg_bin_size;
	bgm_start[106] = 0;
	bgm_end[106] = sex;
	
	bgm[107] = win7_ogg_bin;
	bgm_size[107] = win7_ogg_bin_size;
	bgm_start[107] = 0;
	bgm_end[107] = sex;
	
	bgm[108] = win8_ogg_bin;
	bgm_size[108] = win8_ogg_bin_size;
	bgm_start[108] = 0;
	bgm_end[108] = sex;
	
	bgm[109] = boss9_win_ogg_bin;
	bgm_size[109] = boss9_win_ogg_bin_size;
	bgm_start[109] = 0;
	bgm_end[109] = sex;
	
	bgm[110] = boss10_win_ogg_bin;
	bgm_size[110] = boss10_win_ogg_bin_size;
	bgm_start[110] = 0;
	bgm_end[110] = sex;
	
	bgm[111] = boss11_win_ogg_bin;
	bgm_size[111] = boss11_win_ogg_bin_size;
	bgm_start[111] = 0;
	bgm_end[111] = sex;
	
	bgm[199] = (unsigned char *)"";
	bgm_size[199] = 0;
	bgm_start[199] = 0;
	bgm_end[199] = sex;
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
	LoadSFX(&sfx_flag, flag_ogg_bin, flag_ogg_bin_size);
	LoadSFX(&sfx_unflag, unflag_mp3_bin, unflag_mp3_bin_size);
	LoadSFX(&sfx_9move, nine_move_ogg_bin, nine_move_ogg_bin_size);
	LoadSFX(&sfx_9stop, nine_stop_ogg_bin, nine_stop_ogg_bin_size);
	LoadSFX(&sfx_10bump, ten_bump_ogg_bin, ten_bump_ogg_bin_size);
	LoadSFX(&sfx_10reset, ten_reset_ogg_bin, ten_reset_ogg_bin_size);
	LoadSFX(&sfx_mine, game_mine_ogg_bin, game_mine_ogg_bin_size);
	LoadSFX(&sfx_explosion, game_explosion_ogg_bin, game_explosion_ogg_bin_size);
	LoadSFX(&sfx_11open, eleven_open_ogg_bin, eleven_open_ogg_bin_size);
	LoadSFX(&sfx_11bonk, eleven_bonk_ogg_bin, eleven_bonk_ogg_bin_size);
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
	FILE *fp;
	if(id==99 || id==199 || id==100) fp = fopen((char *)bgm[id], "rb");
    else fp = (FILE *)mem_open((char *)bgm[id], bgm_size[id]);
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
}

void Load_PNG()
{
	pngLoadFromBuffer(captain_sheet_png_bin, captain_sheet_png_bin_size, &spritesheet[0]);
	pngLoadFromBuffer(tiles_sheet_png_bin, tiles_sheet_png_bin_size, &spritesheet[1]);
	pngLoadFromBuffer(ui_sheet_png_bin, ui_sheet_png_bin_size, &spritesheet[2]);
	pngLoadFromBuffer(boss_sheet_png_bin, boss_sheet_png_bin_size, &spritesheet[3]);
	pngLoadFromBuffer(boss1_sheet_png_bin, boss1_sheet_png_bin_size, &spritesheet[4]);
	pngLoadFromBuffer(dualboss_sheet_png_bin, dualboss_sheet_png_bin_size, &spritesheet[5]);
	pngLoadFromBuffer(cursed_sheet_png_bin, cursed_sheet_png_bin_size, &spritesheet[6]);
	pngLoadFromBuffer(background_sheet_png_bin, background_sheet_png_bin_size, &spritesheet[7]);
	pngLoadFromBuffer(trust_me_bro_png_bin, trust_me_bro_png_bin_size, &spritesheet[8]);
	pngLoadFromBuffer(menu_sheet_png_bin, menu_sheet_png_bin_size, &spritesheet[9]);
	pngLoadFromBuffer(coin_png_bin, coin_png_bin_size, &spritesheet[10]);
	pngLoadFromBuffer(buttons_png_bin, buttons_png_bin_size, &spritesheet[11]);
	pngLoadFromBuffer(hats_sheet_png_bin, hats_sheet_png_bin_size, &spritesheet[12]);
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