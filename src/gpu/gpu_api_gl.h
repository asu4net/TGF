#ifndef ENGINE_H_GPU_API_GL
#define ENGINE_H_GPU_API_GL

struct gl_vertex_buffer
{
  struct hmap_handle handle;
  u32 id;
};

void clear_screen_gl(union vec4 color);

#endif // ENGINE_H_GPU_API_GL

#ifdef ENGINE_IMPL_GPU_API_GL

void clear_screen_gl(union vec4 color)
{
  glClearColor(color.x, color.y, color.z, color.w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

#endif // ENGINE_IMPL_GPU_API_GL

