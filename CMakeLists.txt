cmake_minimum_required(VERSION 3.15)

project(LotoKeno C)

# Стандарт C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Оптимизации по умолчанию
if (MSVC)
    # MSVC: /O2 — максимальная оптимизация
    set(CMAKE_C_FLAGS_RELEASE "/MD /O2 /Ob2 /DNDEBUG /GL /Oy")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
else()
    # GCC / Clang: максимальные оптимизации + автоинлайн + луп-раскрутка
    set(CMAKE_C_FLAGS_RELEASE "-O3 -s -flto -fno-math-errno -fomit-frame-pointer -funroll-loops -DNDEBUG")
endif()

# Исходные файлы
set(SOURCES
    src/loto-keno.c
    src/font.c
    src/lang.c
    src/loto-keno.rc
)

# Заголовки (для IDE и порядка)
set(HEADERS
    src/font.h
    src/lang.h
)

# Исполняемый файл
add_executable(loto-keno ${SOURCES} ${HEADERS})

# Пути к заголовкам
target_include_directories(loto-keno PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# SDL2 и SDL2_ttf через vcpkg
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

# Линкуем библиотеки
target_link_libraries(loto-keno
    PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_ttf::SDL2_ttf
)

# Отключаем консольное окно в Windows
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
