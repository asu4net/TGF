#ifndef ENGINE_H_GPU_API
#define ENGINE_H_GPU_API

// ---------------------------------------------------------------------------

void clear_screen(union vec4 color);

struct vertex_buffer_params
{
  struct // Vertices
  {
    const void* data;
    s32 size;
    s32 count;
  }
  vert_view;

  struct // Elements
  {
    const u32* data;
    s32 count;
  }
  elem_view;

  struct // Attributes
  {
    const enum data_type* data;
    s32 count;
  }
  attr_view;
};

#endif // ENGINE_H_GPU_API 


#ifdef ENGINE_IMPL_GPU_API

#ifdef ENGINE_API_OPENGL
#define ENGINE_IMPL_GPU_API_GL
#include "gpu_api_gl.h"
#endif

// ---------------------------------------------------------------------------

void clear_screen(union vec4 color)
{
#ifdef ENGINE_API_OPENGL
clear_screen_gl(color);
#else
#error "Not implemented!"
#endif
}

#endif // ENGINE_IMPL_GPU_API

