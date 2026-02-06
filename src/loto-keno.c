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

/*
Компиляция под Windows / MSYS (GCC):
  windres loto-keno.rc -o loto-keno.o
    gcc -O3 -s loto-keno.c font.c lang.c loto-keno.o -o loto-keno.exe  \
    -lmingw32 -lSDL3main -lSDL3 -lSDL3_ttf -lgdi32 -mwindows

Компиляция под Windows / MSYS (CMake):
  Быстрая конфигурация:
    cmake -G "Ninja" -D CMAKE_BUILD_TYPE="Release" -S . -B build
  Оптимизированная конфигурация:
    cmake -G "Ninja" -D CMAKE_BUILD_TYPE="Release" -D \
    CMAKE_C_FLAGS_RELEASE="-O3 -s -flto -fno-math-errno \
    -fomit-frame-pointer -funroll-loops -DNDEBUG" -S . -B build
  Компиляция:
    cmake --build build

Компиляция под Windows / Build Tools + VCPKG:
  Конфигурация:
    cmake -S . -B build_vs_64 -G Ninja ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_TOOLCHAIN_FILE=R:/VCPKG/scripts/buildsystems/vcpkg.cmake ^
    -DVCPKG_TARGET_TRIPLET=x64-windows
  Компиляция:
    cmake --build build_vs_64 --config Release --parallel

Компиляция под Linux:
  gcc -O3 -s loto-keno.c font.c lang.c loto-keno.o -o loto-keno.exe \
  -lSDL3main -lSDL3 -lSDL3_ttf
*/


#include <SDL3/SDL_main.h>
#include "loto-keno.h"
#include "icon.c"


// SDL рендерер и окно программы:
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
TTF_Font *font = NULL;

bool screen_refresh = true; // Требуется ли перерисовка кадра на следующем тике?
bool screen_visible = true; // Виденно ли окно программы (т.е. не минимизировано)?

static float mouseX = 0.0f; // Позиция курсора на экране (ось X)
static float mouseY = 0.0f; // Позиция курсора на экране (ось Y)
bool isHoveringLeft;  // Курсор мыши наведён на Будь-будь-будь!
bool isHoveringRight; // Курсор мыши наведён на А-ОООО-ООО-Оо!

// Переменные для конфигурационного файла:
int language = 0;     // Язык игры: 0 = English, 1 = Deutsch, 2 = Русский
int fullscreen = 0;   // Полноэкранный режим
int color_scheme = 0; // Цветовая схема CGA
int window_x = SDL_WINDOWPOS_CENTERED; // Позиция окна по X
int window_y = SDL_WINDOWPOS_CENTERED; // Позиция окна по Y
int window_width = SCREENWIDTH;   // Ширина окна
int window_height = SCREENHEIGHT; // Высота окна

// Переменные игры:
int score = 10;    // Стартовый счёт
int maxScore = 10; // Максимальный счёт (рекорд)
int bet = 1;       // Сделанная ставка
int rounds = 0;    // Игровой раунд (round - переменная языка С)
int choice = 0;    // 0 = Ещё не сделана, 1 = Будь-будь-будь!, 2 = А-ОООО-ООО-Оо!

bool gameHna = false;     // Появился самурай? (ХНА!)
bool gameStarted = false; // Игра начата?
bool gameHelp = false;    // Отображается экран помощи?
int  gameOver = 0;        // 0 = игра в процессе, 1 = game over, 2 = game victory

// Цвета CGA для режима 320x200
SDL_Color cga_color_0;  // Чёрный
SDL_Color cga_color_1;  // Кияновый или зелёный
SDL_Color cga_color_2;  // Фиолетовый или красный
SDL_Color cga_color_3;  // Белый или жёлтый

// [JN] Значения координат для сетки моноширного шрифта.
// Один символ занимает 16х16 пикселей, поле CGA равно 40x25 символов.
//   0  16  32  48  64  80  96 112 128 144 160 176 192
//     208 224 240 256 272 288 304 320 336 352 368 384 


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
    if (!file) return;

    // [JN] Чтение параметров с проверкой успешности fscanf
    #define READ_OR_DEFAULT(var, def) if (fscanf(file, #var " %d\n", &var) != 1) var = def
    READ_OR_DEFAULT(language, 0);
    READ_OR_DEFAULT(fullscreen, 0);
    READ_OR_DEFAULT(color_scheme, 0);
    READ_OR_DEFAULT(window_x, SDL_WINDOWPOS_CENTERED);
    READ_OR_DEFAULT(window_y, SDL_WINDOWPOS_CENTERED);
    READ_OR_DEFAULT(window_width, SCREENWIDTH);
    READ_OR_DEFAULT(window_height, SCREENHEIGHT);
    #undef READ_OR_DEFAULT

    fclose(file);
}

