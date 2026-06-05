@echo off
setlocal enabledelayedexpansion

cls

set target=main.c
set out_exe=game.exe
set out_dbg=.out
set out_rel=.out\release

set includes=-Isrc
set links=-lopengl32 -lgdi32 -luser32 -lwinmm
set errors=-Werror -Wall -Wextra -Wconversion -Wsign-conversion

set build=clang -std=c99 %target% %includes% %links% %errors%
set build_dbg=%build% -o %out_dbg%\%out_exe% -g -O0 -DENGINE_DEBUG
set build_rel=%build% -o %out_rel%\%out_exe% -O2

set use_release=0
set use_codegen=0

:parse_args

if "%1"=="" goto args_done

if "%1"=="-r" (
    set use_release=1
)

if "%1"=="-codegen" (
    set use_codegen=1
)

shift
goto parse_args

:args_done

set start_time=%time%

if %use_release%==1 (
    if not exist %out_rel% mkdir %out_rel%
    set build_command=%build_rel%
) else (
    if not exist %out_dbg% mkdir %out_dbg%
    set build_command=%build_dbg%
)

if %use_codegen%==1 (

    echo Building %target% in codegen mode...

    %build% -o codegen.exe -O0 -DENGINE_DEBUG -DENGINE_MODE_CODEGEN

    if !errorlevel! neq 0 (
        echo Codegen build failed.
        exit /b 1
    )

    echo Running codegen...
    codegen.exe

    del codegen.exe
)

echo Building %target%...

%build_command%

if %errorlevel%==0 (
    echo Build succeeded.
    echo Init time: %start_time%
    echo Done time: %time%
) else (
    echo Build failed.
)
