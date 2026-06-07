#ifndef ENGINE_DECLARATION_WNDCREATION
#define ENGINE_DECLARATION_WNDCREATION

#define MAX_WINDOWS 30

struct window 
{
  void* handle; // Handle to the platform-specific window. (Maybe we should change this later)
  u32 index;    // The index occupied in the window array.
};

struct window_params
{
  s32 width;
  s32 height;
  const char* title;
  union vec4 bg_color;
};

#define DEFAULT_WINDOW_PARAMS make_struct(window_params, \
  .width    = 1920,                                      \
  .height   = 1080,                                      \
  .title    = "Application",                             \
  .bg_color = COLOR_CORN_FLOWER_BLUE,                    \
);                                                       \

struct window* create_window(struct window_params* params);

inline struct window* create_window_default()
{
  struct window_params params = DEFAULT_WINDOW_PARAMS;
  return create_window(&params);
}

void destroy_window(struct window* window);
void swap_buffers(struct window* window, b8 vsync);

#endif // ENGINE_DECLARATION_WNDCREATION 

#ifdef ENGINE_IMPLEMENTATION_WNDCREATION

#ifdef ENGINE_OS_WINDOWS
#define ENGINE_IMPLEMENTATION_WNDCREATION_WIN32
#include "os_window_win32.h"
#endif

struct window* create_window(struct window_params* params)
{
#ifdef ENGINE_OS_WINDOWS
  return create_window_win32(params);
#else
#error "Not implemented!"
#endif
}

void destroy_window(struct window* window)
{
#ifdef ENGINE_OS_WINDOWS
  destroy_window_win32(window);
#else
#error "Not implemented!"
#endif
}

#endif // ENGINE_IMPLEMENTATION_WNDCREATION
