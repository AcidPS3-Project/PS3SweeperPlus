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

float ship_x = -40.0f;
float ship_y = -22.0f;
int ship_r = 0;
bool ship_a = false;

bool show_shit = false;

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
			langStr[2] = "Safe Tile Guarantee";
			langStr[3] = "3x3 Guarantee";
			langStr[4] = "Warning! This game uses auto-save feature.";
			langStr[5] = "When you see the \"Saving...\" text, it saves your data.";
			langStr[6] = "Please, do NOT turn off your PlayStation 3 system while the \"Saving...\"";
			langStr[7] = "text is on-screen.";
			langStr[8] = "I understand.";
			langStr[9] = "FINAL RANK";
			langStr[10] = "Thanks for Playing!";
			langStr[11] = "Press Start";
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
			langStr[40] = "s";
			langStr[41] = "Next Level";
			langStr[42] = "Save and Quit";
			langStr[43] = "Reset game";
			langStr[44] = "Settings";
			langStr[45] = "1x1 Guarantee";
			langStr[46] = "No Guarantee";
			langStr[47] = "Episode 4 is currently in works.";
			langStr[48] = "Error!\nFile not found in:\n/\"dev_usb000/SWEEPING/captain.png\"!";
			langStr[49] = "Error! Image's width or height are WAY above 2048 pixels.";
			langStr[50] = "Error!\nFile not found in:\n\"/dev_usb000/SWEEPING/bgm.mp3\" (or bgm.ogg)!";
			langStr[51] = "Warning!\nFile not found in:\n\"/dev_usb000/SWEEPING/win.mp3\" (or win.ogg)!\nIt is not necessary, but winning music won't play.";
			langStr[52] = "Original Minesweeper Plus game";
			langStr[53] = "Error! Image's width and height don't match!!";
			langStr[54] = "Use Custom Sweeper";
			langStr[55] = "Half-Mine Related Sprites";
			langStr[56] = "Special Thanks";
			langStr[57] = "Paused";
			langStr[58] = "Four of a Kind";
			langStr[59] = "Frozen Nine mechanics";
			langStr[60] = "Nuclear Ten Idea";
			langStr[61] = "Kiddie Pool";
			langStr[62] = "Rainy Puddle";
			langStr[63] = "Small Pond";
			langStr[64] = "Blue Lagoon";
			langStr[65] = "Lazy River";
			langStr[66] = "Great Lake";
			langStr[67] = "Open Ocean";
			langStr[68] = "Dead Sea";
			langStr[69] = "THE DEVIL'S TRIANGLE";
			langStr[70] = "Original 9 Boss Music";
			langStr[71] = "Touhou Stuff";
			langStr[72] = "Alien Captain's Hat";
			langStr[73] = "Frozen Captain Sprite";
			langStr[74] = "THE FREEZING CIRCLE";
			langStr[75] = "THE NUCLEAR WASTE";
			langStr[76] = "THE GENOCIDE GARDEN";
			langStr[77] = "Total Time";
			langStr[78] = "Coins Earned";
			langStr[79] = "Mistakes Made";
			langStr[80] = "The Shallow End";
			langStr[81] = "Fountain Flows";
			langStr[82] = "Silent Marsh";
			langStr[83] = "Murky Swamp";
			langStr[84] = "Crazy Cove";
			langStr[85] = "Raging Rapids";
			langStr[86] = "Gulf of Misery";
			langStr[87] = "Black Water";
			langStr[88] = "The Deep End";
			langStr[89] = "THE DEVIL'S PASSAGE";
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
			langStr[2] = "Гарантия на безопасную клетку";
			langStr[3] = "3x3 Гарантия";
			langStr[4] = "Внимание! У этой игры есть функция авто-сохранения.";
			langStr[5] = "Когда вы видите текст \"Сохраняю...\", оно сохраняет игру.";
			langStr[6] = "Пожалуйста, не выключайте ваш PlayStation 3 когда текст";
			langStr[7] = "\"Сохраняю...\" находится на экране.";
			langStr[8] = "Хорошо.";
			langStr[9] = "ФИНАЛЬНАЯ ОЦЕНКА";
			langStr[10] = "Спасибо за Игру!";
			langStr[11] = "Нажмите Start";
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
			langStr[40] = "сек";
			langStr[41] = "Следующий уровень";
			langStr[42] = "Сохранить и выйти";
			langStr[43] = "Сбросить игру";
			langStr[44] = "Настройки";
			langStr[45] = "1x1 Гарантия";
			langStr[46] = "Без Гарантий";
			langStr[47] = "Четвертый эпизод находится в разработке.";
			langStr[48] = "Ошибка!\nФайл не найден:\n/\"dev_usb000/SWEEPING/captain.png\"!";
			langStr[49] = "Ошибка! Ширина или длина изображения превышают 2048 пикселей.";
			langStr[50] = "Ошибка!\nФайл не найден:\n\"/dev_usb000/SWEEPING/bgm.mp3\" (или bgm.ogg)!";
			langStr[51] = "Предупреждение!\nФайл не найден:\n\"/dev_usb000/SWEEPING/win.mp3\" (или win.ogg)!\nЭто не особо важно, но возможно звук победы не заиграет.";
			langStr[52] = "Оригинальная игра Сапер Плюс";
			langStr[53] = "Ошибка! Ширина и длина изображения не совпадают!!";
			langStr[54] = "Использовать своего сапера";
			langStr[55] = "Спрайты связанные с полу-минами";
			langStr[56] = "СПЕЦИАЛЬНАЯ БЛАГОДАРНОСТЬ";
			langStr[57] = "Пауза";
			langStr[58] = "Квартет Смерти";
			langStr[59] = "Механики Ледяной Девятки";
			langStr[60] = "Идея Радиактивной Десятки";
			langStr[61] = "Детский Бассейн";
			langStr[62] = "Дождевая Лужа";
			langStr[63] = "Малый Пруд";
			langStr[64] = "Синее Море";
			langStr[65] = "Ленивая Река";
			langStr[66] = "Большое Озеро";
			langStr[67] = "Открытый Океан";
			langStr[68] = "Море Мертвеца";
			langStr[69] = "ДЬЯВОЛЬСКИЙ ТРЕУГОЛЬНИК";
			langStr[70] = "Оригинальная Музыка Девятки";
			langStr[71] = "Все связанное с Touhou";
			langStr[72] = "Шляпа Инопланетного Капитана";
			langStr[73] = "Спрайт Замороженного Капитана";
			langStr[74] = "ОБМЕРЗШИЙ КРУГ";
			langStr[75] = "ЯДЕРНЫЕ ОТХОДЫ";
			langStr[76] = "САД ГЕНОЦИДА";
			langStr[77] = "Всего времени прошло";
			langStr[78] = "Заработано Бомба-коинов";
			langStr[79] = "Сделано Ошибок";
			langStr[80] = "Мелководья";
			langStr[81] = "Парящий Фонтан";
			langStr[82] = "Тихий Марш";
			langStr[83] = "Скользкое Болото";
			langStr[84] = "Безумные Подмостья";
			langStr[85] = "Быстрая Агрессия";
			langStr[86] = "Дно Страданий";
			langStr[87] = "Черная вода";
			langStr[88] = "Глубочайший Конец";
			langStr[89] = "ДЬЯВОЛЬСКАЯ ПРОПАСТЬ";
			break;
		}
		case 2: // Қазақ Тілі
		{
			// Cheat-sheet! heheh
			// ӘәҒғІіҚқҢңӨөҰұҮүҺһ
			
			menu_title[0] = "Приключения Сапера";
			menu_desc[0] = "Проверь свои навыки сапера в усложняющей последовательности уровней сапера.";
			menu_title[1] = "Сенің ойнасы";
			menu_desc[1] = "Создавайте свой уровень сапера и разминируйте мины как душе угодно!";
			menu_title[2] = "Испытания";
			menu_desc[2] = "Насладитесь выбором испытаний которые проверят ваши знания сапера.";
			menu_title[3] = "Гардероб";
			menu_desc[3] = "Измените вид капитана и пополните запасы.";
			
			langStr[0] = "...";
			langStr[1] = "Сақтамың...";
			langStr[2] = "Гарантия на безопасную клетку";
			langStr[3] = "3x3 Гарантия";
			langStr[4] = "Назар аударыңыз! У этой игры есть функция авто-сохранения.";
			langStr[5] = "Когда вы видите текст \"Сақтамың...\", оно сохраняет игру.";
			langStr[6] = "Пожалуйста, не выключайте ваш PlayStation 3 когда текст";
			langStr[7] = "\"Сақтамың...\" находится на экране.";
			langStr[8] = "Жарайды.";
			langStr[9] = "ФИНАЛЬНАЯ ОЦЕНКА";
			langStr[10] = "Спасибо за Игру!";
			langStr[11] = "Нажмите Start";
			langStr[12] = "Здесь нечего загружать.";
			langStr[13] = "Бастау";
			langStr[14] = "Алғыс";
			langStr[15] = "Продолжить";
			langStr[16] = "Басты меню";
			langStr[17] = "Меню приключений";
			langStr[18] = "Ширина";
			langStr[19] = "Высота";
			langStr[20] = "Кол-во мин";
			langStr[21] = "Кол-во двойных мин";
			langStr[22] = "Кол-во полу-мин";
			langStr[23] = "Кол-во анти-мин";
			langStr[24] = "ЭПИЗОД";
			langStr[25] = "Музыка стильді";
			langStr[26] = "СЕНІКІ";
			langStr[27] = "Сенің ойнасы меню";
			langStr[28] = "Сенің музыкасы аяқтау";
			langStr[29] = "Сенің музыкасы бастау";
			langStr[30] = "Өң стильді";
			langStr[31] = "Сенің ойнасы бастау";
			langStr[32] = "Артқа";
			langStr[33] = "Үлкені";
			langStr[34] = "Жаман Алма!! (Потребуется Адд-он)";
			langStr[35] = "Меню испытаний";
			langStr[36] = "Это меню находится в разработке :)";
			langStr[37] = "ОЙНА БОЛДЫ";
			langStr[38] = "КЕРЕМЕТ!";
			langStr[39] = "Бомба-коинлер";
			langStr[40] = "сек";
			langStr[41] = "Келесі ?????";
			langStr[42] = "Сақтау және Шығу";
			langStr[43] = "Сбросить игру";
			langStr[44] = "Параметрлер";
			langStr[45] = "1x1 Гарантия";
			langStr[46] = "Гарантия Жоқ";
			langStr[47] = "Четвертый эпизод находится в разработке.";
			langStr[48] = "Ошибка!\nФайл не найден:\n/\"dev_usb000/SWEEPING/captain.png\"!";
			langStr[49] = "Ошибка! Ширина или длина изображения превышают 2048 пикселей.";
			langStr[50] = "Ошибка!\nФайл не найден:\n\"/dev_usb000/SWEEPING/bgm.mp3\" (или bgm.ogg)!";
			langStr[51] = "Предупреждение!\nФайл не найден:\n\"/dev_usb000/SWEEPING/win.mp3\" (или win.ogg)!\nЭто не особо важно, но возможно звук победы не заиграет.";
			langStr[52] = "Оригинальная игра Сапер Плюс";
			langStr[53] = "Ошибка! Ширина и длина изображения не совпадают!!";
			langStr[54] = "Использовать своего сапера";
			langStr[55] = "Спрайты связанные с полу-минами";
			langStr[56] = "ҮЛКЕН РАХМЕТ";
			langStr[57] = "Пауза";
			langStr[58] = "Квартет Смерти";
			langStr[59] = "Механики Ледяной Девятки";
			langStr[60] = "Идея Радиактивной Десятки";
			langStr[61] = "Балаларың Бассейн";
			langStr[62] = "Дождевая Лужа";
			langStr[63] = "Малый Пруд";
			langStr[64] = "Көктің Лагуна";
			langStr[65] = "Ерінші Өзен";
			langStr[66] = "Үлкен Қөл";
			langStr[67] = "Открытый Океан";
			langStr[68] = "Море Мертвеца";
			langStr[69] = "ДЬЯВОЛЬСКИЙ ТРЕУГОЛЬНИК";
			langStr[70] = "Оригинальная Музыка Девятки";
			langStr[71] = "Все связанное с Touhou";
			langStr[72] = "Шляпа Инопланетного Капитана";
			langStr[73] = "Спрайт Замороженного Капитана";
			langStr[74] = "ОБМЕРЗШИЙ КРУГ";
			langStr[75] = "ЯДЕРНЫЕ ОТХОДЫ";
			langStr[76] = "САД ГЕНОЦИДА";
			langStr[77] = "Всего времени прошло";
			langStr[78] = "Заработано Бомба-коинов";
			langStr[79] = "Сделано Ошибок";
			langStr[80] = "Мелководья";
			langStr[81] = "Парящий Фонтан";
			langStr[82] = "Тихий Марш";
			langStr[83] = "Скользкое Болото";
			langStr[84] = "Безумные Подмостья";
			langStr[85] = "Быстрая Агрессия";
			langStr[86] = "Дно Страданий";
			langStr[87] = "Қара су";
			langStr[88] = "Глубочайший Конец";
			langStr[89] = "ДЬЯВОЛЬСКАЯ ПРОПАСТЬ";
			break;
		}
	}
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
	splashes_random[splash_count++] = "up up down down left right left right circle cross start.\n   oh wait, it doesnt work";
	splashes_random[splash_count++] = "why do i stay all night for this";
	splashes_random[splash_count++] = "I wanna try making MS+ for PS1 sometime soon. Wait a second...";
	splashes_random[splash_count++] = "What R2 do? BEEP BEEP BEEP";
	splashes_random[splash_count++] = "48x36 boards DO NOT GIVE YOU FRAMEDROPS!!!!!!!";
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
	splashes_random[splash_count++] = "Don't forget to feed me with a spoon!";
	//splashes_random[splash_count++] = "Hi, Xbox Original :)";
	//splashes_random[splash_count++] = "I wonder what I can do for Mac OS 9 tho...";
	splashes_random[splash_count++] = "Cutscenes?";
	splashes_random[splash_count++] = "v0.4 is here. Prepare thyself!!!";
	splashes_random[splash_count++] = "BUGS. I HATE BUGS!!!!";
	splashes_random[splash_count++] = "Hate Lesbian Space Princess. Love PS3Sweeper+.";
	splashes_random[splash_count++] = "Ay, mate! Don't get snowgraved, heh!";
	splashes_random[splash_count++] = "Frozen Nine is not related to Deltarune.";
	splashes_random[splash_count++] = "He's gonna take you back to the past.";
	splashes_random[splash_count++] = "Does not contain too much if else stuff!";
	splashes_random[splash_count++] = "One of the First PS3 games that support Kazakh Language!";
	splashes_random[splash_count++] = "";
	
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
		bgm_start[z] = 0;
		bgm_end[z] = sex;
	}
	
	bgm[0] = title_bin;
	bgm_size[0] = title_bin_size;
	
	bgm[1] = bgm1_bin;
	bgm_size[1] = bgm1_bin_size;
	bgm_start[1] = 499;
	bgm_end[1] = 45281;
	
	bgm[2] = bgm2_bin;
	bgm_size[2] = bgm2_bin_size;
	bgm_start[2] = 7801;
	bgm_end[2] = 54617;
	
	bgm[3] = bgm3_bin;
	bgm_size[3] = bgm3_bin_size;
	bgm_start[3] = 0;
	bgm_end[3] = sex;
	
	bgm[4] = bgm4_bin;
	bgm_size[4] = bgm4_bin_size;
	bgm_start[4] = 1782;
	bgm_end[4] = 137481;
	
	bgm[5] = bgm5_bin;
	bgm_size[5] = bgm5_bin_size;
	
	bgm[6] = bgm6_bin;
	bgm_size[6] = bgm6_bin_size;
	bgm_start[6] = 13439;
	bgm_end[6] = 69080;
	
	bgm[7] = bgm7_bin;
	bgm_size[7] = bgm7_bin_size;
	
	bgm[8] = bgm8_bin;
	bgm_size[8] = bgm8_bin_size;
	bgm_start[8] = 6548;
	bgm_end[8] = 104708-10;
	
	bgm[9] = ocean_bin;
	bgm_size[9] = ocean_bin_size;
	
	bgm[10] = boss9_intro_bin;
	bgm_size[10] = boss9_intro_bin_size;
	bgm_start[10] = 29632;
	bgm_end[10] = 33763;
	
	bgm[11] = boss9_bin;
	bgm_size[11] = boss9_bin_size;
	bgm_start[11] = 10995;
	bgm_end[11] = 100527;
	
	bgm[12] = boss10_intro_bin;
	bgm_size[12] = boss10_intro_bin_size;
	bgm_start[12] = 23725;
	bgm_end[12] = 26893;
	
	bgm[13] = boss10_bin;
	bgm_size[13] = boss10_bin_size;
	
	bgm[15] = boss11_intro_bin;
	bgm_size[15] = boss11_intro_bin_size;
	bgm_start[15] = 42881;
	bgm_end[15] = 70250-10;
	
	bgm[16] = boss11_bin;
	bgm_size[16] = boss11_bin_size;
	bgm_start[16] = 3438;
	bgm_end[16] = 127540-10;
	
	bgm[17] = bgmrs_bin;
	bgm_size[17] = bgmrs_bin_size;
	bgm_start[17] = 44300;
	bgm_end[17] = 103380;
	
	bgm[18] = bgms3m_bin;
	bgm_size[18] = bgms3m_bin_size;
	bgm_start[18] = 44300;
	bgm_end[18] = 103380;
	
	bgm[19] = boss12_intro_bin;
	bgm_size[19] = boss12_intro_bin_size;
	bgm_start[19] = 23283;
	bgm_end[19] = 35027-10;
	
	bgm[20] = boss12_bin;
	bgm_size[20] = boss12_bin_size;
	
	bgm[51]=Ags1_bin;bgm_size[51]=Ags1_bin_size;
	bgm[52]=Ags2_bin;bgm_size[52]=Ags2_bin_size;
	bgm[53]=Ags3_bin;bgm_size[53]=Ags3_bin_size;
	bgm[54]=Ags4_bin;bgm_size[54]=Ags4_bin_size;
	bgm[55]=Ags5_bin;bgm_size[55]=Ags5_bin_size;
	bgm[56]=Ags6_bin;bgm_size[56]=Ags6_bin_size;
	bgm[57]=Ags7_bin;bgm_size[57]=Ags7_bin_size;
	bgm[58]=Ags8_bin;bgm_size[58]=Ags8_bin_size;
	bgm[59]=Ags9_bin;bgm_size[59]=Ags9_bin_size;
	bgm[60]=Ags10_bin;bgm_size[60]=Ags10_bin_size;
	bgm[61]=Ags11_bin;bgm_size[61]=Ags11_bin_size;
	bgm[62]=Agsrs_bin;bgm_size[62]=Agsrs_bin_size;
	bgm[63]=Agss3m_bin;bgm_size[63]=Agss3m_bin_size;
	bgm[64]=Ags12_bin;bgm_size[64]=Ags12_bin_size;
	
	bgm[99] = (unsigned char *)""; // custom music
	bgm_size[99] = 0;
	
	bgm[100] = (unsigned char *)""; // bad apple!!
	bgm_size[100] = 0;
	
	bgm[101] = win1_bin;
	bgm_size[101] = win1_bin_size;
	
	bgm[102] = win2_bin;
	bgm_size[102] = win2_bin_size;
	
	bgm[103] = win3_bin;
	bgm_size[103] = win3_bin_size;
	
	bgm[104] = win4_bin;
	bgm_size[104] = win4_bin_size;
	
	bgm[105] = win5_bin;
	bgm_size[105] = win5_bin_size;
	
	bgm[106] = win6_bin;
	bgm_size[106] = win6_bin_size;
	
	bgm[107] = win7_bin;
	bgm_size[107] = win7_bin_size;
	
	bgm[108] = win8_bin;
	bgm_size[108] = win8_bin_size;
	
	bgm[109] = boss9_win_bin;
	bgm_size[109] = boss9_win_bin_size;
	
	bgm[110] = boss10_win_bin;
	bgm_size[110] = boss10_win_bin_size;
	
	bgm[111] = boss11_win_bin;
	bgm_size[111] = boss11_win_bin_size;
	
	bgm[112] = boss12_win_bin;
	bgm_size[112] = boss12_win_bin_size;
	
	bgm[117] = winrs_bin;
	bgm_size[117] = winrs_bin_size;
	
	bgm[118] = wins3m_bin;
	bgm_size[118] = wins3m_bin_size;
	
	bgm[151]=ls1_bin;bgm_size[151]=ls1_bin_size;
	bgm[152]=ls2_bin;bgm_size[152]=ls2_bin_size;
	bgm[153]=ls3_bin;bgm_size[153]=ls3_bin_size;
	bgm[154]=ls4_bin;bgm_size[154]=ls4_bin_size;
	bgm[155]=ls5_bin;bgm_size[155]=ls5_bin_size;
	bgm[156]=ls6_bin;bgm_size[156]=ls6_bin_size;
	bgm[157]=ls7_bin;bgm_size[157]=ls7_bin_size;
	bgm[158]=ls8_bin;bgm_size[158]=ls8_bin_size;
	bgm[159]=ls9_bin;bgm_size[159]=ls9_bin_size;
	bgm[160]=ls10_bin;bgm_size[160]=ls10_bin_size;
	bgm[161]=ls11_bin;bgm_size[161]=ls11_bin_size;
	bgm[162]=lsrs_bin;bgm_size[162]=lsrs_bin_size;
	bgm[163]=lss3m_bin;bgm_size[163]=lss3m_bin_size;
	bgm[164]=ls12_bin;bgm_size[164]=ls12_bin_size;
	
	bgm[191] = untitled_bin;
	bgm_size[191] = untitled_bin_size;
	
	bgm[192] = credits_bin;
	bgm_size[192] = credits_bin_size;
	
	bgm[193] = ending_bin;
	bgm_size[193] = ending_bin_size;
	
	bgm[194] = fourofakind_intro_bin;
	bgm_size[194] = fourofakind_intro_bin_size;
	
	bgm[195] = fourofakind_bin;
	bgm_size[195] = fourofakind_bin_size;
	
	bgm[197] = shop_bin;
	bgm_size[197] = shop_bin_size;
	
	bgm[198] = logo_bin;
	bgm_size[198] = logo_bin_size;
	
	bgm[199] = (unsigned char *)""; // custom win music
	bgm_size[199] = 0;
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
	
	sfx->voice = voice;
}

void StopSFX(SoundEffect *sfx)
{
    if (sfx->voice >= 1)
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