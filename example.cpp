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

  union vec4 bg_color_a = COLOR_CORN_FLOWER_BLUE;
  union vec4 bg_color_b = COLOR_CHILL_GREEN;

  while (!quit)
  {
    poll_events();

    if (is_key_down('V'))
    {
      clear_screen(bg_color_a);
    }
    else
    {
      clear_screen(bg_color_b);
    }

    Input_Event_View view = events_this_frame();
    for each_index(i, view.len)
    {
      Input_Event* ev = &view.data[i];

      if (ev->kind == INPUT_EVENT_QUIT)
      {
        quit = true;
        break;
      }
    }
    
    swap_buffers(window, /*vsync*/ true);
  }
  destroy_window(window);
}
