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



// Компилировать так:
//  gcc loto-keno.c -o loto-keno.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lgdi32 -mwindows

// Компилировать так:
//  windres loto-keno.rc -o loto-keno.o 
//  gcc loto-keno.c loto-keno.o -o loto-keno.exe -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lgdi32 -mwindows


#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


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
int score = 10;
int bet = 1; // Запоминаем последнюю ставку
int lap = 0; // [JN] Means "round", but it's C reserved word.
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
    lap++;
}

// -----------------------------------------------------------------------------
// G_DetermineResult
//  Determine result of current round.
// -----------------------------------------------------------------------------

void G_DetermineResult (void)
{
    int randomNumber = rand() % 100; // [PN] Число от 0 до 99
    int isEven = (randomNumber % 2 == 0);
    int win = (choice == 1 && isEven) || (choice == 2 && !isEven);
    
    if (win)
    {
        score += bet * (samuraiAppeared ? 2 : 1);
    }
    else
    {
        score -= bet * (samuraiAppeared ? 2 : 1);
        // [JN] В случае проигрыша, ставка не должна превышать текущий счёт!
        if (bet > score)
        {
            bet = score;
        }
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
    R_DrawTextCentered("║      Лото Кено!      ║", 48, white);
    R_DrawTextCentered("║                      ║", 64, white);
    R_DrawTextCentered("╚══════════════════════╝", 80, white);

    R_DrawTextCentered("Версия 1.0 (18.03.2025)", 112, magenta);

    R_DrawTextCentered("Разработка и идея:", 160, white);
    R_DrawTextCentered("Полина \"Аура\" Н. ♥ Юлия Нечаевская", 192, magenta);

    R_DrawTextCentered("F1 - Помощь и правила", 240, white);

/*
    R_DrawTextCentered("Лото Кено!", 16, white);
    R_DrawTextCentered("Версия 1.0 (18.03.2025)", 48, magenta);
    R_DrawTextCentered("Разработка и идея:", 96, white);
    R_DrawTextCentered("Полина \"Аура\" Н. ♥ Юлия Нечаевская", 128, magenta);
    
    R_DrawTextCentered("F1 — помощь и правила", 208, magenta);
    // R_DrawTextCentered("Управление:", 176, white);
    // R_DrawTextCentered("← Будь-будь-будь!", 208, magenta);
    // R_DrawTextCentered("→ А-ООО!", 240, magenta);
    // R_DrawTextCentered("↑ / ↓ Изменить ставку", 272, magenta);
    // R_DrawTextCentered("ENTER — выбор", 304, magenta);
*/

    R_DrawTextCentered("Нажмите любую клавишу...", 352, white);
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
    R_DrawTextCentered("ЛОТО КЕНО ЗАКОНЧЕНО", 176, white);
    R_DrawTextCentered("Нажмите ENTER для перезапуска", 208, cyan);
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
    sprintf(roundText, "Раунд: %d", lap);
    R_DrawText(roundText, 240, 112, white);


    const SDL_Color left_color = (choice == 1 ? white : magenta);
    R_DrawText("┌─────────────────┐", 16, 176, left_color);
    R_DrawText("│                 │", 16, 192, left_color);
    R_DrawText("│                 │", 16, 208, left_color);
    R_DrawText("│                 │", 16, 224, left_color);
    R_DrawText("└─────────────────┘", 16, 240, left_color);

    R_DrawText("Будь-будь-будь!", 48, 208, left_color);

    const SDL_Color right_color = (choice == 2 ? white : magenta);
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

    // [JN] TODO - Win/Loose string?
    R_DrawTextCentered("Какой-нибудь текст", 368, gray);
}

// -----------------------------------------------------------------------------
// D_KenoLoop
//  Main game loop.
// -----------------------------------------------------------------------------

void D_KenoLoop (void)
{
    int running = 1;
    SDL_Event event;
    srand(time(NULL));
    
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
            {
                running = 0;
                return;
            }

            if (event.type == SDL_KEYDOWN)
            {
                // [JN] Help screen can be invoked in any game state.
                // Not in help screen? Open it. Otherwise, close it.
                if (event.key.keysym.sym == SDLK_F1)
                {
                    gameHelp ^= 1;
                }
                else
                {
                    // [JN] Put away help screen by pressing any key.
                    if (gameHelp)
                    {
                        gameHelp = 0;
                        continue;
                    }
                    
                    if (!gameStarted)
                    {
                        gameStarted = 1;
                        G_StartNewRound();
                    }
                    else if (gameOver)
                    {
                        if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)
                        {
                            score = 10;
                            bet = 1;
                            lap = 0;
                            gameOver = 0;
                            G_StartNewRound();
                        }
                    }
                    else
                    {
                        if (event.key.keysym.sym == SDLK_UP && bet < score)
                        {
                            bet++;
                        }
                        if (event.key.keysym.sym == SDLK_DOWN && bet > 1)
                        {
                            bet--;
                        }
                        if (event.key.keysym.sym == SDLK_LEFT && bet > 0)
                        {
                            choice = 1;
                        }
                        if (event.key.keysym.sym == SDLK_RIGHT && bet > 0)
                        {
                            choice = 2;
                        }
                        if ((event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) && choice > 0)
                        {
                            G_DetermineResult();
                        }
                        // [JN] Quit to title screen by pressing ESC.
                        if (event.key.keysym.sym == SDLK_ESCAPE)
                        {
                            gameStarted = 0;
                            score = 10;
                            bet = 1;
                            lap = 0;
                            gameOver = 0;
                        }
                    }
                }
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        if (gameHelp)
        {
            D_DrawHelpScreen();
        }
        else
        {
            if (!gameStarted)
            {
                D_DrawTitleScreen();
            }
            else if (gameOver)
            {
                D_DrawGameOverScreen();
            }
            else
            {
                D_DrawGameField();
            }
        }
        
        SDL_RenderPresent(renderer);

        // [PN] The game runs at a fixed 35 FPS, just like the classic Doom.
        // The frame delay is calculated as 1000 ms / 35 ≈ 28.57 ms.
        // We use SDL_Delay(28) to maintain a stable frame rate and reduce CPU usage.
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
    font = TTF_OpenFont("IBM_VGA_8x8.ttf", FONT_SIZE);

    D_KenoLoop();
    
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}
