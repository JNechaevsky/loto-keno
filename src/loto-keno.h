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

#ifdef _WIN32
#include <windows.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>


// =============================================================================
//
// DRAW.C
//
// =============================================================================

extern void R_InitColors (void);
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
extern const char *lang_over_loose;
extern const char *lang_over_win;
extern const char *lang_over_rounds;
extern const char *lang_over_max_score;
extern const char *lang_over_enter;

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

extern bool screen_refresh;
extern bool screen_visible;

extern bool isHoveringLeft;
extern bool isHoveringRight;

extern SDL_Color cga_color_0;
extern SDL_Color cga_color_1;
extern SDL_Color cga_color_2;
extern SDL_Color cga_color_3;

extern int language;
extern int color_scheme;

extern int score;
extern int maxScore;
extern int bet;
extern int rounds;
extern int choice;

extern bool gameHna;
extern bool gameStarted;
extern bool gameHelp;
extern int  gameOver;
