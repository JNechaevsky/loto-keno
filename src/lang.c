// MIT License
// 
// Copyright (c) 2025 Polina "Aura" N.
// Copyright (c) 2025 Julia Nechaevskaya
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


#include "loto-keno.h"


//
// Title screen
//

const char *lang_title_name;
const char *lang_title_version;
const char *lang_title_developed_by;
const char *lang_title_authors;
const char *lang_title_key_f1;
const char *lang_title_key_f2;
const char *lang_title_press_any_key;

const char **lang_title_quote;
int lang_title_quote_size;

static const char *txt_title_quotes_eng[] = {
    "Chance of Samurai? 1 in 10. Try now!",
    "Blub-blub-blub... A-OOO-OOO-Oo!?",
    "Gambling isn’t a game, it’s life!",
    "Hope for luck, but bet smart!",
    "Every round — your chance to win!",
    "HNA!™ Samurai Approved.",
    "Lost? Try again!",
    "CGA: 4 colors, 1 winner.",
    "Luck is an illusion. Or is it?",
    "Bet like a samurai, lose like a poet.",
    "The gods of luck are watching.",
    "Press any key... if you dare.",
    "Your fate is just a number away.",
    "More colors? No. More luck? Also no.",
    "One bet to rule them all.",
    "Winners smile. Destiny smirks.",
    "It’s not gambling. It’s art.",
    "Today’s luck: Undefined.",
    "The samurai whispers... 'Again.'",
    "Numbers don’t lie. Or do they?",
    "Pick wisely. Or don’t. Who cares?",
    "Fortune favors... probably not you.",
    "The CGA colors are watching you.",
    "Feeling lucky? I wouldn’t be.",
    "Only 4 colors. Only 2 choices.",
    "Who needs 256 colors anyway?",
    "Win = skill. Lose = pure chance.",
    "Do you believe in fate? Good luck!",
    "The bet chooses you, not vice versa.",
    "Just one more round. Just one more...",
    "This is a game about trust issues.",
    "90% luck, 10% regret.",
    "Blub-blub-blub... fate awaits!",
    "The pixels demand sacrifice.",
    "HNA!™— The Ultimate Truth.",
    "CGA. Where dreams go to die.",
    "Hope is just a lucky guess.",
    "You blinked. Luck noticed.",
    "Win? Lose? The answer is: Yes.",
    "Fate is four colors deep.",
    "One click. One fate.",
    "Luck never sleeps.",
    "Nothing personal. Just numbers.",
    "CGA pixels whisper secrets.",
    "Trust the process. Or don’t.",
    "Your luck was here. Now it’s gone.",
    "50% hope. 50% regret.",
    "Is it skill? Or just luck again?",
    "Bet small. Dream big. Lose anyway.",
    "Winning is temporary. Luck is forever.",
};

