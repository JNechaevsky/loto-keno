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


// Компиляция под Windows / MSYS:
//  windres loto-keno.rc -o loto-keno.o 
//  gcc loto-keno.c loto-keno.o -o loto-keno.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lgdi32 -mwindows
//
// Компиляция под Linux:
//  gcc loto-keno.c -o loto-keno -lSDL2main -lSDL2 -lSDL2_ttf


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "font.c"


// Window size, representing CGA 320x200 mode (2x)
#define SCREENWIDTH     640
#define SCREENHEIGHT    400
// The size of the monospacing font (2x as well)
#define FONT_SIZE       16
// 1 in 10 chance of a samurai
#define SAMURAI_CHANCE  10 


SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

int isHoveringLeft;
int isHoveringRight;

int score = 10;
int maxScore = 10; // [PN] Начальное значение соответствует стартовому счёту
int bet = 1; // Запоминаем последнюю ставку
int rounds = 0; // [JN] Means "round", but it's C reserved word.
int choice = 0; // 1 = Будь-будь-будь, 2 = А-ООО!
int samuraiAppeared = 0; // Был ли самурай в этом раунде
int gameOver = 0; // Флаг окончания игры
int gameStarted = 0; // Флаг начала игры
int gameHelp = 0; // [JN] Are we in help screen?

// Standard CGA colors 
SDL_Color white   = { 255, 255, 255, 255 };
SDL_Color black   = {   0,   0,   0, 255 };
SDL_Color cyan    = {   0, 170, 170, 255 };
SDL_Color magenta = { 170,   0, 170, 255 };
// Extra CGA color
SDL_Color gray    = {  85,  85,  85, 255 };
SDL_Color yellow  = { 255, 255,  85, 255 };
SDL_Color blue    = {   0,   0, 170, 255 };
SDL_Color red     = { 170,   0,   0, 255 };




/*
SDL_Color white  = {255, 255, 255, 255};
SDL_Color blue   = {85, 85, 255, 255};
SDL_Color cyan   = {0, 170, 170, 255};
SDL_Color purple = {170, 0, 170, 255};
SDL_Color yellow = {255, 255, 85, 255};

// [JN] TODO - CGA Coordinate grid. Can be removed or cleaned up.
1111111111111111111111111111111111111111      0
2222222222222222222222222222222222222222     16
3333333333333333333333333333333333333333     32
4444444444444444444444444444444444444444     48
5555555555555555555555555555555555555555     64
6666666666666666666666666666666666666666     80
7777777777777777777777777777777777777777     96
8888888888888888888888888888888888888888    112
9999999999999999999999999999999999999999    128
0000000000000000000000000000000000000000    144
1111111111111111111111111111111111111111    160
2222222222222222222222222222222222222222    176
3333333333333333333333333333333333333333    192
4444444444444444444444444444444444444444    208
5555555555555555555555555555555555555555    224
6666666666666666666666666666666666666666    240
7777777777777777777777777777777777777777    256
8888888888888888888888888888888888888888    272
9999999999999999999999999999999999999999    288
0000000000000000000000000000000000000000    304
1111111111111111111111111111111111111111    320
2222222222222222222222222222222222222222    336
3333333333333333333333333333333333333333    352
4444444444444444444444444444444444444444    368
5555555555555555555555555555555555555555    384
*/




// -----------------------------------------------------------------------------
// R_DrawText
//  Draw text string.
// -----------------------------------------------------------------------------

