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


#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


// =============================================================================
//
// DRAW.C
//
// =============================================================================

extern void R_FinishUpdate (void);

// =============================================================================
//
// FONT.C
//
// =============================================================================

extern unsigned int  ibm_vga_data_len;
extern unsigned char ibm_vga_data[];

// =============================================================================
//
// GAME.C
//
// =============================================================================

extern void G_StartNewRound (void);
extern void G_ResetGame (void);
extern void G_StartNewRound (void);
extern void G_DetermineResult (void);

extern const char *G_GetTitleQuote (int randomize);

// =============================================================================
//
// LANG.C
//
// =============================================================================

extern void L_SetLanguageStrings (void);

// Title screen
extern const char *lang_title_name;
extern const char *lang_title_version;
extern const char *lang_title_developed_by;
extern const char *lang_title_authors;
extern const char *lang_title_key_f1;
extern const char *lang_title_key_f2;
extern const char *lang_title_press_any_key;

extern const char **lang_title_quote;
extern int lang_title_quote_size;

// Help 
extern const char **lang_help_lines;
extern int lang_help_lines_count;

// Game field
extern const char *lang_game_score;
extern const char *lang_game_bet;
extern const char *lang_game_round;
extern const char *lang_game_bud_bud_bud;
extern const char *lang_game_aaa_ooo_ooo;
extern const char *lang_game_hna;


extern int resultQuoteIndex;
extern int resultIsWin;

extern const char **lang_game_win_quote;
extern const char **lang_game_loose_quote;
extern int lang_game_quote_win_size;
extern int lang_game_quote_loose_size;

// Game over
extern const char *lang_over_game;
extern const char *lang_over_rounds;
extern const char *lang_over_max_score;
extern const char *lang_over_enter;

// -----------------------------------------------------------------------------
// English language
// -----------------------------------------------------------------------------

#define TXT_TITLE_NAME_ENG          "Loto Keno!"
#define TXT_TITLE_VERSION_ENG       "Version 1.0 (03/18/2025)"  // MM/DD/YYYY
#define TXT_TITLE_DEVELOPED_BY_ENG  "Developed and designed by:"
#define TXT_TITLE_AUTHORS_ENG       "Polina \"Aura\" N. ♥ Julia Nechaevskaya"
#define TXT_TITLE_KEY_F1_ENG        "F1 - Help & Rules"
#define TXT_TITLE_KEY_F2_ENG        "F2 - Language: English"
#define TXT_TITLE_PRESS_ANY_KEY_ENG "Press any key..."

static const char *txt_title_quotes_eng[] = {
    "Chance of Samurai? 1 in 10. Try now!",
    "Blub-blub-blub... A-OOO-OOO-Oo!?",
    "Gambling isn’t a game, it’s life!",
    "Hope for luck, but bet smart!",
    "Every round — your chance to win!",
    "HNA!™ Samurai Approved.",
    "Lost? Try again!",
    "CGA: 4 colors, 1 winner.",
};

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
    "│ is doubled.                        │",
    "│                                    │",
    "│ Game continues until you run       │",
    "│ out of points. If they reach zero, │",
    "│ you lose.                          │",
    "│                                    │",
    "│ Controls:                          │",
    "│ ← and → - select                   │",
    "│ ↑ and ↓ - change bet               │",
    "│ ENTER - confirm                    │",
    "│                                    │",
    "│            Good luck!              │",
    "└────────────────────────────────────┘",
};

#define TXT_GAME_SCORE_ENG          "      Score:"
#define TXT_GAME_BET_ENG            "        Bet:"
#define TXT_GAME_ROUND_ENG          "      Round:"
#define TXT_GAME_BUD_BUD_BUD_ENG    "Blub-blub-blub!"
#define TXT_GAME_AAA_OOO_OOO_ENG    "A-OOOO-OOO-Oo!"
#define TXT_GAME_HNA_ENG            "HNA!"

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

