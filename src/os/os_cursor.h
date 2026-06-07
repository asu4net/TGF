#ifndef ENGINE_H_CURSOR
#define ENGINE_H_CURSOR

enum cursor_mode
{
  CURSOR_MODE_DEFAULT,
  CURSOR_MODE_CONFINED,
  CURSOR_MODE_HIDDEN
};

void update_cursor();
void set_cursor_mode(enum cursor_mode mode);

#endif // ENGINE_H_CURSOR 

#ifdef ENGINE_IMPL_CURSOR

#ifdef ENGINE_OS_WINDOWS
#define ENGINE_IMPL_CURSOR_WIN32
#include "os_cursor_win32.h"
#endif

void update_cursor()
{
#ifdef ENGINE_OS_WINDOWS
  update_cursor_win32();
#else
#error "Not implemented!"
#endif
}

void set_cursor_mode(enum cursor_mode mode)
{
#ifdef ENGINE_OS_WINDOWS
  set_cursor_mode_win32(mode);
#else
#error "Not implemented!"
#endif
}

#endif // ENGINE_IMPL_CURSOR
