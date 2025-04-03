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
// G_GetTitleQuote
//  Возвращает случайную цитату из списка.
// -----------------------------------------------------------------------------

const char *G_GetTitleQuote (int randomize)
{
    static int randomQuoteIndex = 0;

    if (randomize)
        randomQuoteIndex = rand() % lang_title_quote_size;

    return lang_title_quote[randomQuoteIndex];
}

static const char *G_GetWinQuote (int randomize)
{
    static int randomWinQuoteIndex = 0;

    if (randomize)
        randomWinQuoteIndex = rand() % lang_game_quote_win_size;

    return lang_game_win_quote[randomWinQuoteIndex];
}

static const char *G_GetLooseQuote (int randomize)
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
    G_GetTitleQuote(1); // [PN] Обновляем цитату при старте новой игры
    resultQuoteIndex = -1;
}

// -----------------------------------------------------------------------------
// G_DetermineResult
//  Start new round.
// -----------------------------------------------------------------------------

void G_StartNewRound (void)
{
    // [PN] 1 к 10 шанс появления самурая в начале раунда
    gameHna = (rand() % SAMURAI_CHANCE == 0);
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
    const int multiplier = gameHna ? 2 : 1;
    const int win = (choice == 1 && isEven) || (choice == 2 && !isEven);
    
    if (win)
    {
        score += bet * multiplier;
        // [PN] Обновляем maxScore, если score стал больше него:
        if (score > maxScore)
        {
            maxScore = score;
        }
        resultQuoteIndex = rand() % lang_game_quote_win_size;
        resultIsWin = 1; // [PN] 💙 вот это важно!
    }
    else
    {
        score -= bet * multiplier;
        // [JN] В случае проигрыша, ставка не должна превышать текущий счёт!
        if (bet > score)
        {
            bet = score;
        }
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
