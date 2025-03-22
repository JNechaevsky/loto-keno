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
#include "lang.c"


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

int language = 0; // [JN] 0 = English, 1 = Русский
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

SDL_Color resultColor;



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

const char *G_GetRandomQuote (int randomize)
{
    static int randomQuoteIndex = 0;

    if (randomize)
        randomQuoteIndex = rand() % lang_title_quote_size;

    return lang_title_quote[randomQuoteIndex];
}

const char *G_GetWinQuote (int randomize)
{
    static int randomWinQuoteIndex = 0;

    if (randomize)
        randomWinQuoteIndex = rand() % lang_game_quote_win_size;

    return lang_game_win_quote[randomWinQuoteIndex];
}

const char *G_GetLooseQuote (int randomize)
{
    static int randomLooseQuoteIndex = 0;

    if (randomize)
        randomLooseQuoteIndex = rand() % lang_game_quote_loose_size;

    return lang_game_loose_quote[randomLooseQuoteIndex];
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
    G_GetRandomQuote(1); // [PN] Обновляем цитату при старте новой игры
    resultQuoteIndex = -1;
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
        resultColor = blue;
        resultQuoteIndex = rand() % lang_game_quote_win_size;
        resultIsWin = 1; // [PN] 💙 вот это важно!
    }
    else
    {
        score -= bet * (samuraiAppeared ? 2 : 1);
        // [JN] В случае проигрыша, ставка не должна превышать текущий счёт!
        if (bet > score)
        {
            bet = score;
        }
        resultColor = red;
        resultQuoteIndex = rand() % lang_game_quote_loose_size;
        resultIsWin = 0; // [PN] ❤️ и это тоже!
    }
    
    if (score <= 0)
    {
        gameOver = 1;
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
    R_DrawTextCentered("║                      ║", 48, white);
    R_DrawTextCentered("║                      ║", 64, white);
    R_DrawTextCentered("╚══════════════════════╝", 80, white);
    R_DrawTextCentered(lang_title_name, 48, white);
    R_DrawTextCentered(lang_title_version, 112, magenta);
    R_DrawTextCentered(G_GetRandomQuote(0), 144, magenta);

    R_DrawTextCentered(lang_title_developed_by, 192, white);
    R_DrawTextCentered(lang_title_authors, 224, magenta);

    R_DrawText(lang_title_key_f1, 160, 288, white);
    R_DrawText(lang_title_key_f2, 160, 320, white);

    R_DrawTextCentered(lang_title_press_any_key, 368, white);
}

// -----------------------------------------------------------------------------
// D_DrawHelpScreen
//  Draw help screen.
//  [JN] TODO - do not invoke again if we already in help screen.
// -----------------------------------------------------------------------------

void D_DrawHelpScreen (void)
{
    for (int i = 0; i < lang_help_lines_count; i++)
    {
        R_DrawText(lang_help_lines[i], 16, 16 + i * 16, white);
    }
}

// -----------------------------------------------------------------------------
// D_DrawGameOverScreen
//  Draw game over screen.
// -----------------------------------------------------------------------------

