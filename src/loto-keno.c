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


// Компиляция под Windows / MSYS (GCC):
//   windres loto-keno.rc -o loto-keno.o
//     gcc -O3 -s loto-keno.c font.c lang.c loto-keno.o -o loto-keno.exe  \
//     -lmingw32 -lSDL2main -lSDL2 -lSDL2_ttf -lgdi32 -mwindows
//
// Компиляция под Windows / MSYS (CMake):
//   Быстрая конфигурация:
//     cmake -G "Ninja" -D CMAKE_BUILD_TYPE="Release" -S . -B build
//   Оптимизированная конфигурация:
//     cmake -G "Ninja" -D CMAKE_BUILD_TYPE="Release" -D \
//     CMAKE_C_FLAGS_RELEASE="-O3 -s -flto -fno-math-errno \
//     -fomit-frame-pointer -funroll-loops -DNDEBUG" -S . -B build
//   Компиляция:
//     cmake --build build
//
// Компиляция под Windows / Build Tools + VCPKG:
//   Конфигурация:
//     make -B build_vs_64 -G "Ninja" -DCMAKE_BUILD_TYPE=Release \
//     DCMAKE_TOOLCHAIN_FILE=R:/VCPKG/scripts/buildsystems/vcpkg.cmake \
//     DCMAKE_PREFIX_PATH=R:/VCPKG/installed/x64-windows
//   Компиляция:
//     cmake --build build_vs_64 --config Release --parallel
//
// Компиляция под Linux:
//   gcc -O3 -s loto-keno.c font.c lang.c loto-keno.o -o loto-keno.exe \
//   -lSDL2main -lSDL2 -lSDL2_ttf


#include "loto-keno.h"


SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

int fullscreen = 0;
static int window_width = SCREENWIDTH;
static int window_height = SCREENHEIGHT;

int screen_refresh = 1;
int screen_visible = 1;

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
// HandleMouseEvents
//  Обработка событий мыши.
// -----------------------------------------------------------------------------

