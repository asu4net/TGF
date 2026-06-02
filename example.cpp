#ifdef DEBUG
  #define TGF_DEBUG
#endif

#define TGF_IMPL
#define TGF_OPENGL
#include "TGF.h"

B8 quit = false;

S32 
main(S32 argc, char** argv) 
{
  UnusedVar(argc);
  UnusedVar(argv);
  Log("This is Tiny Game Framework example!");
  Window* window = window_create_default();

  while(!quit)
  {
    gpu_clear(vec4(1.0f, 0.0f, 0.0f, 0.0f));
    input_poll_events();
    Input_Event_View view = input_events_this_frame();
    for EachIndex(i, view.len)
    {
      Input_Event* ev = &view.data[i];

      if (ev->kind == Input_Event_Kind_Quit)
      {
        quit = true;
        break;
      }
    }
    window_swap_buffers(window, /*vsync*/ true);
  }
  window_destroy(window);
}