void D_DrawGameOverScreen (void)
{
    R_DrawTextCentered(lang_over_game, 160, white);

    char roundsText[64];
    sprintf(roundsText, "%s %d", lang_over_rounds, rounds);
    R_DrawText(roundsText, 64, 192, white);
    
    char scoreText[64];
    sprintf(scoreText, "%s %d", lang_over_max_score, maxScore);
    R_DrawText(scoreText, 64, 224, white);

    R_DrawTextCentered(lang_over_enter, 304, cyan);
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

    R_DrawText(lang_game_score, 144, 48, white);
    char scoreText[32];
    sprintf(scoreText, "%d", score);
    R_DrawText(scoreText, 352, 48, white);

    R_DrawText(lang_game_bet, 144, 80, white);
    char betText[32];
    sprintf(betText, "%d", bet);
    R_DrawText(betText, 352, 80, white);

    R_DrawText(lang_game_round, 144, 112, white);
    char roundText[32];
    sprintf(roundText, "%d", rounds);
    R_DrawText(roundText, 352, 112, white);

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

    R_DrawText(lang_game_bud_bud_bud, 48, 208, left_color);

    R_DrawText("┌────────────────┐", 336, 176, right_color);
    R_DrawText("│                │", 336, 192, right_color);
    R_DrawText("│                │", 336, 208, right_color);
    R_DrawText("│                │", 336, 224, right_color);
    R_DrawText("└────────────────┘", 336, 240, right_color);

    R_DrawText(lang_game_aaa_ooo_ooo, 368, 208, right_color);

    if (samuraiAppeared)
    {
        R_DrawTextCentered("╔══════════╗", 272, yellow);
        R_DrawTextCentered("║          ║", 288, yellow);
        R_DrawTextCentered("║          ║", 304, yellow);
        R_DrawTextCentered("║          ║", 320, yellow);
        R_DrawTextCentered("╚══════════╝", 336, yellow);
        R_DrawTextCentered(lang_game_hna, 304, yellow);
    }

    // [JN] TODO - Switch between 4 / 16 colors?
    if (resultQuoteIndex >= 0)
    {
        const char *quote = resultIsWin
            ? lang_game_win_quote[resultQuoteIndex]
            : lang_game_loose_quote[resultQuoteIndex];
    
        R_DrawTextCentered(quote, 368, resultColor);
    }
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

    if (key == SDLK_F2)
    {
        language ^= 1; // [JN] Смена языка
        D_SetLanguageStrings();
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
        case SDLK_UP:     if (bet < score) bet++;  break; // [PN] Увеличиваем ставку
        case SDLK_DOWN:   if (bet > 1) bet--;      break; // [PN] Уменьшаем ставку
        case SDLK_LEFT:   choice = 1;              break; // [PN] Выбор "Будь-будь-будь!"
        case SDLK_RIGHT:  choice = 2;              break; // [PN] Выбор "А-ОООО-ООО-Оо!"

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

void D_SetLanguageStrings (void)
{
    switch (language)
    {
        case 0:  // English
            SDL_SetWindowTitle(window, TXT_TITLE_NAME_ENG);
            lang_title_name = TXT_TITLE_NAME_ENG;
            lang_title_version = TXT_TITLE_VERSION_ENG;
            lang_title_developed_by = TXT_TITLE_DEVELOPED_BY_ENG;
            lang_title_authors = TXT_TITLE_AUTHORS_ENG;
            lang_title_key_f1 = TXT_TITLE_KEY_F1_ENG;
            lang_title_key_f2 = TXT_TITLE_KEY_F2_ENG;
            lang_title_press_any_key = TXT_TITLE_PRESS_ANY_KEY_ENG;
            lang_title_quote = txt_title_quotes_eng;
            lang_title_quote_size = sizeof(txt_title_quotes_eng) / sizeof(txt_title_quotes_eng[0]);
            
            lang_help_lines = txt_help_lines_eng;
            lang_help_lines_count = sizeof(txt_help_lines_eng) / sizeof(txt_help_lines_eng[0]);

            lang_game_score = TXT_GAME_SCORE_ENG;
            lang_game_bet = TXT_GAME_BET_ENG;
            lang_game_round = TXT_GAME_ROUND_ENG;
            lang_game_bud_bud_bud = TXT_GAME_BUD_BUD_BUD_ENG;
            lang_game_aaa_ooo_ooo = TXT_GAME_AAA_OOO_OOO_ENG;
            lang_game_hna = TXT_GAME_HNA_ENG;
            lang_game_win_quote = txt_game_win_quotes_eng;
            lang_game_quote_win_size = sizeof(txt_game_win_quotes_eng) / sizeof(txt_game_win_quotes_eng[0]);
            lang_game_loose_quote = txt_game_loose_quotes_eng;
            lang_game_quote_loose_size = sizeof(txt_game_loose_quotes_eng) / sizeof(txt_game_loose_quotes_eng[0]);

            lang_over_game = TXT_OVER_GAME_ENG;
            lang_over_rounds = TXT_OVER_ROUNDS_ENG;
            lang_over_max_score = TXT_OVER_MAX_SCORE_ENG;
            lang_over_enter = TXT_OVER_ENTER_ENG;
            break;

        case 1:  // Русский
            SDL_SetWindowTitle(window, TXT_TITLE_NAME_RUS);
            lang_title_name = TXT_TITLE_NAME_RUS;
            lang_title_version = TXT_TITLE_VERSION_RUS;
            lang_title_developed_by = TXT_TITLE_DEVELOPED_BY_RUS;
            lang_title_authors = TXT_TITLE_AUTHORS_RUS;
            lang_title_key_f1 = TXT_TITLE_KEY_F1_RUS;
            lang_title_key_f2 = TXT_TITLE_KEY_F2_RUS;
            lang_title_press_any_key = TXT_TITLE_PRESS_ANY_KEY_RUS;
            lang_title_quote = txt_title_quotes_rus;
            lang_title_quote_size = sizeof(txt_title_quotes_rus) / sizeof(txt_title_quotes_rus[0]);

            lang_help_lines = txt_help_lines_rus;
            lang_help_lines_count = sizeof(txt_help_lines_rus) / sizeof(txt_help_lines_rus[0]);

            lang_game_score = TXT_GAME_SCORE_RUS;
            lang_game_bet = TXT_GAME_BET_RUS;
            lang_game_round = TXT_GAME_ROUND_RUS;
            lang_game_bud_bud_bud = TXT_GAME_BUD_BUD_BUD_RUS;
            lang_game_aaa_ooo_ooo = TXT_GAME_AAA_OOO_OOO_RUS;
            lang_game_hna = TXT_GAME_HNA_RUS;
            lang_game_win_quote = txt_game_win_quotes_rus;
            lang_game_quote_win_size = sizeof(txt_game_win_quotes_rus) / sizeof(txt_game_win_quotes_rus[0]);
            lang_game_loose_quote = txt_game_loose_quotes_rus;
            lang_game_quote_loose_size = sizeof(txt_game_loose_quotes_rus) / sizeof(txt_game_loose_quotes_rus[0]);

            lang_over_game = TXT_OVER_GAME_RUS;
            lang_over_rounds = TXT_OVER_ROUNDS_RUS;
            lang_over_max_score = TXT_OVER_MAX_SCORE_RUS;
            lang_over_enter = TXT_OVER_ENTER_RUS;
            break;
    }
    // [PN] Выбираем новую цитату при смене языка
    G_GetRandomQuote(0);
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

    window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    
    // [PN] Загрузка шрифта, встроенного в код (font.c)
    SDL_RWops *rw = SDL_RWFromMem(ibm_vga_data, ibm_vga_data_len);
    font = TTF_OpenFontRW(rw, 0, FONT_SIZE);

    // [JN] Предопределяем языковые строки.
    D_SetLanguageStrings();
    // [PN] Выбираем первую случайную цитату
    G_GetRandomQuote(1);

    D_KenoLoop();
    
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}