static const char *txt_title_quotes_rus[] = {
    "Шанс на самурая? 1 к 10. Проверь удачу!",
    "Будь-будь-будь... А-ОООО-ООО-Оо!?",
    "Азарт — это не игра, а стиль жизни!",
    "Надейся на удачу, но ставь с умом!",
    "Каждый раунд — шанс стать чемпионом!",
    "ХНА!™ Одобрено самураями.",
    "Если ты проиграл — попробуй ещё раз!",
    "CGA. 4 цвета. 1 победитель.",
    "Удача — иллюзия. Или нет?",
    "Ставь как самурай, проигрывай как поэт.",
    "Боги удачи пристально следят.",
    "Нажмите клавишу... если не боитесь.",
    "Ваша судьба — это просто число.",
    "Больше цветов? Нет. Больше шансов? Нет.",
    "Одна ставка, чтобы править всем.",
    "Победители улыбаются. Судьба смеётся.",
    "Это не игра. Это искусство провала.",
    "Сегодняшняя удача: неопределённа.",
    "Самурай шепчет... 'Ставь ещё'.",
    "Числа не врут. Или всё-таки врут?",
    "Выберите мудро. Или нет. Всё равно.",
    "Фортуна благоволит... кому-то.",
    "Эти 4 цвета следят за вами.",
    "Чувствуете удачу? Напрасно.",
    "Всего 4 цвета. Всего 2 выбора.",
    "Кому нужны 256 цветов?",
    "Победа — навык. Поражение — случай.",
    "Верите в судьбу? Тогда вам не повезёт.",
    "Ставка выбирает вас, а не наоборот.",
    "Только один раунд. Только один...",
    "Это игра о доверии. Которого нет.",
    "90% удачи, 10% сожаления.",
    "Будь-будь-будь... Судьба зовёт!",
    "Эти пиксели требуют жертв.",
    "ХНА!™ — Истина вне доказательств.",
    "CGA. Где мечты умирают 50/50.",
    "Надежда — просто удачная догадка.",
    "Моргнули? Удача это заметила.",
    "Выигрыш? Проигрыш? Ответ: Да.",
    "Судьба прячется в 4 цветах.",
    "Один клик — одна судьба.",
    "Удача не спит, а ждёт.",
    "Ничего личного. Только цифры.",
    "Пиксели CGA знают тайны.",
    "Доверьтесь судьбе. Или не стоит.",
    "Удача была тут. Теперь её нет.",
    "50% надежды. 50% сожаления.",
    "Это навык? Или просто везение?",
    "Ставьте мало. Мечтайте о многом.",
    "Выигрыш временный. Удача вечна.",
};

//
// Help
//

const char **lang_help_lines;
int lang_help_lines_count;

static const char *txt_help_lines_eng[] = {
    "┌────────────────────────────────────┐",
    "│          LOTO KENO - RULES         │",
    "├────────────────────────────────────┤",
    "│ You start with 10 points.          │",
    "│ You can bet from 1 up to your      │",
    "│ current amount.                    │",
    "│ Then make a choice and see         │",
    "│ if luck is on your side.           │",
    "│                                    │",
    "│ If HNA! appears, win or loss       │",
    "│ is doubled. Game continues until   │",
    "│ you run out of points.             │",
    "│                                    │",
    "│ If they reach zero, you lose.      │",
    "│                                    │",
    "│ Controls:                          │",
    "│ ← and → - select                   │",
    "│ ↑ and ↓ - change bet               │",
    "│ ENTER   - confirm                  │",
    "│ ALT + ENTER - full screen mode     │",
    "│                                    │",
    "│            Good luck!              │",
    "└────────────────────────────────────┘",
};

static const char *txt_help_lines_rus[] = {
    "┌────────────────────────────────────┐",
    "│          ЛОТО КЕНО - ПРАВИЛА       │",
    "├────────────────────────────────────┤",
    "│ В начале у вас есть 10 очков.      │",
    "│ Вы можете сделать ставку от 1      │",
    "│ до вашего текущего количества.     │",
    "│ Затем выберите вариант и узнайте,  │",
    "│ повезло ли вам.                    │",
    "│                                    │",
    "│ Если появляется ХНА!, выигрыш      │",
    "│ или проигрыш удваивается. Игра     │",
    "│ продолжается, пока у вас есть      │",
    "│ очки. Если они закончатся, вы      │",
    "│ проиграете.                        │",
    "│                                    │",
    "│ Управление:                        │",
    "│ ← и → - выбор                      │",
    "│ ↑ и ↓ - ставки                     │",
    "│ ENTER - подтвердить                │",
    "│ ALT + ENTER - полноэкранный режим  │",
    "│                                    │",
    "│               Удачи!               │",
    "└────────────────────────────────────┘",
};

//
// Game field
//

const char *lang_game_score;
const char *lang_game_bet;
const char *lang_game_round;
const char *lang_game_bud_bud_bud;
const char *lang_game_aaa_ooo_ooo;
const char *lang_game_hna;

int resultQuoteIndex = -1;
int resultIsWin = 0; // 1 — выигрыш, 0 — проигрыш

const char **lang_game_win_quote;
const char **lang_game_loose_quote;
int lang_game_quote_win_size;
int lang_game_quote_loose_size;

