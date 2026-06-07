#ifndef ENGINE_H_GPU_API
#define ENGINE_H_GPU_API

struct vertex_buffer
{
  struct hmap_handle handle;
  u32 id;
};

void clear_screen(union vec4 color);

#endif // ENGINE_H_GPU_API 

#ifdef ENGINE_IMPL_GPU_API

#ifdef ENGINE_API_OPENGL
#define ENGINE_IMPL_GPU_API_GL
#include "gpu_api_gl.h"
#endif

void clear_screen(union vec4 color)
{
#ifdef ENGINE_API_OPENGL
clear_screen_gl(color);
#else
#error "Not implemented!"
#endif
}

#endif // ENGINE_IMPL_GPU_API

