#ifndef ENGINE_DECLARATION_CURSOR_WIN32
#define ENGINE_DECLARATION_CURSOR_WIN32

void update_cursor_win32();
void set_cursor_mode_win32(enum cursor_mode mode);

#endif // ENGINE_DECLARATION_CURSOR_WIN32 

#ifdef ENGINE_IMPLEMENTATION_CURSOR_WIN32

static enum cursor_mode g_cursor_mode_win32 = CURSOR_MODE_DEFAULT;

void update_cursor_win32() 
{
  if (g_cursor_mode_win32 == CURSOR_MODE_DEFAULT)
  return;

  HWND fg = GetForegroundWindow();
  
  RECT rect;
  GetClientRect(fg, &rect);
  
  POINT ul = { rect.left,  rect.top };
  POINT lr = { rect.right, rect.bottom };

  ClientToScreen(fg, &ul);
  ClientToScreen(fg, &lr);

  rect.left = ul.x;
  rect.top = ul.y;
  rect.right = lr.x;
  rect.bottom = lr.y;
  
  ClipCursor(&rect);
  
  if (g_cursor_mode_win32 == CURSOR_MODE_HIDDEN)
  {
    // @Note: We center the cursor.
    SetCursorPos
    (
        (rect.left + rect.right)  / 2,
        (rect.top  + rect.bottom) / 2
    );
  }
}

void set_cursor_mode_win32(enum cursor_mode mode)
{
  if (mode == g_cursor_mode_win32)
  {
    return;
  }

  if (mode == CURSOR_MODE_DEFAULT)
  {
    ClipCursor(NULL);

    // @Robustness: This won't work on multiple windows in different threads.
    while (ShowCursor(TRUE) < 0) {}
  }
  else
  {
    // @Robustness: This won't work on multiple windows in different threads.
    while (ShowCursor(FALSE) >= 0) {}
  }
  g_cursor_mode_win32 = mode;
}

#endif // ENGINE_IMPLEMENTATION_CURSOR_WIN32 
