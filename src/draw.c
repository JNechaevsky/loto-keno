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


// -----------------------------------------------------------------------------
// R_InitColors
//  Инициализация цветов
// -----------------------------------------------------------------------------

static SDL_Color R_ApplyColor (Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
    // [JN] Прямое возвращение составного литерала.
    return (SDL_Color){r, g, b, a};
}

void R_InitColors (void)
{
    // Базовые цвета (не зависят от режима)
    color_white = R_ApplyColor(255, 255, 255, 255);
    color_black = R_ApplyColor(0, 0, 0, 255);

    // Цветовые схемы
    static const SDL_Color schemes[3][6] = {
        // Чёрно-белый (0)
        {
            {255,255,255,255},  // cyan
            {255,255,255,255},  // magenta
            {255,255,255,255},  // gray
            {255,255,255,255},  // yellow
            {255,255,255,255},  // blue
            {255,255,255,255}   // red
        },
        // CGA (1)
        {
            {  0,170,170,255},    // cyan
            {170,  0,170,255},    // magenta
            {255,255,255,255},    // gray
            {  0,170,170,255},    // yellow
            {  0,170,170,255},    // blue
            {  0,170,170,255}     // red
        },
        // EGA (2)
        {
            {  0,170,170,255},    // cyan
            {170,  0,170,255},    // magenta
            { 85, 85, 85,255},    // gray
            {255,255, 85,255},    // yellow
            {  0,  0,170,255},    // blue
            {170,  0,  0,255}     // red
        }
    };

    color_cyan    = schemes[colors][0];
    color_magenta = schemes[colors][1];
    color_gray    = schemes[colors][2];
    color_yellow  = schemes[colors][3];
    color_blue    = schemes[colors][4];
    color_red     = schemes[colors][5];
}

// -----------------------------------------------------------------------------
// R_DrawText
//  Отрисовка текстовой строчки.
// -----------------------------------------------------------------------------

static void R_DrawText (const char *text, int x, int y, SDL_Color color)
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
//  Отрисовка центрированной текстовой строчки.
// -----------------------------------------------------------------------------