void R_DrawText (const char *text, int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// -----------------------------------------------------------------------------
// R_DrawTextCentered
//  Draw centerered text string.
// -----------------------------------------------------------------------------

void R_DrawTextCentered (const char *text, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderUTF8_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = { (SCREENWIDTH - surface->w) / 2, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}



// -----------------------------------------------------------------------------
// G_GetRandomQuote
//  Возвращает случайную цитату из списка.
// -----------------------------------------------------------------------------

const char *randomQuote = NULL;

const char* G_GetRandomQuote (void)
{
    static const char *quotes[] = {
        "Шанс на самурая? 1 к 10. Проверь удачу!",
        "Будь-будь-будь... А-ОООО-ООО-Оо!?",
        "Азарт — это не игра, а стиль жизни!",
        "Надейся на удачу, но ставь с умом!",
        "Каждый раунд — шанс стать чемпионом!",
        "ХНА!™ Одобрено самураями.",
        "Если ты проиграл — попробуй ещё раз!",
        "CGA. 4 цвета. 1 победитель."
    };

    const int totalQuotes = sizeof(quotes) / sizeof(quotes[0]);
    return quotes[rand() % totalQuotes];
}

const char *resultQuote = NULL;
SDL_Color resultColor;

const char *G_GetWinQuote (void)
{
    static const char *quotes[] = {
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

    const int totalQuotes = sizeof(quotes) / sizeof(quotes[0]);
    return quotes[rand() % totalQuotes];
}

const char *G_GetLooseQuote (void)
{
    static const char *quotes[] = {
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

    const int totalQuotes = sizeof(quotes) / sizeof(quotes[0]);
    return quotes[rand() % totalQuotes];
}

// -----------------------------------------------------------------------------
// G_ResetGame
//  Сброс состояния игры
// -----------------------------------------------------------------------------

void G_ResetGame (void)
{
    score = maxScore = 10; // [PN] Сбрасываем счёт и максимальный счёт до начальных значений
    bet = 1; // [PN] Сбрасываем ставку
    rounds = 0; // [JN] Обнуляем счётчик раундов
    gameOver = 0; // [PN] Сбрасываем флаг окончания игры
    gameStarted = 0; // [PN] Сбрасываем флаг начала игры
    gameHelp = 0; // [JN] Закрываем экран помощи
    randomQuote = G_GetRandomQuote(); // [PN] Обновляем цитату на титульном экране
    resultQuote = NULL; // [JN] Сбрасываем цитату результата ставки
}

// -----------------------------------------------------------------------------
// G_DetermineResult
//  Start new round.
// -----------------------------------------------------------------------------

void G_StartNewRound (void)
{
    // [PN] 1 к 10 шанс появления самурая в начале раунда
    samuraiAppeared = (rand() % SAMURAI_CHANCE == 0);
    // [PN] Clear bet's choice
    choice = 0;
    // [JN] Increment round counter.
    rounds++;
}

// -----------------------------------------------------------------------------
// G_DetermineResult
//  Determine result of current round.
// -----------------------------------------------------------------------------

void G_DetermineResult (void)
{
    const int randomNumber = rand() % 100; // [PN] Число от 0 до 99
    const int isEven = (randomNumber % 2 == 0);
    const int win = (choice == 1 && isEven) || (choice == 2 && !isEven);
    
    if (win)
    {
        score += bet * (samuraiAppeared ? 2 : 1);
        // [PN] Обновляем maxScore, если score стал больше него:
        if (score > maxScore)
        {
            maxScore = score;
        }
        resultQuote = G_GetWinQuote();
        resultColor = blue;
    }
    else
    {
        score -= bet * (samuraiAppeared ? 2 : 1);
        // [JN] В случае проигрыша, ставка не должна превышать текущий счёт!
        if (bet > score)
        {
            bet = score;
        }
        resultQuote = G_GetLooseQuote();
        resultColor = red;
    }
    
    if (score <= 0)
    {
        gameOver = 1;
        resultQuote = NULL;
    }
    else
    {
        G_StartNewRound();
    }
}



// -----------------------------------------------------------------------------
// D_DrawTitleScreen
//  Draw title screen.
// -----------------------------------------------------------------------------

void D_DrawTitleScreen (void)
{
    R_DrawTextCentered("╔══════════════════════╗", 16, white);
    R_DrawTextCentered("║                      ║", 32, white);
    R_DrawTextCentered("║      Лото Кено!      ║", 48, white);
    R_DrawTextCentered("║                      ║", 64, white);
    R_DrawTextCentered("╚══════════════════════╝", 80, white);

    R_DrawTextCentered("Версия 1.0 (18.03.2025)", 112, magenta);
    R_DrawTextCentered(randomQuote, 144, magenta);

    R_DrawTextCentered("Разработка и идея:", 192, white);
    R_DrawTextCentered("Полина \"Аура\" Н. ♥ Юлия Нечаевская", 224, magenta);

    R_DrawTextCentered("F1 - Помощь и правила", 288, white);

    R_DrawTextCentered("Нажмите любую клавишу...", 368, white);
}

// -----------------------------------------------------------------------------
// D_DrawHelpScreen
//  Draw help screen.
//  [JN] TODO - do not invoke again if we already in help screen.
// -----------------------------------------------------------------------------

void D_DrawHelpScreen (void)
{
    R_DrawText("┌────────────────────────────────────┐", 16,  16, white);
    R_DrawText("│          ЛОТО КЕНО - ПРАВИЛА       │", 16,  32, white);
    R_DrawText("├────────────────────────────────────┤", 16,  48, white);
    R_DrawText("│ В начале у вас есть 10 очков.      │", 16,  64, white);
    R_DrawText("│ Вы можете сделать ставку от 1      │", 16,  80, white);
    R_DrawText("│ до вашего текущего количества.     │", 16,  96, white);
    R_DrawText("│ Затем выберите вариант и узнайте,  │", 16, 112, white);
    R_DrawText("│ повезло ли вам.                    │", 16, 128, white);
    R_DrawText("│                                    │", 16, 144, white);
    R_DrawText("│ Если появляется ХНА!, выигрыш      │", 16, 160, white);
    R_DrawText("│ или проигрыш удваивается.          │", 16, 176, white);
    R_DrawText("│                                    │", 16, 192, white);
    R_DrawText("│ Игра продолжается, пока у вас есть │", 16, 208, white);
    R_DrawText("│ очки. Если они закончатся, вы      │", 16, 224, white);
    R_DrawText("│ проиграете.                        │", 16, 240, white);
    R_DrawText("│                                    │", 16, 256, white);
    R_DrawText("│ Управление:                        │", 16, 272, white);
    R_DrawText("│ ← и → - выбор                      │", 16, 288, white);
    R_DrawText("│ ↑ и ↓ - изменение ставки           │", 16, 304, white);
    R_DrawText("│ ENTER - подтвердить                │", 16, 320, white);
    R_DrawText("│                                    │", 16, 336, white);
    R_DrawText("│               Удачи!               │", 16, 352, white);
    R_DrawText("└────────────────────────────────────┘", 16, 368, white);
}

// -----------------------------------------------------------------------------
// D_DrawGameOverScreen
//  Draw game over screen.
// -----------------------------------------------------------------------------

void D_DrawGameOverScreen (void)
{
    R_DrawTextCentered("ЛОТО КЕНО ЗАКОНЧЕНО", 160, white);

    // [PN] Русский язык — тот ещё монстр склонений, поэтому нужно учитывать
    // числа 11–14, 111–114 и т.д., чтобы не получилось ошибок.
    char gameOverText[64];
    const int lastTwoDigits = rounds % 100;
    const int lastDigit = rounds % 10;

    sprintf(gameOverText, "Максимальный счёт: %d (%d %s)", maxScore, rounds,
           (lastTwoDigits >= 11 && lastTwoDigits <= 14) ? "раундов" :
           (lastDigit == 1) ? "раунд" :
           (lastDigit >= 2 && lastDigit <= 4) ? "раунда" :
           "раундов");

    R_DrawTextCentered(gameOverText, 192, white);

    R_DrawTextCentered("Нажмите ENTER для перезапуска", 240, cyan);
}

// -----------------------------------------------------------------------------
// D_DrawGameField
//  Draw main game field.
// -----------------------------------------------------------------------------

void D_DrawGameField (void)
{
    R_DrawTextCentered("╔══════════════════════╗",  16, white);
    R_DrawTextCentered("║                      ║",  32, white);
    R_DrawTextCentered("║                      ║",  48, white);
    R_DrawTextCentered("║                      ║",  64, white);
    R_DrawTextCentered("║                      ║",  80, white);
    R_DrawTextCentered("║                      ║",  96, white);
    R_DrawTextCentered("║                      ║", 112, white);
    R_DrawTextCentered("║                      ║", 128, white);
    R_DrawTextCentered("╚══════════════════════╝", 144, white);

    char scoreText[32];
    sprintf(scoreText, "Очки: %d", score);
    R_DrawText(scoreText, 256, 48, white);

    char betText[32];
    sprintf(betText, "Ставка: %d", bet);
    R_DrawText(betText, 224, 80, white);

    char roundText[32];
    sprintf(roundText, "Раунд: %d", rounds);
    R_DrawText(roundText, 240, 112, white);

    // [PN] Обработка событий мыши
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    isHoveringLeft = (mouseX >= 16 && mouseX <= 320 && mouseY >= 176 && mouseY <= 256);
    isHoveringRight = (mouseX >= 336 && mouseX <= 624 && mouseY >= 176 && mouseY <= 256);
    const SDL_Color left_color = (isHoveringLeft || choice == 1) ? white : magenta;
    const SDL_Color right_color = (isHoveringRight || choice == 2) ? white : magenta;

    R_DrawText("┌─────────────────┐", 16, 176, left_color);
    R_DrawText("│                 │", 16, 192, left_color);
    R_DrawText("│                 │", 16, 208, left_color);
    R_DrawText("│                 │", 16, 224, left_color);
    R_DrawText("└─────────────────┘", 16, 240, left_color);

    R_DrawText("Будь-будь-будь!", 48, 208, left_color);

    R_DrawText("┌────────────────┐", 336, 176, right_color);
    R_DrawText("│                │", 336, 192, right_color);
    R_DrawText("│                │", 336, 208, right_color);
    R_DrawText("│                │", 336, 224, right_color);
    R_DrawText("└────────────────┘", 336, 240, right_color);

    R_DrawText("А-ОООО-ООО-Оо!", 368, 208, right_color);

    if (samuraiAppeared)
    {
        R_DrawTextCentered("╔══════════╗", 272, yellow);
        R_DrawTextCentered("║          ║", 288, yellow);
        R_DrawTextCentered("║   ХНА!   ║", 304, yellow);
        R_DrawTextCentered("║          ║", 320, yellow);
        R_DrawTextCentered("╚══════════╝", 336, yellow);
    }

    // [JN] TODO - Switch between 4 / 16 colors?
    if (resultQuote)
    R_DrawTextCentered(resultQuote, 368, resultColor);
}

// Обработка событий мыши
void HandleMouseEvents (SDL_Event *event)
{
    static int mousePressed = 0; // [PN] Отслеживание нажатия кнопки мыши

    if (event->type == SDL_MOUSEWHEEL && gameStarted && !gameOver)
    {
        bet += (event->wheel.y > 0 && bet < score) ? 1 : 0; // [PN] Увеличиваем ставку
        bet -= (event->wheel.y < 0 && bet > 1) ? 1 : 0;     // [PN] Уменьшаем ставку
    }

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && !mousePressed)
    {
        mousePressed = 1; // [PN] Фиксируем нажатие кнопки

        if (gameHelp)
        {
            gameHelp = 0; // [JN] Закрываем экран помощи
            return;
        }

        if (!gameStarted)
        {
            gameStarted = 1; // [PN] Стартуем игру
            G_StartNewRound();
            return;
        }

        if (gameOver)
        {
            G_ResetGame(); // [PN] Перезапуск игры
            gameStarted = 1;
            G_StartNewRound();
            return;
        }

        if (bet > 0)
        {
            choice = isHoveringLeft ? 1 : isHoveringRight ? 2 : 0; // [PN] Определяем выбор
            if (choice > 0)
                G_DetermineResult();
        }
    }

    if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT)
    {
        mousePressed = 0; // [PN] Сбрасываем нажатие
    }
}

// Обработка событий клавиатуры
void HandleKeyboardEvents (SDL_Event *event)
{
    if (event->type != SDL_KEYDOWN) return;

    SDL_Keycode key = event->key.keysym.sym;

    if (key == SDLK_F1)
    {
        gameHelp ^= 1; // [JN] Переключаем экран помощи
        return;
    }

    if (gameHelp)
    {
        gameHelp = 0; // [JN] Закрываем экран помощи
        return;
    }

    if (!gameStarted)
    {
        if (key == SDLK_ESCAPE)
            exit(0); // [PN] Выход из игры

        gameStarted = 1; // [PN] Стартуем игру
        G_StartNewRound();
        return;
    }

    if (gameOver)
    {
        if (key == SDLK_RETURN || key == SDLK_KP_ENTER)
        {
            G_ResetGame(); // [PN] Перезапуск игры
            gameStarted = 1;
            G_StartNewRound();
        }
        return;
    }

    switch (key)
    {
        case SDLK_UP:     if (bet < score) bet++  break; // [PN] Увеличиваем ставку
        case SDLK_DOWN:   if (bet > 1) bet--;     break; // [PN] Уменьшаем ставку
        case SDLK_LEFT:   choice = 1;             break; // [PN] Выбор "Будь-будь-будь!"
        case SDLK_RIGHT:  choice = 2;             break; // [PN] Выбор "А-ОООО-ООО-Оо!"

        case SDLK_RETURN:
        case SDLK_KP_ENTER:
            if (choice > 0)
                G_DetermineResult(); // [PN] Подтверждаем выбор
            break;

        case SDLK_ESCAPE:
            G_ResetGame(); // [PN] Возвращаемся в меню
            break;
    }
}

// -----------------------------------------------------------------------------
// D_KenoLoop
//  Главный цикл игры.
// -----------------------------------------------------------------------------

void D_KenoLoop (void)
{
    SDL_Event event;
    srand(time(NULL));

    int running = 1;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
                running = 0;

            HandleMouseEvents(&event);
            HandleKeyboardEvents(&event);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        if (gameHelp)
            D_DrawHelpScreen();
        else if (!gameStarted)
            D_DrawTitleScreen();
        else if (gameOver)
            D_DrawGameOverScreen();
        else
            D_DrawGameField();

        SDL_RenderPresent(renderer);
        // [PN] Игра работает с фиксированной частотой 35 кадров в секунду — как в классическом Doom.
        // Задержка между кадрами вычисляется как 1000 мс / 35 ≈ 28.57 мс.
        // Мы используем SDL_Delay(28), чтобы поддерживать стабильный FPS и снизить нагрузку на процессор.
        SDL_Delay(28);
    }
}


// -----------------------------------------------------------------------------
// Main program loop.
// -----------------------------------------------------------------------------

int main (int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1)
    {
        return 1;
    }

    window = SDL_CreateWindow("Лото Кено!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    
    // [PN] Загрузка шрифта, встроенного в код (font.c)
    SDL_RWops *rw = SDL_RWFromMem(ibm_vga_data, ibm_vga_data_len);
    font = TTF_OpenFontRW(rw, 0, FONT_SIZE);

    // [PN] Выбор цитаты при запуске игры
    randomQuote = G_GetRandomQuote();

    D_KenoLoop();
    
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}
