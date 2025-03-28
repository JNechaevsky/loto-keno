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
#include "icon.c"


SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

// Переменные для конфигурационного файла
int language = 0; // [JN] 0 = English, 1 = Русский
static int fullscreen = 0;
int colors = 1; // [JN] 0 = B&W, 1 = CGA, 2 = EGA
static int window_width = SCREENWIDTH;
static int window_height = SCREENHEIGHT;

int screen_refresh = 1;
int screen_visible = 1;

int isHoveringLeft;
int isHoveringRight;

int score = 10;
int maxScore = 10; // [PN] Начальное значение соответствует стартовому счёту
int bet = 1; // Запоминаем последнюю ставку
int rounds = 0; // [JN] Means "round", but it's C reserved word.
int choice = 0; // 1 = Будь-будь-будь, 2 = А-ООО!
int samuraiAppeared = 0; // Был ли самурай в этом раунде
int gameOver = 0; // Флаг окончания игры
int gameStarted = 0; // Флаг начала игры
int gameHelp = 0; // [JN] Are we in help screen?

// Цвета B&W
SDL_Color color_white;
SDL_Color color_black;
// Цвета CGA
SDL_Color color_cyan;
SDL_Color color_magenta;
// Цвета EGA
SDL_Color color_blue;
SDL_Color color_red;
SDL_Color color_gray;
SDL_Color color_yellow;

SDL_Color resultColor;

// [JN] Значения координат для сетки моноширного шрифта.
// Один символ занимает 16х16 пикселей, поле CGA равно 40x25 символов.
//   0  16  32  48  64  80  96 112 128 144 160 176 192
//     208 224 240 256 272 288 304 320 336 352 368 384 


// -----------------------------------------------------------------------------
// HandleMouseEvents
//  Обработка событий мыши.
// -----------------------------------------------------------------------------

static void HandleMouseEvents (SDL_Event *event)
{
    static int mousePressed = 0; // [PN] Отслеживание нажатия кнопки мыши

    // [PN] Обновить экран при движении мыши, нажатии кнопки или скролле
    if (event->type == SDL_MOUSEMOTION || event->type == SDL_MOUSEBUTTONDOWN
    || (event->type == SDL_MOUSEWHEEL && gameStarted && !gameOver))
    {
        screen_refresh = 1;
    }

    // [PN] Обработка колёсика мыши (ставка)
    if (event->type == SDL_MOUSEWHEEL && gameStarted && !gameOver)
    {
        bet += (event->wheel.y > 0 && bet < score) ? 1 : 0; // Увеличиваем ставку
        bet -= (event->wheel.y < 0 && bet > 1) ? 1 : 0;     // Уменьшаем ставку
    }

    // [PN] Обработка нажатия ЛКМ
    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT && !mousePressed)
    {
        mousePressed = 1; // [PN] Фиксируем нажатие кнопки

        if (gameHelp) // [JN] Закрываем экран помощи
        {
            gameHelp = 0;
            return;
        }

        if (!gameStarted) // [PN] Стартуем игру
        {
            gameStarted = 1;
            G_StartNewRound();
            return;
        }

        if (gameOver) // [PN] Перезапуск игры
        {
            G_ResetGame();
            gameStarted = 1;
            G_StartNewRound();
            return;
        }

        if (bet > 0) // [PN] Определяем выбор и результат
        {
            choice = isHoveringLeft ? 1 : isHoveringRight ? 2 : 0;
            if (choice > 0)
                G_DetermineResult();
        }
    }

    // [PN] Сбрасываем нажатие ЛКМ
    if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT)
    {
        mousePressed = 0;
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

            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                if (!fullscreen)
                {
                    // [PN] Проверка, максимально ли окно в данный момент:
                    Uint32 flags = SDL_GetWindowFlags(window);
                    if (flags & SDL_WINDOW_MAXIMIZED)
                    {
                        // [PN] Если окно максимизировано, пропорцию не трогаем,
                        // просто сохраняем размеры как есть:
                        window_width = event->data1;
                        window_height = event->data2;
                    }
                    else
                    {
                        const float aspect_ratio = (float)SCREENWIDTH / SCREENHEIGHT;
                        int new_width = event->data1;
                        int new_height = event->data2;
                        const int delta_width = abs(new_width - window_width);
                        const int delta_height = abs(new_height - window_height);

                        if (delta_width > delta_height) // [PN] изменили ширину
                        {
                            new_height = (int)(new_width / aspect_ratio);
                        }
                        else // [PN] изменили высоту
                        {
                            new_width = (int)(new_height * aspect_ratio);
                        }

                        SDL_SetWindowSize(window, new_width, new_height);

                        window_width = new_width;
                        window_height = new_height;
                    }
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
// Функции файла с настройками.
// -----------------------------------------------------------------------------

static const char *GetConfigPath (void)
{
#ifdef _WIN32
    // [PN] Для Windows — просто текущий каталог.
    return "loto-keno.ini";
#else
    const char* homeDir = getenv("HOME");
    if (!homeDir) homeDir = ".";

    static char path[256];
    snprintf(path, sizeof(path), "%s/.local/share/loto-keno", homeDir);

    // [PN] Проверяем и создаём каталог, если его ещё нет
    struct stat st = {0};
    if (stat(path, &st) == -1) {
        mkdir(path, 0700);
    }

    static char fullPath[512];
    snprintf(fullPath, sizeof(fullPath), "%s/loto-keno.ini", path);
    return fullPath;
#endif
}

static void LoadConfig (void)
{
    FILE *file = fopen(GetConfigPath(), "r");

    if (file)
    {
        if (fscanf(file, "language        %d\n", &language) != 1)      language = 0;
        if (fscanf(file, "fullscreen      %d\n", &fullscreen) != 1)    fullscreen = 0;
        if (fscanf(file, "colors          %d\n", &colors) != 1)        colors = 1;
        if (fscanf(file, "window_width    %d\n", &window_width) != 1)  window_width = SCREENWIDTH;
        if (fscanf(file, "window_height   %d\n", &window_height) != 1) window_height = SCREENHEIGHT;
        fclose(file);
    }
}

static void SaveConfig (void)
{
    FILE *file = fopen(GetConfigPath(), "w");

    if (file)
    {
        fprintf(file, "language        %d\n",   language);
        fprintf(file, "fullscreen      %d\n",   fullscreen);
        fprintf(file, "colors          %d\n",   colors);
        fprintf(file, "window_width    %d\n",   window_width);
        fprintf(file, "window_height   %d\n",   window_height);
        fclose(file);
    }
}

// -----------------------------------------------------------------------------
// Основной цикл программы.
// -----------------------------------------------------------------------------

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

    // [JN] Присвоение иконки для окна.
    // Не требуется для Windows, т.к. используется .ico файл из loto-keno.rc
#ifndef _WIN32
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void *) icon_data, icon_w, icon_h,
                                                    32, icon_w * 4,
                                                    0xffu << 24, 0xffu << 16,
                                                    0xffu << 8, 0xffu << 0);

    SDL_SetWindowIcon(window, surface);
    SDL_FreeSurface(surface);
#endif

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

    // [JN] Инициализация цветовой схемы.
    R_InitColors();

    D_KenoLoop();
    
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    
    SaveConfig();
    
    return 0;
}
