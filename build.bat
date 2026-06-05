@echo off

set target=example.cpp
set out_file=game.exe
set out_dbg=.out\
set out_rel=.out\release
set includes=-Isrc
set links=-lopengl32 -lgdi32 -luser32 -lwinmm
set errors=-Werror -Wall -Wextra -Wconversion -Wsign-conversion

set build=clang++ -std=c++20 %target% %includes% %links% %errors%
set build_dbg=%build% -o %out_dbg%\%out_file% -g -O0 -DENGINE_DEBUG
set build_rel=%build% -o %out_rel%\%out_file% -O2

cls
rmdir .out /s /q
set start_time=%time%

echo Building %target%.

if "%1"=="-r" (
    if not exist %out_rel% mkdir %out_rel%
    set build_command=%build_rel%
) else (
    if not exist %out_dbg% mkdir %out_dbg%
    set build_command=%build_dbg%
)

%build_command%

if %errorlevel%==0 (
    echo Build succeeded.
    echo Init time: %start_time%.
    echo Done time: %time%.
) else (
    echo Build failed.
)

