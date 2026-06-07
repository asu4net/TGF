#ifndef ENGINE_H_OS_EVENT
#define ENGINE_H_OS_EVENT

enum os_event_kind 
{
    OS_EVENT_NONE
  , OS_EVENT_KEY
  , OS_EVENT_WINDOW
  , OS_EVENT_MOUSE_MOVE
  , OS_EVENT_MOUSE_WHEEL
  , OS_EVENT_QUIT
};

struct os_event
{
  enum os_event_kind kind;

  u32 key;
  u32 key_state;
  s32 wheel_delta;

  s32 window_x;
  s32 window_y;

  s32 mouse_x;
  s32 mouse_y;

  s32 mouse_delta_x;
  s32 mouse_delta_y;
};

struct os_event_view
{
  struct os_event* data;
  s32 len;
};

extern struct arena* g_os_events_arena;
extern s32 g_os_events_len;

inline struct os_event_view events_this_frame()
{
  struct os_event_view view;
  view.data = arena_first(g_os_events_arena, struct os_event);
  view.len  = g_os_events_len;
  return view;
}

inline void clear_events()
{
  check(g_os_events_arena != NULL);
  arena_clear(g_os_events_arena);
  g_os_events_len = 0;
}

void poll_events();

#endif // ENGINE_H_OS_EVENT 

#ifdef ENGINE_IMPL_OS_EVENT

struct arena* g_os_events_arena = NULL;
s32 g_os_events_len = 0;

#ifdef ENGINE_OS_WINDOWS
#define ENGINE_IMPL_OS_EVENT_WIN32
#include "os_event_win32.h"
#endif // ENGINE_OS_WINDOWS

void poll_events()
{
#ifdef ENGINE_OS_WINDOWS
  poll_events_win32();
#else
#error "Not implemented!"
#endif
}

#endif // ENGINE_IMPL_OS_EVENT
