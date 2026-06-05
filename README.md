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

> **Note:**
> Almost all features are Windows only for now. 

Installation:
- Include `engine.h` wherever you want.
- At least in one translation unit define `ENGINE_IMPLEMENTATION` and include `engine.h`.
- Enjoy.

Linking:
- Windows/OpenGL:
    - gdi32    (Graphics Device Interface)
    - winmm    (Windows Multimedia)
    - opengl32 (OpenGL)

> **Note:**
> `build.bat` just builds `example.cpp`.