static const char *txt_game_win_quotes_eng[] = {
    // [PN] Победные фразы:
    "Thank you!",
    "Very filling!",
    "YES! Good!",
    "Your call is very important to us!",
    "You got the job!",
    "Hooray! The milk didn't boil over!",
    "Now you are a senior samurai!",
    "A limousine is on its way for you.",
    "You found the meaning of life!",
    "Well, now you are a robot!",
    "I allow you to eat another cookie!",
    "You are twice as powerful now!",
    "Cats approve your choice.",
    "The barbecue is already sizzling!",
    "Magnificent, 10 out of 10!",
    "Wow! Oh! Ah! Unbelievable!",
    "You gave me hope!",
    "Your legs are beautiful!",
    "YES! THANK YOU!",
    "Now you can drink tea.",
    "Approved by kangaroos!",
    "You are perfect!",
    "Well, that’s wonderful!",
    "Fair enough!",
    "Excellent! Poisonous!",
    // [JN] Let’s add some nonsense!
    "Tumba-pumba!",
    "Menno-zamenno.",
    "Pooo-o-o-o!",
    "Tsa-ah!",
    "Oozyoo!",
    "E-eh!",
    "O-oh!",
    "Decent.",
    "Proper.",
    "D!",
    "Byr-byr-byr!",
    "Dzaytsa!!",
    "Lord, have mercy!",
    "Coins are jingling!",
    "Acha-acha!",
    "Aaah-eeee!",
    "Pickles!",
    "Tomatoes!",
    "Bonus on loyalty card!",
    "Just like Friday!",
    "Come again!",
    "Must-do, must-do!",
    "Hhhkkk!",
    "Ra-ta-ta-ta!",
    "Aa, ooo-oo-oo!",
    // [ND]
    "Rottsyu!",
    "M'daoh!",
    "D'yass!",
    "Gkoonya!",
    "Zao-zhunn!",
};

static const char *txt_game_loose_quotes_eng[] = {
    "Pancakes got burned.",
    "Well, almost!",
    "The cat is disappointed in you.",
    "Just a little short!",
    "You are still beautiful!",
    "The rabbit is disappointed.",
    "Your CPU overheated.",
    "Your glasses fell off.",
    "The neighbor looked at you sadly.",
    "Oops! Op-op!",
    "That was unthinkable!",
    "You are my hero! But no.",
    "Sad trombone sound.",
    "You were tricked!",
    "Almost! (but no).",
    "The carpet is dissatisfied.",
    "Someone sighed in the corner.",
    "The pancake ran away.",
    "It was an experiment!",
    "Everything went off course!",
    "The bicycle silently rode away.",
    "Now try again!",
    "The chicken said 'no'.",
    "System failure.",
    "Banana canceled!",
    // [JN] Let’s add more nonsense!
    "Don’t do it!",
    "Don’t need anything.",
    "Peeeeh!",
    "Nope.",
    "Nah.",
    "Extravagance!",
    "PEEEEeeeh!",
    "Bad purchase.",
    "Not enoughness.",
    "Ptsyuk!",
    "Goodbye then.",
    "Come again!",
    "Nobody said!",
    "TRRRrrsh!",
    "Pso!",
    "Ktslo-o!",
    "Pzleh-eh!",
    "Pyyyyy!",
    "Bankruptcy!",
    "Roof’s gone, water’s dripping.",
    "Unsatisfactory.",
    "POO-OOP!",
    "P-f-f-f-f...",
    "Cap fell sideways.",
    "Horse theft!",
    // [ND]
    "Trysh.",
    "Zhzhytz.",
    "Syomsh.",
    "Luffrr.",
    "Mryaks.",
};

