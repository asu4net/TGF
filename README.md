# Tiny Game Framework (TGF)

TGF pretends to be a minimalistic cross-platform abstraction layer for common things in games.

> **Note:**
> This project is in a premature development stage, don't use it, unless you are open to extend this for your specific needs.

Features:
- Window creation.
- Keyboard and mouse input.
- OpenGL context creation.
- Basic math constructs.
- Memory arenas.

> **Note:**
> Almost all features are Windows only for now. 

Installation:
- Include `TGF.h` wherever you want.
- At least in one translation unit define `TGF_IMPL` and include `TGF.h`.
- Enjoy.

> **Note:**
> `build.bat` just builds `example.cpp`.