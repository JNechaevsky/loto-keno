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
int choice = 0; // 1 = Будь-будь-будь, 2 = А-ООО!
int samuraiAppeared = 0; // Был ли самурай в этом раунде
int gameOver = 0; // Флаг окончания игры
int gameStarted = 0; // Флаг начала игры



/*
SDL_Color white  = {255, 255, 255, 255};
SDL_Color blue   = {85, 85, 255, 255};
SDL_Color cyan   = {0, 170, 170, 255};
SDL_Color purple = {170, 0, 170, 255};
SDL_Color yellow = {255, 255, 85, 255};
*/

SDL_Color white   = {255, 255, 255, 255};
SDL_Color black   = {  0,   0,   0, 255};
SDL_Color cyan    = {  0, 170, 170, 255};
SDL_Color magenta = {170,   0, 170, 255};

SDL_Color yellow  = {255, 255, 85, 255};



void renderText (const char *text, int x, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderUTF8_Blended(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void renderTextCentered(const char *text, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderUTF8_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = { (SCREENWIDTH - surface->w) / 2, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void startNewRound()
{
    samuraiAppeared = (rand() % SAMURAI_CHANCE == 0); // [PN] 1 к 10 шанс появления самурая в начале раунда
    choice = 0;
}

void determineResult()
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
            bet = score;
    }
    
    if (score <= 0)
    {
        gameOver = 1;
    }
    else
    {
        startNewRound();
    }
}

void gameLoop (void)
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
                if (!gameStarted)
                {
                    gameStarted = 1;
                    startNewRound();
                }
                else if (gameOver)
                {
                    if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER)
                    {
                        score = 10;
                        bet = 1;
                        gameOver = 0;
                        startNewRound();
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
                        determineResult();
                    }
                }
            }
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        if (!gameStarted)
        {
            renderTextCentered("Лото Кено!", 16, white);
            renderTextCentered("Версия 1.0 (18.03.2025)", 48, magenta);

            renderTextCentered("Разработка и идея:", 96, white);
            renderTextCentered("Полина \"Аура\" Н. ♥ Юлия Нечаевская", 128, magenta);

            renderTextCentered("Управление:", 176, white);
            renderTextCentered("← Будь-будь-будь!", 208, magenta);
            renderTextCentered("→ А-ООО!", 240, magenta);
            renderTextCentered("↑ / ↓ Изменить ставку", 272, magenta);
            renderTextCentered("ENTER — выбор", 304, magenta);

            renderTextCentered("Нажмите любую клавишу...", 352, white);

            /*
            renderText("1111111111111111111111111111111111111111", 0, 0, blue);
            renderText("2222222222222222222222222222222222222222", 0, 16, blue);
            renderText("3333333333333333333333333333333333333333", 0, 32, blue);
            renderText("4444444444444444444444444444444444444444", 0, 48, blue);
            renderText("5555555555555555555555555555555555555555", 0, 64, blue);
            renderText("6666666666666666666666666666666666666666", 0, 80, blue);
            renderText("7777777777777777777777777777777777777777", 0, 96, blue);
            renderText("8888888888888888888888888888888888888888", 0, 112, blue);
            renderText("9999999999999999999999999999999999999999", 0, 128, blue);
            renderText("0000000000000000000000000000000000000000", 0, 144, blue);
            renderText("1111111111111111111111111111111111111111", 0, 160, blue);
            renderText("2222222222222222222222222222222222222222", 0, 176, blue);
            renderText("3333333333333333333333333333333333333333", 0, 192, blue);
            renderText("4444444444444444444444444444444444444444", 0, 208, blue);
            renderText("5555555555555555555555555555555555555555", 0, 224, blue);
            renderText("6666666666666666666666666666666666666666", 0, 240, blue);
            renderText("7777777777777777777777777777777777777777", 0, 256, blue);
            renderText("8888888888888888888888888888888888888888", 0, 272, blue);
            renderText("9999999999999999999999999999999999999999", 0, 288, blue);
            renderText("0000000000000000000000000000000000000000", 0, 304, blue);
            renderText("1111111111111111111111111111111111111111", 0, 320, blue);
            renderText("2222222222222222222222222222222222222222", 0, 336, blue);
            renderText("3333333333333333333333333333333333333333", 0, 352, blue);
            renderText("4444444444444444444444444444444444444444", 0, 368, blue);
            renderText("5555555555555555555555555555555555555555", 0, 384, blue);
            */
            
            // renderText("AA", 0, 0, blue);
            // renderText("AA", 0, 20, blue);
            // renderText("Нажмите любую клавишу для начала", SCREENWIDTH / 2 - 200, SCREENHEIGHT / 2 + 180, blue);
        }
        else if (gameOver)
        {
            renderTextCentered("ЛОТО КЕНО ЗАКОНЧЕНО", 176, white);
            renderTextCentered("Нажмите ENTER для перезапуска", 208, cyan);
        }
        else
        {
            renderText("Будь-будь-будь!", 50, 192, choice == 1 ? white : magenta);
            renderText("А-ООО!", 432, 192, choice == 2 ? white : magenta);
            
            char scoreText[32];
            sprintf(scoreText, "Очки: %d", score);
            renderText(scoreText, 240, 20, white);

            char betText[32];
            sprintf(betText, "Ставка: %d", bet);
            renderText(betText, 208, 50, white);

            if (samuraiAppeared)
            {
                renderTextCentered("╔══════════╗", 336, yellow);
                renderTextCentered("║   ХНА!   ║", 352, yellow);
                renderTextCentered("╚══════════╝", 368, yellow);
            }
        }
        
        SDL_RenderPresent(renderer);

        // [PN] The game runs at a fixed 35 FPS, just like the classic Doom.
        // The frame delay is calculated as 1000 ms / 35 ≈ 28.57 ms.
        // We use SDL_Delay(28) to maintain a stable frame rate and reduce CPU usage.
        SDL_Delay(28);
    }
}

int main (int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1)
    {
        return 1;
    }

    window = SDL_CreateWindow("Лото Кено!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREENWIDTH, SCREENHEIGHT, SDL_WINDOW_ALLOW_HIGHDPI);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    font = TTF_OpenFont("IBM_VGA_8x8.ttf", FONT_SIZE);

    gameLoop();
    
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    
    return 0;
}
