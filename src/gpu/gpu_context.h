#ifndef ENGINE_DECLARATION_GPU_CONTEXT
#define ENGINE_DECLARATION_GPU_CONTEXT

#define ENGINE_API_OPENGL // @Note: For now we force the OpenGL implementation.
 
b8 create_gpu_context(struct window* window);
void swap_buffers(struct window* window, b8 vsync);

#endif // ENGINE_DECLARATION_GPU_CONTEXT 

#ifdef ENGINE_IMPLEMENTATION_GPU_CONTEXT

#if defined(ENGINE_OS_WINDOWS) && defined(ENGINE_API_OPENGL)
#define ENGINE_IMPLEMENTATION_GPU_CONTEXT_WGL
#include "gpu_context_wgl.h"
#endif

#ifdef ENGINE_API_OPENGL

void swap_buffers(struct window* window, b8 vsync)
{
#if defined(ENGINE_OS_WINDOWS) && defined(ENGINE_API_OPENGL)
  swap_buffers_wgl(window, vsync);
#else
#error "Not implemented!"
#endif
}

b8 create_gpu_context(struct window* window)
{
#if defined(ENGINE_OS_WINDOWS) && defined(ENGINE_API_OPENGL)
  return create_context_wgl(window);
#else
#error "Not implemented!"
#endif
}

#endif

#endif // ENGINE_IMPLEMENTATION_GPU_CONTEXT