static const char *txt_game_win_quotes_rus[] = {
    // [PN] Победные фразы:
    "Спасибо!",
    "Очень сытно!",
    "ДА! Хорошо!",
    "Ваш звонок очень важен для нас!",
    "Вы приняты на работу!",
    "Ура! Молоко не убежало!",
    "Теперь вы старший самурай!",
    "За вами выехал лимузин.",
    "Вы нашли смысл жизни!",
    "Ну вот, теперь вы робот!",
    "Разрешаю съесть ещё печенье!",
    "Вы в два раза мощнее!",
    "Коты одобряют ваш выбор.",
    "Шашлыки уже жарятся!",
    "Великолепно, 10 из 10!",
    "Вау! О! А! Ничего себе!",
    "Вы дали мне надежду!",
    "Ваши ноги прекрасны!",
    "ДАА! СПАСИБО!",
    "Теперь можно пить чай.",
    "Одобрено кенгуру!",
    "Вы идеальны!",
    "Что ж, замечательно!",
    "Справедливо!",
    "Превосходно! Ядовито!",
    // [JN] Добавим абсурда!
    "Тумба-пумба!",
    "Менно-заменно.",
    "По-о-о-о-о!",
    "Ца-а!",
    "Узю!",
    "Э-эх!",
    "О-ох!",
    "Пристойно.",
    "Благочинно.",
    "Д!",
    "Быр-быр-быр!",
    "Дзаыца!!",
    "Господи, помилуй!",
    "Монетки зазвенели!",
    "Ача-ача!",
    "Ааа-ээээ!",
    "Огурчики!",
    "Помидорчики!",
    "Бонус на карту лояльности!",
    "Как в пятничку!",
    "Приходите ещё!",
    "Надо-надо!",
    "Хььььк!",
    "Ра-та-та-та!",
    "А, ооо-оо-оо!",
    // [ND]
    "Роццю!",
    "Мдао!",
    "Дяссь!",
    "Гкуньа!",
    "Зао-жунь!",
};

static const char *txt_game_loose_quotes_rus[] = {
    // [PN] Проигрышные фразы:
    "Блины подгорели.",
    "Ну, почти!",
    "Кошка на вас обиделась.",
    "Чуть-чуть не хватило!",
    "Вы всё ещё красивы!",
    "Кролик разочарован.",
    "Процессор перегрелся.",
    "Ваши очки упали.",
    "Сосед грустно посмотрел.",
    "Упс! Оп-оп!",
    "Это было немыслимо!",
    "Вы — мой герой! Но нет.",
    "Грустный звук тромбона.",
    "Обманули!",
    "Почти! (но нет).",
    "Ковёр недоволен.",
    "Кто-то вздохнул в углу.",
    "Оладушек сбежал.",
    "Это был эксперимент!",
    "Всё пошло не по плану!",
    "Велосипед молча уехал.",
    "Теперь ещё разок!",
    "Курица сказала \"нет\".",
    "Система не справилась.",
    "Банан отменён!",
    // [JN] Добавим абсурда!
    "Не надо!",
    "Ничего не надо.",
    "Пээээ!",
    "Нет.",
    "Ни.",
    "Растратно!",
    "ПЭЭЭэээ!",
    "Плохо куплено.",
    "Недостатошно.",
    "Пцьк!",
    "Досвидание.",
    "Приходите ещё!",
    "Не сказали!",
    "ТРРррщ!",
    "Псё!",
    "Кцло-о!",
    "Пцлэ-э!",
    "Пыыыы!",
    "Разорение!",
    "Крыша едет, водичка потекла.",
    "Неудовлетворительно.",
    "ПУ-УП!",
    "П-ф-ф-ф-ф...",
    "Кепочка на бок упала.",
    "Конокрадство!",
    // [ND]
    "Трыщ.",
    "Жжыц.", 
    "Сёмщ.",
    "Луфьр.",
    "Мрякс.",
};

//
// Game over
//
const char *lang_over_game;
const char *lang_over_rounds;
const char *lang_over_max_score;
const char *lang_over_enter;


// -----------------------------------------------------------------------------
// L_SetLanguageStrings
//  Инициализация и переинициализация текстовых строчек.
// -----------------------------------------------------------------------------

