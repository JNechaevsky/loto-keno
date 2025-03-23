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


// Title screen
const char *lang_title_name;
const char *lang_title_version;
const char *lang_title_developed_by;
const char *lang_title_authors;
const char *lang_title_key_f1;
const char *lang_title_key_f2;
const char *lang_title_press_any_key;

const char **lang_title_quote;
int lang_title_quote_size;

// Help 
const char **lang_help_lines;
int lang_help_lines_count;

// Game field
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

// Game over
const char *lang_over_game;
const char *lang_over_rounds;
const char *lang_over_max_score;
const char *lang_over_enter;


void L_SetLanguageStrings (void)
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
    G_GetTitleQuote(0);
}
