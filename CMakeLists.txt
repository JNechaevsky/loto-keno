cmake_minimum_required(VERSION 3.15)

project(LotoKeno C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Предупреждения компилятора
if(MSVC)
    # TODO? add_compile_options(/W4 /WX)
else()
    add_compile_options("-Wall"
                        "-Wredundant-decls"
                        "-Wformat-security"
                        "-Wno-ignored-qualifiers"
                        "-Wpedantic"
                        "-Wextra"
                        )
endif()

if (MSVC)
    set(CMAKE_C_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG /GL /Oy")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
else()
    set(CMAKE_C_FLAGS_RELEASE "-O3 -s -flto -fno-math-errno -fomit-frame-pointer -funroll-loops -DNDEBUG")
endif()

# Исходные файлы
set(SOURCES
    src/loto-keno.c
    src/draw.c
    src/font.c
    src/game.c
    src/lang.c
    data/loto-keno.rc
)

# Файл с иконкой (не используется в Windows)
if (NOT WIN32)
    list(APPEND SOURCE_FILES src/icon.c)
endif()

# Заголовки (для IDE и порядка)
set(HEADERS
    src/loto-keno.h
)

# Исполняемый файл
add_executable(loto-keno ${SOURCES} ${HEADERS})

# Пути к заголовкам
target_include_directories(loto-keno PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Универсальный поиск SDL2 и SDL2_ttf
find_package(SDL2 REQUIRED)

# Явно ищем SDL2_ttf, это нужно для Linux и MinGW/MSYS
find_path(SDL2_TTF_INCLUDE_DIR SDL_ttf.h PATH_SUFFIXES SDL2)
find_library(SDL2_TTF_LIBRARY NAMES SDL2_ttf)

if (NOT SDL2_TTF_INCLUDE_DIR OR NOT SDL2_TTF_LIBRARY)
    message(FATAL_ERROR "SDL2_ttf library not found. Make sure it's installed and available in PATH.")
endif()

target_include_directories(loto-keno PRIVATE ${SDL2_INCLUDE_DIRS} ${SDL2_TTF_INCLUDE_DIR})
target_link_libraries(loto-keno PRIVATE ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARY})

# Для Windows отключаем консольное окно
if(WIN32)
    if (MSVC)
        set_target_properties(loto-keno PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS"
            WIN32_EXECUTABLE YES
        )
    else()
        set_target_properties(loto-keno PROPERTIES
            LINK_FLAGS "-mwindows"
        )
    endif()
endif()