static void SaveConfig (void)
{
    FILE *file = fopen(GetConfigPath(), "w");
    if (!file) return;

    #define WRITE(var) fprintf(file, "%-18s %d\n", #var, var)
    WRITE(language);
    WRITE(fullscreen);
    WRITE(color_scheme);
    WRITE(window_x);
    WRITE(window_y);
    WRITE(window_width);
    WRITE(window_height);
    #undef WRITE

    fclose(file);
}

// -----------------------------------------------------------------------------
// HandleMouseEvents
//  Обработка событий мыши.
// -----------------------------------------------------------------------------

static void HandleMouseEvents (SDL_Event *event)
{
    static int mousePressed = 0; // [PN] Отслеживание нажатия кнопки мыши

    // [PN] Обновить экран при движении мыши, нажатии кнопки или скролле
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN || event->type == SDL_EVENT_MOUSE_WHEEL)
    {
        screen_refresh = true;
    }
    else
    if (event->type == SDL_EVENT_MOUSE_MOTION && gameStarted && !gameOver)
    {
        // Обновляем координаты мыши
        float realX, realY;
        SDL_GetMouseState(&realX, &realY);
        SDL_RenderCoordinatesFromWindow(renderer, realX, realY, &mouseX, &mouseY);

        // [PN/JN] Определение наведения курсора на рамки выбора
        isHoveringLeft = (mouseX >= 16 && mouseX <= 320 && mouseY >= 176 && mouseY <= 256);
        isHoveringRight = (mouseX >= 336 && mouseX <= 624 && mouseY >= 176 && mouseY <= 256);

        // [PN] Для отслеживания изменений наведения — предыдущие состояния рамок
        static bool lastHoverLeft = false;
        static bool lastHoverRight = false;

        // [PN/JN] Обновление экрана происходит только при изменении наведения на рамки
        if (isHoveringLeft != lastHoverLeft || isHoveringRight != lastHoverRight)
        {
            screen_refresh = true;
            lastHoverLeft = isHoveringLeft;
            lastHoverRight = isHoveringRight;
        }
    }

    // [PN] Обработка колёсика мыши (ставка)
    if (event->type == SDL_EVENT_MOUSE_WHEEL && gameStarted && !gameOver)
    {
        if (event->wheel.y != 0) // Увеличиваем или уменьшаем ставку
        {
            const int delta =
                (event->wheel.y > 0.0f) ? 1 :
                (event->wheel.y < 0.0f) ? -1 : 0;

            bet = SDL_clamp(bet + delta, 1, score);
        }
    }

    // [PN] Обработка нажатия ЛКМ
    if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN && event->button.button == SDL_BUTTON_LEFT && !mousePressed)
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
    if (event->type == SDL_EVENT_MOUSE_BUTTON_UP && event->button.button == SDL_BUTTON_LEFT)
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
    if (event->type != SDL_EVENT_KEY_DOWN)
        return;

    SDL_Keycode key = event->key.key;
    const SDL_Keymod mod = event->key.mod;

    // [JN] Нажатие любой клавиши перерисовывает экран.
    screen_refresh = true;

    if (key == SDLK_F1)  // [JN] Открытие экрана помощи
    {
        gameHelp ^= 1;
        return;
    }

    if (key == SDLK_F2)  // [JN] Смена языка
    {
        if (++language > 2) language = 0;
        L_SetLanguageStrings();
        return;
    }

    if (key == SDLK_F3)  // [JN] Смена палитры
    {
        if (++color_scheme > 3) color_scheme = 0;
        R_InitColors();
        return;
    }

    if (key == SDLK_F4  // [PN/JN] Переключение полноэкранного режима
    || ((key == SDLK_RETURN || key == SDLK_KP_ENTER) && (mod & SDL_KMOD_ALT)))
    {
        fullscreen ^= 1;
        SDL_SetWindowFullscreen(window, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);

        // Перемещение курсора в центр экрана.
        int screen_width, screen_height;
        SDL_GetRenderOutputSize(renderer, &screen_width, &screen_height);
        SDL_WarpMouseInWindow(window,
                              (float)screen_width  * 0.5f,
                              (float)screen_height * 0.5f);
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

static void HandleWindowEvents (SDL_Event *event)
{
    switch (event->type)
    {
        case SDL_EVENT_WINDOW_MINIMIZED:
            screen_visible = 0;
            break;

        case SDL_EVENT_WINDOW_RESTORED:
        case SDL_EVENT_WINDOW_SHOWN:
            screen_visible = 1;
            break;

        case SDL_EVENT_WINDOW_MOVED:
            if (!fullscreen)
            {
                // [PN] Получение позиции окна при перемещении
                SDL_GetWindowPosition(window, &window_x, &window_y);
            }
            break;

        case SDL_EVENT_WINDOW_RESIZED:
        case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
            if (!fullscreen)
            {
                // [PN] Проверка, максимально ли окно в данный момент:
                SDL_WindowFlags flags = SDL_GetWindowFlags(window);
                if (flags & SDL_WINDOW_MAXIMIZED)
                {
                    // [PN] Если окно максимизировано, пропорцию не трогаем,
                    // просто сохраняем размеры как есть:
                    window_width = event->window.data1;
                    window_height = event->window.data2;
                }
                else
                {
                    const float aspect_ratio = (float)SCREENWIDTH / SCREENHEIGHT;
                    int new_width = event->window.data1;
                    int new_height = event->window.data2;
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
            screen_refresh = true;
            break;
        default:
            break;
    }
}

// -----------------------------------------------------------------------------
// D_KenoLoop
//  Главный цикл игры.
// -----------------------------------------------------------------------------

static void D_KenoLoop (void)
{
    SDL_Event event;
    srand((unsigned int)time(NULL));

    int running = 1;
    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
                running = 0;

            HandleMouseEvents(&event);
            HandleKeyboardEvents(&event);
            HandleWindowEvents(&event);
        }

        R_FinishUpdate();

        // [PN] Игра работает с фиксированной частотой 35 кадров в секунду — как в классическом Doom.
        // Задержка между кадрами вычисляется как 1000 мс / 35 ≈ 28.57 мс.
        // Мы используем SDL_Delay(28), чтобы поддерживать стабильный FPS и снизить нагрузку на процессор.
        SDL_Delay(28);
    }
}

// -----------------------------------------------------------------------------
// Основной цикл программы.
// -----------------------------------------------------------------------------

int main (int argc, char *argv[])
{
    // [PN] Параметры командной строки используются на Windows (например, -console),
    // поэтому они обязательно сохраняются. На других платформах могут быть не нужны,
    // но объявлены сознательно. Эти строки подавляют варнинги об их неиспользовании.
    (void)argc;
    (void)argv;

    int window_flags = 0;

    if (!SDL_Init(SDL_INIT_VIDEO) || !TTF_Init())
    {
        return 1;
    }

#ifdef _WIN32
    // [PN/JN] Отключаем автоматическое масштабирование окна Windows,
    // устанавливая осведомлённость процесса о DPI (DPI-aware).
    // Это позволяет избежать размытости изображения и корректно 
    // перекрывать экран при запуске в полноэкранном режиме.
    //
    // [PN] Для совместимости с Windows XP мы вызываем SetProcessDPIAware
    // через GetProcAddress. Эта функция доступна только начиная с Vista,
    // поэтому прямой вызов приведёт к падению на XP.
    {
        HMODULE user32 = LoadLibraryA("user32.dll");
        if (user32)
        {
            typedef BOOL (WINAPI *DPIAWARENESSFUNC)(void);
            DPIAWARENESSFUNC SetProcessDPIAware = 
                (DPIAWARENESSFUNC)(uintptr_t)GetProcAddress(user32, "SetProcessDPIAware");
            if (SetProcessDPIAware)
                SetProcessDPIAware();
            FreeLibrary(user32);
        }
    }

    // [JN/PN] Создание консольного окна для вывода, если
    // предоставлен параметр коммандной строки "-console".
    if (argc > 1 && strcmp(argv[1], "-console") == 0)
    {
        AllocConsole();
        SetConsoleTitle("Console");
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    }
#endif

    LoadConfig();

    window_flags = SDL_WINDOW_RESIZABLE;
    window_flags |= SDL_WINDOW_HIGH_PIXEL_DENSITY;
    if (fullscreen)
    {
        window_flags |= SDL_WINDOW_FULLSCREEN;
    }

    window = SDL_CreateWindow("", window_width, window_height, window_flags);
    if (!fullscreen)
    {
        SDL_SetWindowPosition(window, window_x, window_y);
    }

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

    renderer = SDL_CreateRenderer(window, NULL);
    SDL_SetRenderVSync(renderer, 1);

    SDL_SetRenderLogicalPresentation(renderer, SCREENWIDTH, SCREENHEIGHT,
                                     SDL_LOGICAL_PRESENTATION_LETTERBOX);

    // [PN] Загрузка шрифта, встроенного в код (font.c)
    SDL_IOStream *rw = SDL_IOFromMem(ibm_vga_data, ibm_vga_data_len);
    font = TTF_OpenFontIO(rw, 1, FONT_SIZE);

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
