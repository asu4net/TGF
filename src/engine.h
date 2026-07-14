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
#include "gpu_api.h"

#endif // ENGINE_H
  
#ifdef ENGINE_IMPL

#define ENGINE_IMPL_CORE
#define ENGINE_IMPL_MATH
#define ENGINE_IMPL_SYS
#define ENGINE_IMPL_ARENA
#define ENGINE_IMPL_KEY
#define ENGINE_IMPL_CURSOR
#define ENGINE_IMPL_OS_EVENT
#define ENGINE_IMPL_WINDOW
#define ENGINE_IMPL_GPU_CONTEXT
#define ENGINE_IMPL_GPU_API

#include "base_core.h"
#include "base_math.h"
#include "base_sys.h"
#include "base_arena.h"

#include "os_key.h"
#include "os_cursor.h"
#include "os_event.h"
#include "os_window.h"
#include "gpu_context.h"
#include "gpu_api.h"

#include "../.codegen_generated.c"

#endif // ENGINE_IMPL