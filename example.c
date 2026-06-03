#ifdef DEBUG
  #define TGF_DEBUG
#endif

#define TGF_IMPL
#define TGF_OPENGL
#include "TGF.h"

b8 quit = false;

s32 main(s32 argc, char** argv) 
{
  UNUSED(argc);
  UNUSED(argv);
  trace("This is Tiny Game Framework example!");
  struct window* window = create_window_default();

  while(!quit)
  {
    clear_screen(COLOR_RED);
    poll_events();
    Input_Event_View view = events_this_frame();
    for each_index(i, view.len)
    {
      Input_Event* ev = &view.data[i];

      if (ev->kind == Input_Event_Kind_Quit)
      {
        quit = true;
        break;
      }
    }
    swap_buffers(window, /*vsync*/ true);
  }
  destroy_window(window);
}