static void HandleMouseEvents (SDL_Event *event)
{
    static int mousePressed = 0; // [PN] Отслеживание нажатия кнопки мыши

    if (event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN)
    {
        // [PN] Обновить экран при движении мыши или нажатии кнопки
        screen_refresh = 1;
    }

    if (event->type == SDL_MOUSEWHEEL && gameStarted && !gameOver)
    {
        bet += (event->wheel.y > 0 && bet < score) ? 1 : 0; // [PN] Увеличиваем ставку
        bet -= (event->wheel.y < 0 && bet > 1) ? 1 : 0;     // [PN] Уменьшаем ставку
        // [JN] Обновить экран при активации колёсика мыши
        screen_refresh = 1;
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

// -----------------------------------------------------------------------------
// HandleMouseEvents
//  Обработка событий клавиатуры.
// -----------------------------------------------------------------------------

static void HandleKeyboardEvents (SDL_Event *event)
{
    if (event->type != SDL_KEYDOWN)
        return;

    SDL_Keycode key = event->key.keysym.sym;
    const Uint16 mod = event->key.keysym.mod;

    // [JN] Нажатие любой клавиши перерисовывает экран.
    screen_refresh = 1;

    if ((key == SDLK_RETURN || key == SDLK_KP_ENTER) && (mod & KMOD_ALT))
    {
        // [PN] Alt + Enter пойман, переключение в полноэкранный режим
        fullscreen ^= 1;
        SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    }

    if (key == SDLK_F1)  // [JN] Открытие экрана помощи
    {
        gameHelp ^= 1;
        return;
    }

    if (key == SDLK_F2)  // [JN] Смена языка
    {
        language ^= 1;
        L_SetLanguageStrings();
        return;
    }

    if (gameHelp)  // [JN] Закрытие экрана помощи
    {
        gameHelp = 0;
        return;
    }

    if (!gameStarted)
    {
        if (key == SDLK_ESCAPE) // [PN] Выход из игры
        {
            SaveConfig();
            exit(0); 
        }

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
// HandleWindowEvents
//  Обработка событий окна.
// -----------------------------------------------------------------------------

static void HandleWindowEvents (SDL_WindowEvent *event)
{
    if (event->type == SDL_WINDOWEVENT)
    {
        switch (event->event)
        {
            case SDL_WINDOWEVENT_MINIMIZED:
                screen_visible = 0;
                break;
    
            case SDL_WINDOWEVENT_RESTORED:
            case SDL_WINDOWEVENT_SHOWN:
                screen_visible = 1;
                break;

            case SDL_WINDOWEVENT_SIZE_CHANGED:
                if (!fullscreen)
                {
                    window_width = event->data1;
                    window_height = event->data2;
                }
                screen_refresh = 1;
                break;
        }
    }
}

// -----------------------------------------------------------------------------
// D_KenoLoop
//  Главный цикл игры.
// -----------------------------------------------------------------------------

static void D_KenoLoop (void)
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
            HandleWindowEvents(&event.window);
        }

        R_FinishUpdate();

        // [PN] Игра работает с фиксированной частотой 35 кадров в секунду — как в классическом Doom.
        // Задержка между кадрами вычисляется как 1000 мс / 35 ≈ 28.57 мс.
        // Мы используем SDL_Delay(28), чтобы поддерживать стабильный FPS и снизить нагрузку на процессор.
        SDL_Delay(28);
    }
}

// -----------------------------------------------------------------------------
// Основные циклы программы.
// -----------------------------------------------------------------------------

static void LoadConfig (void)
{
#ifdef _WIN32
    FILE *f = fopen("loto-keno.ini", "r");

    // [PN] Файл не найден — создаём с дефолтными значениями
    if (!f)
    {
        language = 0;
        fullscreen = 0;
        window_width = SCREENWIDTH;
        window_height = SCREENHEIGHT;
        return;
    }

    char key[64];
    int value;

    while (fscanf(f, "%63s %d", key, &value) == 2)
    {
        if (strcmp(key, "language") == 0)       language = value;
        if (strcmp(key, "fullscreen") == 0)     fullscreen = value;
        if (strcmp(key, "window_width") == 0)   window_width = value;
        if (strcmp(key, "window_height") == 0)  window_height = value;
    }

    fclose(f);
#endif
}

static void SaveConfig (void)
{
#ifdef _WIN32
    FILE *f = fopen("loto-keno.ini", "w");

    if (!f)
    {
        return;
    }

    fprintf(f, "language        %d\n",   language);
    fprintf(f, "fullscreen      %d\n",   fullscreen);
    fprintf(f, "window_width    %d\n",   window_width);
    fprintf(f, "window_height   %d\n",   window_height);
    fclose(f);
#endif
}


int main (int argc, char *argv[])
{
    int window_flags = 0, renderer_flags = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1)
    {
        return 1;
    }

    // [JN/PN] Создание консольного окна для вывода, если
    // предоставлен параметр коммандной строки "-console".
#ifdef _WIN32
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-console") == 0)
        {
            AllocConsole();
            SetConsoleTitle("Console");
            freopen("CONIN$", "r", stdin);
            freopen("CONOUT$", "w", stdout);
            freopen("CONOUT$", "w", stderr);
            SetConsoleOutputCP(CP_UTF8);
            SetConsoleCP(CP_UTF8);
            break;
        }
    }
#endif

    LoadConfig();

    window_flags = SDL_WINDOW_RESIZABLE;
    window_flags |= SDL_WINDOW_ALLOW_HIGHDPI;
    if (fullscreen)
    window_flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;

    window = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                              window_width, window_height, window_flags);

    // [JN] Минимальный размер окна
    SDL_SetWindowMinimumSize(window, SCREENWIDTH/2, SCREENHEIGHT/2);

    renderer_flags = SDL_RENDERER_TARGETTEXTURE;
    renderer_flags |= SDL_RENDERER_PRESENTVSYNC;
    renderer = SDL_CreateRenderer(window, -1, renderer_flags);

    SDL_RenderSetLogicalSize(renderer, SCREENWIDTH, SCREENHEIGHT);

    // [PN] Загрузка шрифта, встроенного в код (font.c)
    SDL_RWops *rw = SDL_RWFromMem(ibm_vga_data, ibm_vga_data_len);
    font = TTF_OpenFontRW(rw, 0, FONT_SIZE);

    // [JN] Предопределяем языковые строки.
    L_SetLanguageStrings();
    // [PN] Выбираем первую случайную цитату
    G_GetTitleQuote(1);

    D_KenoLoop();
    
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    
    SaveConfig();
    
    return 0;
}
