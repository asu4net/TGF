# Tiny Engine

This project pretends to be a minimalistic abstraction layer for common things in games.

> **Note:**
> This project is in a premature development stage, don't use it, unless you are open to extend this for your specific needs.

Features:
- Single header implementation.
- Works on C99 and C++20 out of the box (tested using clang++).
- Arenas.
- Window creation.
- Keyboard and mouse input.
- OpenGL context creation.
- Basic math constructs.
- Basic metaprogramming.

> **Note:**
> Almost all features are Windows only for now. 

Usage:
- Include `engine.h` wherever you want.
- At least in one translation unit define `ENGINE_IMPL` and include `engine.h`.
- Call `engine_init` from the main function.
- If you don't have `.code_gen.h` and `.code_gen.c` generated yet. (is first build).
    - Build with `ENGINE_MODE_CODEGEN` defined. (flag `-codegen` in `build.bat`)
- Enjoy.

Linking:
- Windows/OpenGL:
    - gdi32    (Graphics Device Interface)
    - winmm    (Windows Multimedia)
    - opengl32 (OpenGL)

> **Note:**
> `build.bat` just builds `example.cpp`.