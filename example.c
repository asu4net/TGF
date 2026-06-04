#ifdef DEBUG
#define TGF_DEBUG
#endif

#define TGF_IMPL
#define TGF_OPENGL
#include "TGF.h"

union vec4 bg_color_a = COLOR_CORN_FLOWER_BLUE;
union vec4 bg_color_b = COLOR_CHILL_GREEN;

static void tick(f32 dt)
{
  UNUSED(dt);

  if (is_key_down('V'))
  {
    g_engine.clear_color = bg_color_a;
  }
  else
  {
    g_engine.clear_color = bg_color_b;
  }
}

s32 main(s32 argc, char** argv) 
{
  struct engine_params p = DEFAULT_ENGINE_PARAMS;
  p.argv = argv;
  p.argc = argc;
  p.callbacks.tick = &tick;
  engine_run(&p);
}