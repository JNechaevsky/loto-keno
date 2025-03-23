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


// Title screen
void D_SetLanguageStrings (void);
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