void L_SetLanguageStrings (void)
{
    switch (language)
    {
        case 0:  // English
            SDL_SetWindowTitle(window, "Loto Keno!");
            lang_title_name          = "Loto Keno!";
            lang_title_version       = "Version 1.0 (03/18/2025)"; // MM/DD/YYYY
            lang_title_developed_by  = "Developed and designed by:";
            lang_title_authors       = "Polina \"Aura\" N. ♥ Julia Nechaevskaya";
            lang_title_key_f1        = "F1 - Help & Rules";
            lang_title_key_f2        = "F2 - Language: English";
            lang_title_press_any_key = "Press any key...";
            lang_title_quote = txt_title_quotes_eng;
            lang_title_quote_size = sizeof(txt_title_quotes_eng) / sizeof(txt_title_quotes_eng[0]);
            
            lang_help_lines = txt_help_lines_eng;
            lang_help_lines_count = sizeof(txt_help_lines_eng) / sizeof(txt_help_lines_eng[0]);

            lang_game_score       = "      Score:";
            lang_game_bet         = "        Bet:";
            lang_game_round       = "      Round:";
            lang_game_bud_bud_bud = "Blub-blub-blub!";
            lang_game_aaa_ooo_ooo = "A-OOOO-OOO-Oo!";
            lang_game_hna         = "HNA!";
            lang_game_win_quote = txt_game_win_quotes_eng;
            lang_game_quote_win_size = sizeof(txt_game_win_quotes_eng) / sizeof(txt_game_win_quotes_eng[0]);
            lang_game_loose_quote = txt_game_loose_quotes_eng;
            lang_game_quote_loose_size = sizeof(txt_game_loose_quotes_eng) / sizeof(txt_game_loose_quotes_eng[0]);

            lang_over_game      = "GAME OVER";
            lang_over_rounds    = "       Rounds played:";
            lang_over_max_score = "         Highest win:";
            lang_over_enter     = "Press ENTER to restart";
            break;

        case 1:  // Русский
            SDL_SetWindowTitle(window, "Лото Кено!");
            lang_title_name          = "Лото Кено!";
            lang_title_version       = "Версия 1.0 (18.03.2025)"; // DD.MM.YYYY
            lang_title_developed_by  = "Разработка и идея:";
            lang_title_authors       = "Полина \"Аура\" Н. ♥ Юлия Нечаевская";
            lang_title_key_f1        = "F1 - Помощь и правила";
            lang_title_key_f2        = "F2 - Язык: Русский";
            lang_title_press_any_key = "Нажмите любую клавишу...";
            lang_title_quote = txt_title_quotes_rus;
            lang_title_quote_size = sizeof(txt_title_quotes_rus) / sizeof(txt_title_quotes_rus[0]);

            lang_help_lines = txt_help_lines_rus;
            lang_help_lines_count = sizeof(txt_help_lines_rus) / sizeof(txt_help_lines_rus[0]);

            lang_game_score       = "       Очки:";
            lang_game_bet         = "     Ставка:";
            lang_game_round       = "      Раунд:";
            lang_game_bud_bud_bud = "Будь-будь-будь!";
            lang_game_aaa_ooo_ooo = "А-ОООО-ООО-Оо!";
            lang_game_hna         = "ХНА!";
            lang_game_win_quote = txt_game_win_quotes_rus;
            lang_game_quote_win_size = sizeof(txt_game_win_quotes_rus) / sizeof(txt_game_win_quotes_rus[0]);
            lang_game_loose_quote = txt_game_loose_quotes_rus;
            lang_game_quote_loose_size = sizeof(txt_game_loose_quotes_rus) / sizeof(txt_game_loose_quotes_rus[0]);

            lang_over_game      = "ИГРА ЗАКОНЧЕНА";
            lang_over_rounds    = "        Сыграно раундов:";
            lang_over_max_score = "   Максимальный выигрыш:";
            lang_over_enter     = "Нажмите ENTER для перезапуска";
            break;
    }
    // [PN] Выбираем новую цитату при смене языка
    G_GetTitleQuote(0);
}
