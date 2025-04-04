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
//  Инициализация цветов CGA.
//  https://en.wikipedia.org/wiki/Color_Graphics_Adapter#Standard_graphics_modes
// -----------------------------------------------------------------------------

void R_InitColors (void)
{
    // Индекс 0 всегда чёрного цвета.
    cga_color_0 = (SDL_Color){0, 0, 0, 255};
    
    static const SDL_Color cga_colors[4][3] = {
        // Палитра 0. Высокая интенсивность.
        {
            { 85, 255,  85, 255},  // light green
            {255,  85,  85, 255},  // light red
            {255, 255,  85, 255},  // yellow
        },
        // Палитра 0. Низкая интенсивность.
        {
            {  0, 170,   0, 255},  // green
            {170,   0,   0, 255},  // red
            {170,  85,   0, 255},  // brown
        },
        // Palette 1. Высокая интенсивность.
        {
            { 85, 255, 255, 255},  // light cyan
            {255,  85, 255, 255},  // light magenta
            {255, 255, 255, 255},  // white
        },
        // Palette 1. Низкая интенсивность.
        {
            {  0, 170, 170, 255},  // cyan
            {170,   0, 170, 255},  // magenta
            {170, 170, 170, 255},  // light gray
        },
    };

    cga_color_1 = cga_colors[color_scheme][0];
    cga_color_2 = cga_colors[color_scheme][1];
    cga_color_3 = cga_colors[color_scheme][2];
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

    // [PN] Вычисляем начальную позицию, округляя до ближайшего кратного 16
    const int raw_x = (SCREENWIDTH - surface->w) / 2;
    const int aligned_x = (raw_x / 16) * 16;

    SDL_Rect dest = { aligned_x, y, surface->w, surface->h };

    SDL_RenderCopy(renderer, texture, NULL, &dest);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

// -----------------------------------------------------------------------------
// R_DrawImage
//  [PN] Рисует BMP-картинку в заданных координатах. Пример использования: 
//  R_DrawImage("hna.bmp", 100, 50);
//  В данный момент не используется, оставлена как пример.
// -----------------------------------------------------------------------------

/*
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
*/

// -----------------------------------------------------------------------------
// R_DrawTitleScreen
//  Отрисовка титульного экрана.
// -----------------------------------------------------------------------------

static void R_DrawTitleScreen (void)
{
    R_DrawTextCentered("╔══════════════════════╗", 16, cga_color_3);
    R_DrawTextCentered("║                      ║", 32, cga_color_3);
    R_DrawTextCentered("║                      ║", 48, cga_color_3);
    R_DrawTextCentered("║                      ║", 64, cga_color_3);
    R_DrawTextCentered("╚══════════════════════╝", 80, cga_color_3);
    R_DrawTextCentered(lang_title_name, 48, cga_color_3);
    R_DrawTextCentered(lang_title_version, 112, cga_color_2);
    R_DrawTextCentered(G_GetTitleQuote(0), 144, cga_color_2);

    R_DrawTextCentered(lang_title_developed_by, 192, cga_color_3);
    R_DrawTextCentered(lang_title_authors, 224, cga_color_2);

    R_DrawText(lang_title_key_f1, 80, 288, cga_color_3);
    R_DrawText(lang_title_key_f2, 80, 320, cga_color_3);

    R_DrawTextCentered(lang_title_press_any_key, 368, cga_color_1);
}

// -----------------------------------------------------------------------------
// R_DrawHelpScreen
//  Отрисовка экрана помощи.
// -----------------------------------------------------------------------------

static void R_DrawHelpScreen (void)
{
    for (int i = 0; i < lang_help_lines_count; i++)
    {
        R_DrawText(lang_help_lines[i], 16, 16 + i * 16, cga_color_3);
    }
}

// -----------------------------------------------------------------------------
// R_DrawGameField
//  Отрисовка игрового поля.
// -----------------------------------------------------------------------------

static void R_DrawGameField (void)
{
    R_DrawTextCentered("╔══════════════════════╗",  16, cga_color_3);
    R_DrawTextCentered("║                      ║",  32, cga_color_3);
    R_DrawTextCentered("║                      ║",  48, cga_color_3);
    R_DrawTextCentered("║                      ║",  64, cga_color_3);
    R_DrawTextCentered("║                      ║",  80, cga_color_3);
    R_DrawTextCentered("║                      ║",  96, cga_color_3);
    R_DrawTextCentered("║                      ║", 112, cga_color_3);
    R_DrawTextCentered("║                      ║", 128, cga_color_3);
    R_DrawTextCentered("╚══════════════════════╝", 144, cga_color_3);

    R_DrawText(lang_game_score, 144, 48, cga_color_3);
    char scoreText[32];
    sprintf(scoreText, "%d", score);
    R_DrawText(scoreText, 352, 48, cga_color_3);

    R_DrawText(lang_game_bet, 144, 80, cga_color_3);
    char betText[32];
    sprintf(betText, "%d", bet);
    R_DrawText(betText, 352, 80, cga_color_3);

    R_DrawText(lang_game_round, 144, 112, cga_color_3);
    char roundText[32];
    sprintf(roundText, "%d", rounds);
    R_DrawText(roundText, 352, 112, cga_color_3);

    // [PN] Подсветка рамки с выбором
    const SDL_Color left_color = (isHoveringLeft || choice == 1) ? cga_color_3 : cga_color_2;
    R_DrawText("┌─────────────────┐", 16, 176, left_color);
    R_DrawText("│                 │", 16, 192, left_color);
    R_DrawText("│                 │", 16, 208, left_color);
    R_DrawText("│                 │", 16, 224, left_color);
    R_DrawText("└─────────────────┘", 16, 240, left_color);        
    R_DrawText(lang_game_bud_bud_bud, 48, 208, left_color);

    const SDL_Color right_color = (isHoveringRight || choice == 2) ? cga_color_3 : cga_color_2;
    R_DrawText("┌────────────────┐", 336, 176, right_color);
    R_DrawText("│                │", 336, 192, right_color);
    R_DrawText("│                │", 336, 208, right_color);
    R_DrawText("│                │", 336, 224, right_color);
    R_DrawText("└────────────────┘", 336, 240, right_color);
    R_DrawText(lang_game_aaa_ooo_ooo, 368, 208, right_color);

    if (gameHna)
    {
        R_DrawTextCentered("╔══════════╗", 272, cga_color_1);
        R_DrawTextCentered("║          ║", 288, cga_color_1);
        R_DrawTextCentered("║          ║", 304, cga_color_1);
        R_DrawTextCentered("║          ║", 320, cga_color_1);
        R_DrawTextCentered("╚══════════╝", 336, cga_color_1);
        R_DrawTextCentered(lang_game_hna, 304, cga_color_1);
    }

    if (resultQuoteIndex >= 0)
    {
        const char *quote = resultIsWin
            ? lang_game_win_quote[resultQuoteIndex]
            : lang_game_loose_quote[resultQuoteIndex];
    
        R_DrawTextCentered(quote, 368, resultIsWin ? cga_color_1 : cga_color_2);
    }
}

// -----------------------------------------------------------------------------
// R_DrawGameOverScreen
//  Отрисовка экрана Game Over.
// -----------------------------------------------------------------------------

static void R_DrawGameOverScreen (void)
{
    R_DrawTextCentered(lang_over_game, 144, cga_color_3);

    char roundsText[64];
    sprintf(roundsText, "%s %d", lang_over_rounds, rounds);
    R_DrawText(roundsText, 64, 192, cga_color_2);
    
    char scoreText[64];
    sprintf(scoreText, "%s %d", lang_over_max_score, maxScore);
    R_DrawText(scoreText, 64, 224, cga_color_2);

    R_DrawTextCentered(lang_over_enter, 304, cga_color_1);
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
    screen_refresh = false;
}
