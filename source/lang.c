#include "lang.h"
#include "libraries.h"

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
			
			langStr[74] = "There is a boss battle in this game.\nThe battle works the same as normal Minesweeper, but with some key distinctions: you won't know the number of mines hidden on the grid, you have limited time to complete the level, and you won't be able to use the sonar or pause the game. Clicking on a mine will make the time run out faster. To win, you must press TRIANGLE ONLY AFTER revealing all the tiles that don't contain mines. If the timer hits 0, or you press TRIANGLE before revealing the entire grid, it's game over.";
			langStr[75] = "There is another boss battle in this game.\nAs soon as the game starts, this boss will start counting down from 10. If it counts all the way down to 0, it's game over. Press SQUARE reset the countdown back to 10, but be careful. The boss will move faster and faster every time you press. Clicking on a mine will cause the boss to count down more quickly. To win, reveal the entire grid before the boss is able to finish its countdown.";
			langStr[76] = "There is yet another boss battle in this game.\nThe top left number now shows how many more tiles are allowed to be flagged before it's game over. The boss will teleport around the grid placing flags. If you can click the boss before it materializes, you can prevent it from placing a flag, but don't click the boss when its electrical coils are active or you will receive a penalty. Unflagging tiles before the game starts will also result in a penalty. Furthermore, the mines in this boss's grid are pressure mines. Do not remove flags from tiles that contain pressure mines. The boss will place many flags in quick succession if a mine is revealed. To win, reveal the entire grid before the flag counter reaches 0, or before no more flags can be placed on the grid.";
			
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
			
			langStr[90] = "Welcome to \"Acid's Solid Shop\"!";
			langStr[91] = "My shop is not solid at all, I just wanted to make a funny abbreviature, heh.";
			langStr[92] = "Yes, the dev being in the game is cliche, I know. At least I don't alter the game's lore.";
			langStr[93] = "So much weird numbers. I'm not ready for them yet.";
			langStr[94] = "This is my second shop I ever had, heh! I don't actually remember why.";
			langStr[95] = "Hey, if you like this game, make sure you leave a good rating in my online store! Wait does it have ratings yet?..";
			langStr[96] = "Who will you be tonight? That's the question.";
			langStr[97] = "Why do I feel tired every time?";
			langStr[98] = "I am basically alone here, finally someone cared to pay me a visit.";
			langStr[99] = "In some free time I like to listen to some TLoZ music. My favorite one is definitely Gerudo Valley from OoT.";
			langStr[100] = "There is some crazy stuff going on. I heard some noises when I was sleeping, some random beeps and explosions. I hope those sounds won't go close to me...";
			langStr[101] = "See you soon!";
			
			langStr[102] = "Something has been unlocked.";
			
			langStr[103] = "This is the Frozen Nine, heavily differs from Devil's Nine.\nIt is way faster than it's original counterpart, and it summons the iceberg every 10 seconds. After 10 seconds, the iceberg melts and summons another. The iceberg blocks the 3x3 area on it's location. Good luck.";
			langStr[104] = "This is the Nuclear Ten.\nAt first, behaves just like it's original, but when pressing SQUARE, it may not reset it's timer, instead it dodges the attack. Be careful! Every time you press SQUARE, it now pollutes the air with toxic acid fumes, making it very harder to see (and maybe breathe). If it fully blocks the view, you suffocate and it's game over.";
			langStr[105] = "This is the Menacing Eleven.\nAs for now, it's just a re-skin. More mechanics might come in later.";
			langStr[106] = "Additional Note:";
			langStr[107] = "Press START to continue.";
			langStr[108] = "Press TRIANGLE to not show it again.";
			langStr[109] = "PS3SWEEPER+";
			langStr[110] = "INSPIRED BY MINESWEEPER PLUS";
			
			langStr[111] = "Cherry Pool";
			langStr[112] = "Kourindou Pond";
			langStr[113] = "Higan Lagoon";
			langStr[114] = "Sanzo River";
			langStr[115] = "Nameless Shipwreck";
			langStr[116] = "Misty Lake";
			langStr[117] = "Youkai Ocean";
			langStr[118] = "Scarlet Dead Sea";
			langStr[119] = "THE FREEZING CIRCLE";
			
			langStr[120] = "Icy Puddle";
			langStr[121] = "Frozen Fountain";
			langStr[122] = "Silent Marsh";
			langStr[123] = "Murky Swamp";
			langStr[124] = "Crazy Cove";
			langStr[125] = "Raging Rapids";
			langStr[126] = "Gulf of Misery";
			langStr[127] = "Black Water";
			langStr[128] = "The Deep End";
			langStr[129] = "THE NUCLEAR WASTE";
			
			langStr[130] = "Boss Options";
			langStr[131] = "ENABLE";
			langStr[132] = "True";
			langStr[133] = "False";

			langStr[134] = "There is again, yet another boss.\nYou don't know, how many mines in this board. Also you don't know how much time passed, but it won't matter anyway. It can stop the time to mess with your current position. You might not notice it, but it may flash before time was stopped. The time stop will last less, than a second. It may also shoot several projectiles after the time stop. Hitting it with cursor explodes the projectile and gives you penalty. After each penalty, time will freeze more often, until the time completely stoppes forever, and then it's game over. To win, complete the board until it ends the time.";
			langStr[135] = "This is the Killer XII.\nAs for now, it's just a re-skin. More mechanics might come in later.";
			
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
			
			langStr[74] = "Здесь имеется битва с боссом.\nБитва работает также, как в обычном сапере, только с усложнениями: ты не знаешь сколько здесь мин, время ограничено, и ты не можешь использовать радар или паузу (хотя здесь радара пока нет ЛОЛ). Нажатие на мину ускоряет таймер. Чтобы выиграть, нажмите на ТРЕУГОЛЬНИК ТОЛЬКО после того, как все пустые клетки открыты. Истечение времени, или же нажатие на босса до того, как все пустые клетки открыты, приводят к окончанию игры.";
			langStr[75] = "Здесь имеется еще одна битва с боссом.\nКак только вы начнете, пойдет обратный отсчет от 10. Если оно опустится до нуля, игра окончена. Нажмите на КВАДРАТ, чтобы сбросить отсчет, НО будьте аккуратны. Босс от этого будет двигаться быстрее. Нажатие на мину ускорит обратный отсчет. Чтобы выиграть, откройте все пустые клетки до того, как истечет обратный отсчет.";
			langStr[76] = "Здесь имеется еще одна очередная битва с боссом.\nЛевая верхняя панель теперь показывает сколько флажков можно поставить перед окончанием игры. Босс будет телепортироваться на поле для установки флагов. Если ты нажмешь на босса до установки флажка, ты можешь предотвратить установку флажка, но не нажимай на босса когда электрические антенны активированы (а когда они собственно будут активированы?) или ты получишь по ебальнику. Удаление флагов до начала игры также приведет к тому, что ты получишь по ебальнику. Так же, все мины в этом поле теперь нажимные. НЕ УБИРАЙТЕ ФЛАГИ С НАЖИМНЫХ МИН. Босс будет ставить много флажков если мина будет обнаружена. Чтобы выиграть, откройте все пустые клетки до того, как количество флагов станет 0, или до того, как некуда будет поставить флажки.";
			
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
			
			langStr[90] = "Добро пожаловать в \"Acid's Solid Shop\"!";
			langStr[91] = "Эта аббревиатура явно что-то значит, только вот я до сих пор не пойму что.";
			langStr[92] = "Да да да, разработчик в игре - клише. Хотя-бы я никак не влияю на сюжет.";
			langStr[93] = "Очень много странных чисел и цифр... Я к ним еще не готов.";
			langStr[94] = "Это мой второй магазинчик. Не пойму, для чего.";
			langStr[95] = "Хей, если тебе нравится игра, то почему бы не оставить отзыв? Стой, разве такое возможно?";
			langStr[96] = "Кем ты будешь сегодня? Кароче перевод провалился увы. Это главная тема Гадкий Я 4.";
			langStr[97] = "Чего-то я часто бываю уставшим.";
			langStr[98] = "По сути здесь я один, хотя-бы не помру сейчас от скуки.";
			langStr[99] = "В свободное время предпочитаю слушать ОСТ Зельды, особенно Gerudo Valley с Окарины Времени.";
			langStr[100] = "Здесь кароче какие-то странности происходят. Я слышал непонятные пики и взрывы. Надеюсь эти звуки не настигнут меня...";
			langStr[101] = "Еще увидимся!";
			
			langStr[102] = "Что-то разблокировалось.";
			
			langStr[103] = "Это Ледяная Девятка, сильно отличается от Дьявольской Девятки.\nОна призывает айсберги каждые 10 секунд. По истечении 10 секунд айсберг тает, и появляется новая, но в другом месте. Айсберг блокирует определенную территорию 3 на 3. Удачи.";
			langStr[104] = "Это Ядерная Десятка.\nAt first, behaves just like it's original, but when pressing SQUARE, it may not reset it's timer, instead it dodges the attack. Be careful! Every time you press SQUARE, it now pollutes the air with toxic acid fumes, making it very harder to see (and maybe breathe). If it fully blocks the view, you suffocate and it's game over.";
			langStr[105] = "Это Menacing Eleven.\nAs for now, it's just a re-skin. More mechanics might come in later.";
			langStr[106] = "Заметка:";
			langStr[107] = "Нажмите на START чтобы продолжить.";
			langStr[108] = "Нажмите на TRIANGLE чтобы не показывать снова.";
			langStr[109] = "ПС3САПЕР+";
			langStr[110] = "ВДОХНОВЛЕНО САПЕРОМ ПЛЮС";
			
			langStr[111] = "Вишневый Бассейн";
			langStr[112] = "Пруд Коуриндоу";
			langStr[113] = "Хиган Лагуна";
			langStr[114] = "Река Санзо";
			langStr[115] = "Безымянный Корабль";
			langStr[116] = "Туманное Озеро";
			langStr[117] = "Океан Йокаев";
			langStr[118] = "Багрянное Море Мертвяца";
			langStr[119] = "ОБМОРОЖЕННЫЙ КРУГ";
			
			langStr[120] = "Ледяная Лужа";
			langStr[121] = "Замороженный Фонтан";
			langStr[122] = "Тихий Марш";
			langStr[123] = "Скользкое Болото";
			langStr[124] = "Безумные Подмостья";
			langStr[125] = "Raging Rapids";
			langStr[126] = "Дно Страданий";
			langStr[127] = "Черное  Море";
			langStr[128] = "Глубочайший Конец";
			langStr[129] = "РАДИОАКТИВНЫЕ ОТХОДЫ";
			
			langStr[130] = "Опции Боссов";
			langStr[131] = "ВКЛЮЧИТЬ";
			langStr[132] = "Да";
			langStr[133] = "Нет";
			
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
			
			langStr[74] = "Здесь имеется битва с боссом.\nБитва работает также, как в обычном сапере, только с усложнениями: ты не знаешь сколько здесь мин, время ограничено, и ты не можешь использовать радар или паузу (хотя здесь радара пока нет ЛОЛ). Нажатие на мину ускоряет таймер. Чтобы выиграть, нажмите на ТРЕУГОЛЬНИК ТОЛЬКО после того, как все пустые клетки открыты. Истечение времени, или же нажатие на босса до того, как все пустые клетки открыты, приводят к окончанию игры.";
			langStr[75] = "Здесь имеется еще одна битва с боссом.\nКак только вы начнете, пойдет обратный отсчет от 10. Если оно опустится до нуля, игра окончена. Нажмите на КВАДРАТ, чтобы сбросить отсчет, НО будьте аккуратны. Босс от этого будет двигаться быстрее. Нажатие на мину ускорит обратный отсчет. Чтобы выиграть, откройте все пустые клетки до того, как истечет обратный отсчет.";
			langStr[76] = "Здесь имеется еще одна очередная битва с боссом.\nЛевая верхняя панель теперь показывает сколько флажков можно поставить перед окончанием игры. Босс будет телепортироваться на поле для установки флагов. Если ты нажмешь на босса до установки флажка, ты можешь предотвратить установку флажка, но не нажимай на босса когда электрические антенны активированы (а когда они собственно будут активированы?) или ты получишь по ебальнику. Удаление флагов до начала игры также приведет к тому, что ты получишь по ебальнику. Так же, все мины в этом поле теперь нажимные. НЕ УБИРАЙТЕ ФЛАГИ С НАЖИМНЫХ МИН. Босс будет ставить много флажков если мина будет обнаружена. Чтобы выиграть, откройте все пустые клетки до того, как количество флагов станет 0, или до того, как некуда будет поставить флажки.";
			
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
			
			langStr[90] = "\"Acid's Solid Shop\" қош келдіңіз!";
			langStr[91] = "Эта аббревиатура явно что-то значит, только вот я до сих пор не пойму что.";
			langStr[92] = "Да да да, разработчик в игре - клише. Хотя-бы я никак не влияю на сюжет.";
			langStr[93] = "Очень много странных чисел и цифр... Я к ним еще не готов.";
			langStr[94] = "Это мой второй магазинчик. Не пойму, для чего.";
			langStr[95] = "Хей, если тебе нравится игра, то почему бы не оставить отзыв? Стой, разве такое возможно?";
			langStr[96] = "Кем ты будешь сегодня? Кароче перевод провалился увы. Это главная тема Гадкий Я 4.";
			langStr[97] = "Чего-то я часто бываю уставшим.";
			langStr[98] = "По сути здесь я один, хотя-бы не помру сейчас от скуки.";
			langStr[99] = "В свободное время предпочитаю слушать ОСТ Зельды, особенно Gerudo Valley с Окарины Времени.";
			langStr[100] = "Здесь кароче какие-то странности происходят. Я слышал непонятные пики и взрывы. Надеюсь эти звуки не настигнут меня...";
			langStr[101] = "Сау бол!";
			
			langStr[102] = "Что-то разблокировалось.";
			
			langStr[103] = "Это Ледяная Девятка, сильно отличается от Дьявольской Девятки.\nОна призывает айсберги каждые 10 секунд. По истечении 10 секунд айсберг тает, и появляется новая, но в другом месте. Айсберг блокирует определенную территорию 3 на 3. Удачи.";
			langStr[104] = "Это Ядерная Десятка.\nAt first, behaves just like it's original, but when pressing SQUARE, it may not reset it's timer, instead it dodges the attack. Be careful! Every time you press SQUARE, it now pollutes the air with toxic acid fumes, making it very harder to see (and maybe breathe). If it fully blocks the view, you suffocate and it's game over.";
			langStr[105] = "Это Menacing Eleven.\nAs for now, it's just a re-skin. More mechanics might come in later.";
			langStr[106] = "Заметка:";
			langStr[107] = "Нажмите на START чтобы продолжить.";
			langStr[108] = "Нажмите на TRIANGLE чтобы не показывать снова.";
			langStr[109] = "ПС3САПЕР+";
			langStr[110] = "ВДОХНОВЛЕНО САПЕРОМ ПЛЮС";
			
			langStr[111] = "Вишневый Бассейн";
			langStr[112] = "Пруд Коуриндоу";
			langStr[113] = "Хиган Лагуна";
			langStr[114] = "Река Санзо";
			langStr[115] = "Безымянный Корабль";
			langStr[116] = "Туманное Озеро";
			langStr[117] = "Океан Йокаев";
			langStr[118] = "Багрянное Море Мертвяца";
			langStr[119] = "ОБМОРОЖЕННЫЙ КРУГ";
			
			langStr[120] = "Ледяная Лужа";
			langStr[121] = "Замороженный Фонтан";
			langStr[122] = "Тихий Марш";
			langStr[123] = "Скользкое Болото";
			langStr[124] = "Безумные Подмостья";
			langStr[125] = "Raging Rapids";
			langStr[126] = "Дно Страданий";
			langStr[127] = "Черное  Море";
			langStr[128] = "Глубочайший Конец";
			langStr[129] = "РАДИОАКТИВНЫЕ ОТХОДЫ";
			
			langStr[130] = "Опции Боссов";
			langStr[131] = "ВКЛЮЧИТЬ";
			langStr[132] = "Да";
			langStr[133] = "Нет";
			
			break;
		}
	}
}