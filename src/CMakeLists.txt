cmake_minimum_required(VERSION 3.15)

project(LotoKeno C)

# Устанавливаем стандарт C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Оптимизация по умолчанию
if (MSVC)
    add_compile_options(/O2)
else()
    add_compile_options(-O3 -s)
endif()

# Создаём исполняемый файл из исходника
add_executable(loto-keno
    loto-keno.c
    loto-keno.rc
    font.c
    font.h
    lang.c
    lang.h
)

# Ищем SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Подключаем include-директории
target_include_directories(loto-keno PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_TTF_INCLUDE_DIRS}
)

# Линкуем библиотеки
target_link_libraries(loto-keno SDL2 SDL2_ttf)

# Это включает GUI-подсистему, и консольное окно не появляется
if (MSVC)
    set_target_properties(loto-keno PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
else()
    set_target_properties(loto-keno PROPERTIES LINK_FLAGS "-mwindows")
endif()

