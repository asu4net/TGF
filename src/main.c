#define ENGINE_IMPL
#include "engine.h"

struct
{
  b8 quit;
  b8 vsync;
  union vec4 clear_color;
  struct window* window;
} 
game = nil;

union vec4 bg_color_a = COLOR_CORN_FLOWER_BLUE;
union vec4 bg_color_b = COLOR_CHILL_GREEN;

s32 main(s32 argc, char** argv) 
{
  UNUSED(argc);
  UNUSED(argv);

  // Initialization.
  game.clear_color = COLOR_CHILL_GREEN;
  game.vsync = true;
  game.window = create_window_default();
  create_gpu_context(game.window);

  // Main loop.
  while (!game.quit)
  {
    poll_events();

    struct os_event_view view = events_this_frame();

    // Process events.
    for each_index(i, view.len)
    {
      struct os_event* ev = &view.data[i];

      if (ev->kind == OS_EVENT_QUIT)
      {
        game.quit = true;
        break;
      }

      if (ev->kind == OS_EVENT_KEY && ev->key == KEY_ESCAPE && ev->key_state == KEY_STATE_PRESSED)
      {
        game.quit = true;
        break;
      }
    }

    if (is_key_down('V'))
    {
      game.clear_color = bg_color_a;
    }
    else
    {
      game.clear_color = bg_color_b;
    }

    // Draw frame.
    clear_screen(game.clear_color);

    // Present.
    swap_buffers(game.window, game.vsync);
  }
  destroy_window(game.window);
}