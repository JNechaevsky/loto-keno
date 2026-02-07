cmake_minimum_required(VERSION 3.15)

project(LotoKeno C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Предупреждения компилятора
if(MSVC)
    add_compile_options(/W4 /WX /D_CRT_SECURE_NO_WARNINGS)
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
    set(CMAKE_C_FLAGS_RELEASE "/O2 /Ob2 /DNDEBUG /GL /Oy")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /LTCG")
else()
    set(CMAKE_C_FLAGS_RELEASE "-O3 -flto -fno-math-errno -fomit-frame-pointer -funroll-loops -DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s")
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

# Заголовки (для IDE и порядка)
set(HEADERS
    src/loto-keno.h
)

# Исполняемый файл
add_executable(loto-keno ${SOURCES} ${HEADERS})

# Пути к заголовкам
target_include_directories(loto-keno PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Универсальный поиск SDL3 и SDL3_ttf
find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)

target_link_libraries(loto-keno PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)

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