static void R_DrawTextCentered (const char *text, int y, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderUTF8_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect dest = { (SCREENWIDTH - surface->w) / 2, y, surface->w, surface->h };

    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// -----------------------------------------------------------------------------
// R_DrawImage
//  [PN] Рисует BMP-картинку в заданных координатах. Пример использования: 
//  R_DrawImage("hna.bmp", 100, 50);
// -----------------------------------------------------------------------------
static void R_DrawImage (const char *path, int x, int y)
{
    SDL_Surface *surface = SDL_LoadBMP(path);
    // if (!surface) {
    //     printf("R_DrawImage: Failed to load BMP: %s\n", SDL_GetError());
    //     return;
    // }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // if (!texture) {
    //     printf("R_DrawImage: Failed to create texture: %s\n", SDL_GetError());
    //     return;
    // }

    SDL_Rect dst = { x, y, 0, 0 };
    SDL_QueryTexture(texture, NULL, NULL, &dst.w, &dst.h); // Получить размеры

    SDL_RenderCopy(renderer, texture, NULL, &dst);
    SDL_DestroyTexture(texture);
}

// -----------------------------------------------------------------------------
// R_DrawTitleScreen
//  Отрисовка титульного экрана.
// -----------------------------------------------------------------------------

static void R_DrawTitleScreen (void)
{
    R_DrawTextCentered("╔══════════════════════╗", 16, color_white);
    R_DrawTextCentered("║                      ║", 32, color_white);
    R_DrawTextCentered("║                      ║", 48, color_white);
    R_DrawTextCentered("║                      ║", 64, color_white);
    R_DrawTextCentered("╚══════════════════════╝", 80, color_white);
    R_DrawTextCentered(lang_title_name, 48, color_white);
    R_DrawTextCentered(lang_title_version, 112, color_magenta);
    R_DrawTextCentered(G_GetTitleQuote(0), 144, color_magenta);

    R_DrawTextCentered(lang_title_developed_by, 192, color_white);
    R_DrawTextCentered(lang_title_authors, 224, color_magenta);

    R_DrawText(lang_title_key_f1, 160, 288, color_white);
    R_DrawText(lang_title_key_f2, 160, 320, color_white);

    R_DrawTextCentered(lang_title_press_any_key, 368, color_white);
}

// -----------------------------------------------------------------------------
// R_DrawHelpScreen
//  Отрисовка экрана помощи.
// -----------------------------------------------------------------------------

static void R_DrawHelpScreen (void)
{
    for (int i = 0; i < lang_help_lines_count; i++)
    {
        R_DrawText(lang_help_lines[i], 16, 16 + i * 16, color_white);
    }
}

// -----------------------------------------------------------------------------
// R_DrawGameField
//  Отрисовка игрового поля.
// -----------------------------------------------------------------------------

static void R_DrawGameField (void)
{
    R_DrawTextCentered("╔══════════════════════╗",  16, color_white);
    R_DrawTextCentered("║                      ║",  32, color_white);
    R_DrawTextCentered("║                      ║",  48, color_white);
    R_DrawTextCentered("║                      ║",  64, color_white);
    R_DrawTextCentered("║                      ║",  80, color_white);
    R_DrawTextCentered("║                      ║",  96, color_white);
    R_DrawTextCentered("║                      ║", 112, color_white);
    R_DrawTextCentered("║                      ║", 128, color_white);
    R_DrawTextCentered("╚══════════════════════╝", 144, color_white);

    R_DrawText(lang_game_score, 144, 48, color_white);
    char scoreText[32];
    sprintf(scoreText, "%d", score);
    R_DrawText(scoreText, 352, 48, color_white);

    R_DrawText(lang_game_bet, 144, 80, color_white);
    char betText[32];
    sprintf(betText, "%d", bet);
    R_DrawText(betText, 352, 80, color_white);

    R_DrawText(lang_game_round, 144, 112, color_white);
    char roundText[32];
    sprintf(roundText, "%d", rounds);
    R_DrawText(roundText, 352, 112, color_white);

    // [PN] Обработка событий мыши
    int realX, realY;
    float mouseX, mouseY;
    SDL_GetMouseState(&realX, &realY);
    SDL_RenderWindowToLogical(renderer, (float)realX, (float)realY, &mouseX, &mouseY);

    isHoveringLeft = (mouseX >= 16 && mouseX <= 320 && mouseY >= 176 && mouseY <= 256);
    isHoveringRight = (mouseX >= 336 && mouseX <= 624 && mouseY >= 176 && mouseY <= 256);
    const SDL_Color left_color = (isHoveringLeft || choice == 1) ? color_white : color_magenta;
    const SDL_Color right_color = (isHoveringRight || choice == 2) ? color_white : color_magenta;


    if ((isHoveringLeft || choice == 1) && colors == 0)
    {
        R_DrawText("╔═════════════════╗", 16, 176, left_color);
        R_DrawText("║                 ║", 16, 192, left_color);
        R_DrawText("║                 ║", 16, 208, left_color);
        R_DrawText("║                 ║", 16, 224, left_color);
        R_DrawText("╚═════════════════╝", 16, 240, left_color);
    }
    else
    {
        R_DrawText("┌─────────────────┐", 16, 176, left_color);
        R_DrawText("│                 │", 16, 192, left_color);
        R_DrawText("│                 │", 16, 208, left_color);
        R_DrawText("│                 │", 16, 224, left_color);
        R_DrawText("└─────────────────┘", 16, 240, left_color);        
    }

    R_DrawText(lang_game_bud_bud_bud, 48, 208, left_color);

    if ((isHoveringRight || choice == 2) && colors == 0)
    {
        R_DrawText("╔════════════════╗", 336, 176, right_color);
        R_DrawText("║                ║", 336, 192, right_color);
        R_DrawText("║                ║", 336, 208, right_color);
        R_DrawText("║                ║", 336, 224, right_color);
        R_DrawText("╚════════════════╝", 336, 240, right_color);
    }
    else
    {
        R_DrawText("┌────────────────┐", 336, 176, right_color);
        R_DrawText("│                │", 336, 192, right_color);
        R_DrawText("│                │", 336, 208, right_color);
        R_DrawText("│                │", 336, 224, right_color);
        R_DrawText("└────────────────┘", 336, 240, right_color);
    }

    R_DrawText(lang_game_aaa_ooo_ooo, 368, 208, right_color);

    if (samuraiAppeared)
    {
        R_DrawTextCentered("╔══════════╗", 272, color_yellow);
        R_DrawTextCentered("║          ║", 288, color_yellow);
        R_DrawTextCentered("║          ║", 304, color_yellow);
        R_DrawTextCentered("║          ║", 320, color_yellow);
        R_DrawTextCentered("╚══════════╝", 336, color_yellow);
        R_DrawTextCentered(lang_game_hna, 304, color_yellow);
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

// -----------------------------------------------------------------------------
// R_DrawGameOverScreen
//  Отрисовка экрана Game Over.
// -----------------------------------------------------------------------------

static void R_DrawGameOverScreen (void)
{
    R_DrawTextCentered(lang_over_game, 160, color_white);

    char roundsText[64];
    sprintf(roundsText, "%s %d", lang_over_rounds, rounds);
    R_DrawText(roundsText, 64, 192, color_white);
    
    char scoreText[64];
    sprintf(scoreText, "%s %d", lang_over_max_score, maxScore);
    R_DrawText(scoreText, 64, 224, color_white);

    R_DrawTextCentered(lang_over_enter, 304, color_cyan);
}

// -----------------------------------------------------------------------------
// R_FinishUpdate
//  Вызов функций отрисовки и осуществление блиттинга, если окно не свёрнуто.
// -----------------------------------------------------------------------------

void R_FinishUpdate (void)
{
    if (!screen_refresh || !screen_visible)
        return;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    if (gameHelp)
        R_DrawHelpScreen();
    else if (!gameStarted)
        R_DrawTitleScreen();
    else if (gameOver)
        R_DrawGameOverScreen();
    else
        R_DrawGameField();

    SDL_RenderPresent(renderer);

    // [JN] Отрисовка завершена, не осуществлять перерисовку
    // до следующего нажатия клавиш или действия мышью.
    screen_refresh = 0;
}
