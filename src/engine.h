#ifndef ENGINE_H
#define ENGINE_H

#include "base_core.h"
#include "base_math.h"
#include "base_sys.h"
#include "base_arena.h"
#include "../.codegen_generated.h"

#include "os_key.h"
#include "os_cursor.h"
#include "os_event.h"
#include "os_window.h"
#include "gpu_context.h"
#include "engine_state.h"

struct vertex_buffer
{
  struct hmap_handle handle;
  u32 id;
};

void clear_screen(union vec4 color);

#endif // ENGINE_H
  
#ifdef ENGINE_IMPLEMENTATION

#define ENGINE_IMPLEMENTATION_BASE
#define ENGINE_IMPLEMENTATION_MATH
#define ENGINE_IMPLEMENTATION_SYS
#define ENGINE_IMPLEMENTATION_ARENA
#define ENGINE_IMPLEMENTATION_STATE
#define ENGINE_IMPLEMENTATION_KEY
#define ENGINE_IMPLEMENTATION_CURSOR
#define ENGINE_IMPLEMENTATION_EVENT
#define ENGINE_IMPLEMENTATION_WNDCREATION
#define ENGINE_IMPLEMENTATION_GPU_CONTEXT

#include "base_core.h"
#include "base_math.h"
#include "base_sys.h"
#include "base_arena.h"

#include "../.codegen_generated.c"

#include "os_key.h"
#include "os_cursor.h"
#include "os_event.h"
#include "os_window.h"
#include "gpu_context.h"
#include "engine_state.h"

// ============================================
// @i: Graphics (OpenGL).
// ============================================

#ifdef ENGINE_API_OPENGL

void clear_screen(union vec4 color)
{
  glClearColor(color.x, color.y, color.z, color.w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

#endif // ENGINE_API_OPENGL

#endif // ENGINE_IMPLEMENTATION