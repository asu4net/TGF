#ifndef ENGINE_DECLARATION_EVENT
#define ENGINE_DECLARATION_EVENT

enum input_event_kind 
{
    INPUT_EVENT_NONE
  , INPUT_EVENT_KEY
  , INPUT_EVENT_WINDOW
  , INPUT_EVENT_MOUSE_MOVE
  , INPUT_EVENT_MOUSE_WHEEL
  , INPUT_EVENT_QUIT
};

struct input_event
{
  enum input_event_kind kind;

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

struct input_event_view
{
  struct input_event* data;
  s32 len;
};

extern struct arena* g_input_events_arena;
extern s32 g_input_events_len;

inline struct input_event_view events_this_frame()
{
  struct input_event_view view;
  view.data = arena_first(g_input_events_arena, struct input_event);
  view.len  = g_input_events_len;
  return view;
}

inline void clear_events()
{
  check(g_input_events_arena != NULL);
  arena_clear(g_input_events_arena);
  g_input_events_len = 0;
}

void poll_events();

#endif // ENGINE_DECLARATION_EVENT 

#ifdef ENGINE_IMPLEMENTATION_EVENT

struct arena* g_input_events_arena = NULL;
s32 g_input_events_len = 0;

#ifdef ENGINE_OS_WINDOWS
#define ENGINE_IMPLEMENTATION_EVENT_WIN32
#include "input/event_win32.h"
#endif // ENGINE_OS_WINDOWS

void poll_events()
{
#ifdef ENGINE_OS_WINDOWS
  poll_events_win32();
#else
#error "Not implemented!"
#endif
}

#endif // ENGINE_IMPLEMENTATION_EVENT