#define TXT_OVER_GAME_ENG           "GAME OVER"
#define TXT_OVER_ROUNDS_ENG         "       Rounds played:"
#define TXT_OVER_MAX_SCORE_ENG      "         Highest win:"
#define TXT_OVER_ENTER_ENG          "Press ENTER to restart"

// -----------------------------------------------------------------------------
// Русский язык
// -----------------------------------------------------------------------------

#define TXT_TITLE_NAME_RUS          "Лото Кено!"
#define TXT_TITLE_VERSION_RUS       "Версия 1.0 (18.03.2025)"  // DD.MM.YYYY
#define TXT_TITLE_DEVELOPED_BY_RUS  "Разработка и идея:"
#define TXT_TITLE_AUTHORS_RUS       "Полина \"Аура\" Н. ♥ Юлия Нечаевская"
#define TXT_TITLE_KEY_F1_RUS        "F1 - Помощь и правила"
#define TXT_TITLE_KEY_F2_RUS        "F2 - Язык: Русский"
#define TXT_TITLE_PRESS_ANY_KEY_RUS "Нажмите любую клавишу..."

static const char *txt_title_quotes_rus[] = {
    "Шанс на самурая? 1 к 10. Проверь удачу!",
    "Будь-будь-будь... А-ОООО-ООО-Оо!?",
    "Азарт — это не игра, а стиль жизни!",
    "Надейся на удачу, но ставь с умом!",
    "Каждый раунд — шанс стать чемпионом!",
    "ХНА!™ Одобрено самураями.",
    "Если ты проиграл — попробуй ещё раз!",
    "CGA. 4 цвета. 1 победитель."
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
    "│ или проигрыш удваивается.          │",
    "│                                    │",
    "│ Игра продолжается, пока у вас есть │",
    "│ очки. Если они закончатся, вы      │",
    "│ проиграете.                        │",
    "│                                    │",
    "│ Управление:                        │",
    "│ ← и → - выбор                      │",
    "│ ↑ и ↓ - изменение ставки           │",
    "│ ENTER - подтвердить                │",
    "│                                    │",
    "│               Удачи!               │",
    "└────────────────────────────────────┘",
};

#define TXT_GAME_SCORE_RUS          "       Очки:"
#define TXT_GAME_BET_RUS            "     Ставка:"
#define TXT_GAME_ROUND_RUS          "      Раунд:"
#define TXT_GAME_BUD_BUD_BUD_RUS    "Будь-будь-будь!"
#define TXT_GAME_AAA_OOO_OOO_RUS    "А-ОООО-ООО-Оо!"
#define TXT_GAME_HNA_RUS            "ХНА!"

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

#define TXT_OVER_GAME_RUS           "ИГРА ЗАКОНЧЕНА"
#define TXT_OVER_ROUNDS_RUS         "        Сыграно раундов:"
#define TXT_OVER_MAX_SCORE_RUS      "   Максимальный выигрыш:"
#define TXT_OVER_ENTER_RUS          "Нажмите ENTER для перезапуска"

// =============================================================================
//
// LOTO-KENO.C
//
// =============================================================================

// Window size, representing CGA 320x200 mode (2x)
#define SCREENWIDTH     640
#define SCREENHEIGHT    400
// The size of the monospacing font (2x as well)
#define FONT_SIZE       16
// 1 in 10 chance of a samurai
#define SAMURAI_CHANCE  10 

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern TTF_Font *font;

extern int screen_visible;

extern SDL_Color white;
extern SDL_Color black;
extern SDL_Color cyan;
extern SDL_Color magenta;
extern SDL_Color gray;
extern SDL_Color yellow;
extern SDL_Color blue;
extern SDL_Color red;
extern SDL_Color resultColor;

extern int language;
extern int score;
extern int maxScore;
extern int bet;
extern int rounds;
extern int choice;
extern int samuraiAppeared;
extern int gameOver;
extern int gameStarted;
extern int gameHelp;

extern int isHoveringLeft;
extern int isHoveringRight;
