// ============================================
// @: Info.
// ============================================
//
// Usage:
//
// 1. Just define TGF_IMPL before including in one *.c or *.cpp file.
// 2. Link the platform specific libs:
//
// Link:
//
// Windows -------------------------
// gdi32    (Graphics Device Interface)
// winmm    (Windows Multimedia)
// opengl32 (OpenGL) (optional)
// ---------------------------------
//
// Compile Flags:
//
// TGF_IMPL   Pastes the function implementations.
// TGF_DEBUG  Enables trace, check, ensure macros.
// TGF_OPENGL Uses OpenGL.

#ifndef TINY_GAME_FRAMEWORK_H
#define TINY_GAME_FRAMEWORK_H

// ============================================
// @: Platform detection.
// ============================================

#ifdef _WIN32
#ifdef _WIN64
#define TGF_WINDOWS
#else
#error "x86 Builds are not supported!"
#endif
#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
// TARGET_OS_MAC exists on all the platforms
// so we must check all of them (in this order)
// to ensure that we're running on MAC
// and not some other Apple platform.
#if TARGET_IPHONE_SIMULATOR == 1
#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE == 1
#define TGF_IOS
#error "IOS is not supported!"
#elif TARGET_OS_MAC == 1
#define TGF_MACOS
#error "MacOS is not supported!"
#else
#error "Unknown Apple platform!"
#endif
// We also have to check __ANDROID__ before __linux__
// since android is based on the linux kernel
// it has __linux__ defined */
#elif defined(__ANDROID__)
#define TGF_ANDROID
#error "Android is not supported!"
#elif defined(__linux__)
#define TGF_LINUX 
#error "Linux is not supported!"
#else
#error "Unknown platform!"
#endif

#if defined(__cplusplus)
#define TGF_CPP
#else
#define TGF_C
#endif

// ============================================
// @: Vendor headers.
// ============================================

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef TGF_WINDOWS 
#define UNICODE
#define _UNICODE
#include <Windows.h>
#endif

#ifdef TGF_OPENGL
#include "GL/GL.h"
#endif

// ============================================
// @: Numeric Types.
// ============================================

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef float f32;
typedef double f64;
typedef s8 b8;
typedef u32 u32_enum;

// ============================================
// @: Helper Macros.
// ============================================

// Heavily used macros ---------------------------------------- 

// Custom true/false literals.
#ifdef TGF_C
#undef true
#undef false
#define true ((b8) 1)
#define false ((b8) 0)
#endif

// Logging and assertions macros.

// Logging/Assertion Helper functions -------
#ifdef TGF_DEBUG

void trace_function  (const char* fmt, ...);
void check_function  (b8 expr, const char* fmt, ...);
b8   ensure_function (b8 expr, const char* fmt, ...);

#endif
//-------------------------------------------

#ifdef TGF_DEBUG
#define trace(X, ...) (trace_function(X, ##__VA_ARGS__))
#else
#define trace(X, ...)
#endif

#ifdef TGF_DEBUG
#define check(X) check_function((X), "Check failed: %s\n", #X)
#define check_msg(X, ...) check_function((X), __VA_ARGS__)
#define ensure(X) ensure_function((X), "Ensure failed: %s\n", #X)
#define ensure_msg(X, ...) ensure_function((X), __VA_ARGS__)
#else
#define check(X) ((void)0)
#define check_msg(...) ((void)0)
#define ensure(X) (X)
#define ensure_msg(X, ...) (X)
#endif

// Loop helpers.
#define each_index(it, count) (s32 it = 0; it < (count); it += 1)

// Cross-language zero struct literal.
#if defined(TGF_CPP)
#define nil {}
#elif defined(TGF_C)
#define nil {0}
#endif

// Cross-language struct intializer/designated initializer list helper.
#if defined(TGF_CPP)
#define make_struct(T, ...) { __VA_ARGS__ }
#define make_union(T, ...) { __VA_ARGS__ }
#elif defined(TGF_C)
#define make_struct(T, ...) (struct T){ __VA_ARGS__ }
#define make_union(T, ...) (union T){ __VA_ARGS__ }
#endif

// ----------------------------------------------------------

// Static assert.
#ifdef TGF_DEBUG
#define STATIC_CHECK(C, ID) static u8 GLUE(ID, __LINE__)[(C)?1:-1] // @Review: Maybe we shouldn't get rid of this one, ever.
#else
#define STATIC_CHECK(C, ID)
#endif

// Units.
#define KB(n) (((u64)(n)) << 10)
#define MB(n) (((u64)(n)) << 20)
#define GB(n) (((u64)(n)) << 30)
#define TB(n) (((u64)(n)) << 40)
#define THOUSAND(n) ((n)*1000)
#define MILLION(n) ((n)*1000000)
#define BILLION(n) ((n)*1000000000)

// ALIGN_OF, MIN, MAX, CLAMP.
#define ALIGN_OF(T) __alignof(T) // @Pending(Platform): This will work just in clang and msvc.
#define MIN(A,B) (((A)<(B))?(A):(B))
#define MAX(A,B) (((A)>(B))?(A):(B))
#define CLAMP(A,X,B) (((X)<(A))?(A):((X)>(B))?(B):(X))

// Macro expansion helpers.
#define STRINGIFY_NO_EXPAND(S) #S
#define STRINGIFY(S) STRINGIFY_NO_EXPAND(S)
#define GLUE_NO_EXPAND(A,B) A##B
#define GLUE(A,B) GLUE_NO_EXPAND(A,B)

// Misc.
#define UNUSED(v) ((void)v)
#define DEFER_BLOCK(begin, end) for(s32 _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#define ALIGN_POW2(x,b) (((x) + (b) - 1)&(~((b) - 1))) // @Note: b has to be pow2. 

// Debug break.
#ifdef TGF_DEBUG
#ifdef TGF_WINDOWS
#define STOP_AT_THIS_LINE() __debugbreak()
#else
#define STOP_AT_THIS_LINE()
#endif
#else
#define STOP_AT_THIS_LINE()
#endif

// ============================================
// @: OS.
// ============================================

// @Pending(Platform): All the OS code it is just windows for now.

struct system_info 
{
  u64 page_size;
  u64 large_page_size;
  u64 allocation_granularity;
};

extern struct system_info g_system_info;
struct system_info* get_system_info();

void* reserve_large (u64 size);
b8    commit_large  (void* ptr, u64 size);
void* reserve       (u64 size);
b8    commit        (void* ptr, u64 size);
void  decommit      (void* ptr, u64 size);
void  release       (void* ptr, u64 size);

// ============================================
// @: Arena.
// ============================================

#define ARENA_HEADER_SIZE 128
#define ARENA_FREE_LIST 1
#define ARENA_FLAG_NO_CHAIN (1<<0)
#define ARENA_FLAG_LARGE_PAGES (1<<1)

struct arena_params 
{
  u32_enum flags;
  u64 reserve_size;
  u64 commit_size;
  void* optional_backing_buffer;
  const char* allocation_site_file;
  s32 allocation_site_line;
};

#define DEFAULT_ARENA_PARAMS make_struct(arena_params, \
  .flags                   = 0,                        \
  .reserve_size            = MB(64),                   \
  .commit_size             = KB(64),                   \
  .optional_backing_buffer = NULL,                     \
  .allocation_site_file    = __FILE__,                 \
  .allocation_site_line    = __LINE__                  \
)                                                      \

struct arena 
{
  struct arena* prev;    // Previous arena in chain.
  struct arena* current; // Current arena in chain.
  u32_enum flags;
  u64 cmt_size;
  u64 res_size;
  u64 base_pos;
  u64 pos;
  u64 cmt;
  u64 res;
  const char* allocation_site_file;
  s32 allocation_site_line;

#if ARENA_FREE_LIST
    struct arena* free_last;
#endif
};

STATIC_CHECK(sizeof(struct arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

struct temp
{
  struct arena* arena;
  u64 pos;
};

struct arena*   arena_alloc_default   ();
struct arena*   arena_alloc           (struct arena_params* params);
void            arena_release         (struct arena* arena);
void*           arena_push            (struct arena* arena, u64 size, u64 align, b8 zero);
u64             arena_pos             (struct arena* arena);
void            arena_pop_to          (struct arena* arena, u64 pos);
void*           arena_first_raw       (struct arena* arena, u64 align);
void            arena_clear           (struct arena* arena);
void            arena_pop             (struct arena* arena, u64 amt);
struct temp     temp_begin            (struct arena* arena);
void            temp_end              (struct temp temp);

// @Note: Push helper macros.
#define arena_push_array_no_zero_aligned(a, T, c, align) (T*) arena_push((a), sizeof(T)*(c), (align), (0))    
#define arena_push_array_aligned(a, T, c, align)  (T*) arena_push((a), sizeof(T)*(c), (align), (1))
#define arena_push_array_no_zero(a, T, c) arena_push_array_no_zero_aligned(a, T, c, MAX(8, ALIGN_OF(T)))
#define arena_push_array(a, T, c) arena_push_array_aligned(a, T, c, MAX(8, ALIGN_OF(T)))
#define arena_push_element(a, T) (T*) arena_push((a), sizeof(T), ALIGN_OF(T), (1))
#define arena_first(a, T) (T*) arena_first_raw((a), ALIGN_OF(T))

// ============================================
// @: Vec2.
// ============================================

union vec2
{
  struct
  {
    f32 x;
    f32 y;
  };

  f32 v[2];
};

// ============================================
// @: Vec3.
// ============================================

union vec3
{
  struct
  {
    f32 x;
    f32 y;
    f32 z;
  };

  struct
  {
    union vec2 xy;
    f32 _z;
  };

  struct
  {
    f32 _x;
    union vec2 yz;
  };
    
  f32 v[3];
};

// ============================================
// @: Vec4.
// ============================================

union vec4
{
  struct
  {
    f32 x;
    f32 y;
    f32 z;
    f32 w;
  };
    
  struct
  {
    union vec2 xy;
    union vec2 zw;
  };
    
  struct
  {
    union vec3 xyz;
    f32 _w;
  };
    
  struct
  {
    f32 _x;
    union vec3 yzw;
  };

  f32 v[4];
};

#define vec4_lit(_x, _y, _z, _w) make_union(vec4, .x = _x, .y = _y, .z = _z, .w = _w)

// ============================================
// @: Mat4.
// ============================================

typedef struct Mat4
{
  // @Note: Row major matrix. First number is the row, Second is the column. (D3D11)
  f32 _11, _12, _13, _14;
  f32 _21, _22, _23, _24;
  f32 _31, _32, _33, _34;
  f32 _41, _42, _43, _44;
} Mat4;

// ============================================
// @: Colors. (Vec4)
// ============================================

#define COLOR_WHITE            vec4_lit(1.000f, 1.000f, 1.000f, 1.000f)
#define COLOR_WHITE_FADED      vec4_lit(1.000f, 1.000f, 1.000f, 0.000f)
#define COLOR_BLACK            vec4_lit(0.000f, 0.000f, 0.000f, 1.000f)
#define COLOR_CORN_FLOWER_BLUE vec4_lit(0.388f, 0.584f, 0.933f, 1.000f)
#define COLOR_BLUE             vec4_lit(0.000f, 0.000f, 1.000f, 1.000f)
#define COLOR_LIGHT_BLUE       vec4_lit(0.300f, 0.300f, 1.000f, 1.000f)
#define COLOR_CYAN             vec4_lit(0.000f, 1.000f, 1.000f, 1.000f)
#define COLOR_GRAY             vec4_lit(0.500f, 0.500f, 0.500f, 1.000f)
#define COLOR_DARK_GRAY        vec4_lit(0.200f, 0.200f, 0.200f, 1.000f)
#define COLOR_GREEN            vec4_lit(0.000f, 1.000f, 0.000f, 1.000f)
#define COLOR_LIGHT_GREEN      vec4_lit(0.300f, 1.000f, 0.300f, 1.000f)
#define COLOR_CHILL_GREEN      vec4_lit(0.047f, 0.651f, 0.408f, 1.000f)
#define COLOR_MAGENTA          vec4_lit(1.000f, 0.000f, 1.000f, 1.000f)
#define COLOR_RED              vec4_lit(1.000f, 0.000f, 0.000f, 1.000f)
#define COLOR_LIGHT_RED        vec4_lit(1.000f, 0.300f, 0.300f, 1.000f)
#define COLOR_YELLOW           vec4_lit(1.000f, 0.920f, 0.016f, 1.000f)
#define COLOR_ORANGE           vec4_lit(0.970f, 0.600f, 0.110f, 1.000f)


// ============================================
// @: Window.
// ============================================

#define MAX_WINDOWS 30

struct window 
{
  void* handle; // Handle to the platform-specific window. (Maybe we should change this later)
  u32 index;    // The index occupied in the window array.
};

struct window_params
{
  s32 width;
  s32 height;
  const char* title;
  union vec4 bg_color;
};

#define DEFAULT_WINDOW_PARAMS make_struct(window_params, \
  .width    = 1920,                                      \
  .height   = 1080,                                      \
  .title    = "Application",                             \
  .bg_color = COLOR_CORN_FLOWER_BLUE,                    \
);                                                       \

struct window* create_window_default();
struct window* create_window(struct window_params* params);
void destroy_window(struct window* window);

#ifdef TGF_OPENGL
void swap_buffers(struct window* window, b8 vsync);
#endif

// ============================================
// @: Input.
// ============================================

enum cursor_mode
{
  CURSOR_MODE_DEFAULT,
  CURSOR_MODE_CONFINED,
  CURSOR_MODE_HIDDEN
};

void set_cursor_mode(enum cursor_mode mode);

enum key_state
{
    KEY_STATE_NONE     = 1 << 0 
  , KEY_STATE_DOWN     = 1 << 1
  , KEY_STATE_START    = 1 << 2 
  , KEY_STATE_END      = 1 << 3
  , KEY_STATE_PRESSED  = KEY_STATE_START | KEY_STATE_DOWN
  , KEY_STATE_REPEAT   = KEY_STATE_DOWN
  , KEY_STATE_RELEASED = KEY_STATE_END
};

enum key_code
{
    KEY_UNKNOWN = 0
    
  // *** ASCII (REAL CODES) ***

  , KEY_BACKSPACE    = 8
  , KEY_TAB          = 9
  , KEY_LINEFEED     = 10
  , KEY_ENTER        = 13
  , KEY_ESCAPE       = 27
  , KEY_SPACE        = 32

  // @Note: Here live ASCII letters, digits and symbols via direct char input.
  // They are not enumerated because they are received as char or UTF-32 text.

  // *** END OF ASCII ***
  // Begin custom keycodes (physical keys)

  , KEY_BEGIN_NON_ASCII = 512 // @Note: Explicit boundary to avoid collisions.

  , KEY_DELETE       
  , KEY_ARROW_UP     
  , KEY_ARROW_DOWN   
  , KEY_ARROW_LEFT   
  , KEY_ARROW_RIGHT  
  , KEY_PAGE_UP      
  , KEY_PAGE_DOWN    
  , KEY_HOME         
  , KEY_END          
  , KEY_INSERT       
  , KEY_PAUSE        
  , KEY_SCROLL_LOCK  
  , KEY_ALT          
  , KEY_CTRL         
  , KEY_SHIFT        
  , KEY_CMD          

  // *** FUNCTION KEYS ***
  , KEY_F1  , KEY_F2  , KEY_F3  , KEY_F4  , KEY_F5  , KEY_F6  , KEY_F7  , KEY_F8  , KEY_F9 , KEY_F10
  , KEY_F11 , KEY_F12 , KEY_F13 , KEY_F14 , KEY_F15 , KEY_F16 , KEY_F17 , KEY_F18 , KEY_F19
  , KEY_F20 , KEY_F21 , KEY_F22 , KEY_F23 , KEY_F24

  , KEY_PRINT_SCREEN

  // *** MOUSE AS BUTTON INPUT KEYS ***
  , KEY_MOUSE_LEFT
  , KEY_MOUSE_MIDDLE
  , KEY_MOUSE_RIGHT

  // *** End ***
  , KEY_COUNT
};

STATIC_CHECK(sizeof(enum key_code) <= sizeof(u32), key_code_size_check);

enum input_event_kind 
{
    INPUT_EVENT_NONE
  , INPUT_EVENT_KEY
  , INPUT_EVENT_WINDOW
  , INPUT_EVENT_MOUSE_MOVE
  , INPUT_EVENT_MOUSE_WHEEL
  , INPUT_EVENT_QUIT
};

struct input_event
{
  enum input_event_kind kind;

  u32 key;
  u32 key_state;
  s32 wheel_delta;

  s32 window_x;
  s32 window_y;

  s32 mouse_x;
  s32 mouse_y;

  s32 mouse_delta_x;
  s32 mouse_delta_y;
};

struct input_event_view
{
  struct input_event* data;
  s32 len;
};

void poll_events();

struct input_event_view events_this_frame();

b8 is_key_down(u32 key);

// ============================================
// @: OpenGL Functions.
// ============================================

#ifdef TGF_OPENGL

// Utility function types.
typedef void (*GLDebugProc)(u32_enum source, u32_enum type, u32 id, u32_enum severity, s32 length, const char* message, const void* userParam);

// X Macro declaration for defining the OpenGL function prototypes.
#define FOR_OPEN_GL_FUNCTIONS(_DO)                                                 \
  _DO(glDebugMessageCallback, void, (GLDebugProc callback, const void* userParam)) \
  _DO(glCreateShader, u32, (u32_enum source))                                      \

// Use X macro to generate the function pointer declarations.
#define DO_FUNCTION_DECLARATIONS(_NAME, _RETURN, _PARAMS) extern _RETURN (*_NAME) _PARAMS; 

FOR_OPEN_GL_FUNCTIONS(DO_FUNCTION_DECLARATIONS)

#undef DO_FUNCTION_DECLARATIONS 
#endif

// ============================================
// @: OpenGL Context.
// ============================================

#ifdef TGF_OPENGL
b8 gl_context_create(struct window* window);
#endif

// ============================================
// @: OpenGL Context (Windows).
// ============================================

#if defined(TGF_OPENGL) && defined(TGF_WINDOWS)

typedef BOOL (WINAPI *PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL (WINAPI *PFNWGLSWAPINTERVALEXTPROC) (int interval);

extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

typedef struct GL_Win32_Context GL_Win32_Context;
struct GL_Win32_Context 
{ 
  HGLRC handle;
  HDC   device;
};

// @Note: 
// This array it is just going to mimic the window array.
// So creating a context just means occuping the same slot
// as the window.

extern GL_Win32_Context g_wgl_context_array[MAX_WINDOWS];

#endif // defined(TGF_OPENGL) && defined(TGF_WINDOWS)

// ============================================
// @: Graphics.
// ============================================

void clear_screen(union vec4 color);

#endif // TINY_GAME_FRAMEWORK_H

// ============================================================================================================================================================================================================================

#ifdef TGF_IMPL

// ============================================
// @i: Log and Assert macros.
// ============================================

#ifdef TGF_DEBUG

void trace_function(const char* fmt, ...) 
{
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  printf("\n");
}

void check_function(b8 expr, const char* fmt, ...) 
{
  if (expr == false) 
  {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
    STOP_AT_THIS_LINE();
  }
}

b8 ensure_function(b8 expr, const char* fmt, ...) 
{
  if (expr == false) 
  {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
    STOP_AT_THIS_LINE();
  }
  return expr != 0;
}

#endif // TGF_DEBUG

// ============================================
// @i: OS.
// ============================================

struct system_info g_system_info;

#ifdef TGF_WINDOWS

struct system_info* get_system_info()
{
  if (g_system_info.page_size == 0)
  {  
    SYSTEM_INFO sysinfo = nil;
    GetSystemInfo(&sysinfo);
    // @Pending: This we don't need this for now.
    // g_system_info.logical_processor_count = (u64)sysinfo.dwNumberOfProcessors;
    g_system_info.page_size = sysinfo.dwPageSize;
    g_system_info.large_page_size = GetLargePageMinimum();
    g_system_info.allocation_granularity = sysinfo.dwAllocationGranularity;
  }    
  check(g_system_info.page_size != 0);
  return &g_system_info;
}

void* reserve_large(u64 size)
{
  // We commit on reserve because windows. I trust.
  return VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT|MEM_LARGE_PAGES, PAGE_READWRITE);
}

b8 commit_large(void* ptr, u64 size) 
{
  UNUSED(ptr);
  UNUSED(size);
  return 1; 
}

void* reserve(u64 size)
{
  UNUSED(size);
  return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
}

b8 commit(void* ptr, u64 size)
{
  return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0;
}

void decommit(void* ptr, u64 size)
{
  VirtualFree(ptr, size, MEM_DECOMMIT);
}

void release(void* ptr, u64 size)
{
  // NOTE(rjf): size not used - not necessary on Windows, but necessary for other OSes.
  UNUSED(size);
  VirtualFree(ptr, 0, MEM_RELEASE);
}

#endif

// ============================================
// @i: Arena.
// ============================================

struct arena* arena_alloc(struct arena_params* params)
{
  // Round up reserve/commit sizes.
  u64 reserve_size = params->reserve_size;
  u64 commit_size = params->commit_size;
  if(params->flags & ARENA_FLAG_LARGE_PAGES)
  {
    reserve_size = ALIGN_POW2(reserve_size, get_system_info()->large_page_size);
    commit_size  = ALIGN_POW2(commit_size,  get_system_info()->large_page_size);
  }
  else
  {
    reserve_size = ALIGN_POW2(reserve_size, get_system_info()->page_size);
    commit_size  = ALIGN_POW2(commit_size,  get_system_info()->page_size);
  }
  
  // Reserve/commit initial block.
  void* base = params->optional_backing_buffer;
  if(base == 0)
  {
    if(params->flags & ARENA_FLAG_LARGE_PAGES)
    {
      base = reserve_large(reserve_size);
      commit_large(base, commit_size);
    }
    else
    {
      base = reserve(reserve_size);
      commit(base, commit_size);
    }
    
    // @Pending: Keep track of allocations like raddebugger does.
    //raddbg_annotate_vaddr_range(base, reserve_size, "arena %s:%i", params->allocation_site_file, params->allocation_site_line);
  }
  
  check_msg(base != 0, "Unexpected memory allocation failure.");
  
  // Extract arena header & fill.
  struct arena* arena = (struct arena*) base;
  arena->current = arena;
  arena->flags = params->flags;
  arena->cmt_size = params->commit_size;
  arena->res_size = params->reserve_size;
  arena->base_pos = 0;
  arena->pos = ARENA_HEADER_SIZE;
  arena->cmt = commit_size;
  arena->res = reserve_size;
  arena->allocation_site_file = params->allocation_site_file;
  arena->allocation_site_line = params->allocation_site_line;
#if ARENA_FREE_LIST
  arena->free_last = 0;
#endif
  // @Pending: Use adress sanitizer like raddebugger does.
  //AsanPoisonMemoryRegion(base, commit_size);
  //AsanUnpoisonMemoryRegion(base, ARENA_HEADER_SIZE);
  return arena;
}

struct arena* arena_alloc_default()
{
  struct arena_params params = DEFAULT_ARENA_PARAMS;
  return arena_alloc(&params);
}

void arena_release(struct arena* arena)
{
  for(struct arena* n = arena->current, *prev = 0; n != 0; n = prev)
  {
    prev = n->prev;
    release(n, n->res);
  }
}

// @Note: arena push/pop core functions

void* arena_push(struct arena* arena, u64 size, u64 align, b8 zero)
{
  struct arena* current = arena->current;
  u64 ppre = ALIGN_POW2(current->pos, align);
  u64 ppst = ppre + size;
  
  // Chain, if needed.
  if(current->res < ppst && !(arena->flags & ARENA_FLAG_NO_CHAIN))
  {
    struct arena* new_block = 0;
    
#if ARENA_FREE_LIST
    {
      struct arena* prev_block;
      for (new_block = arena->free_last, prev_block = 0; new_block != 0; prev_block = new_block, new_block = new_block->prev)
      {
        if (new_block->res >= ALIGN_POW2(new_block->pos, align) + size)
        {
          if (prev_block)
          {
            prev_block->prev = new_block->prev;
          }
          else
          {
            arena->free_last = new_block->prev;
          }
          break;
        }
      }
    }
#endif
    
    if (new_block == 0)
    {
      u64 res_size = current->res_size;
      u64 cmt_size = current->cmt_size;

      if (size + ARENA_HEADER_SIZE > res_size)
      {
        res_size = ALIGN_POW2(size + ARENA_HEADER_SIZE, align);
        cmt_size = ALIGN_POW2(size + ARENA_HEADER_SIZE, align);
      }
      struct arena_params params = DEFAULT_ARENA_PARAMS;
      params.reserve_size = res_size;
      params.commit_size = cmt_size;
      params.flags = current->flags;
      params.allocation_site_file = current->allocation_site_file;
      params.allocation_site_line = current->allocation_site_line;
      new_block = arena_alloc(&params);
    }
    
    new_block->base_pos = current->base_pos + current->res;
    new_block->prev = arena->current, arena->current = new_block;
    
    current = new_block;
    ppre = ALIGN_POW2(current->pos, align);
    ppst = ppre + size;
  }
  
  // Compute the size we need to zero.
  u64 size_to_zero = 0;
  if (zero)
  {
    size_to_zero = MIN(current->cmt, ppst) - ppre;
  }
  
  // Commit new pages, if needed.
  if (current->cmt < ppst)
  {
    u64 cmt_pst_aligned = ppst + current->cmt_size - 1;
    cmt_pst_aligned -= cmt_pst_aligned % current->cmt_size;
    u64 cmt_pst_clamped = MIN(cmt_pst_aligned, current->res);
    u64 cmt_size = cmt_pst_clamped - current->cmt;
    u8 *cmt_ptr = (u8*) current + current->cmt;

    if (current->flags & ARENA_FLAG_LARGE_PAGES)
    {
      commit_large(cmt_ptr, cmt_size);
    }
    else
    {
      commit(cmt_ptr, cmt_size);
    }
    current->cmt = cmt_pst_clamped;
  }
  
  // Push onto current block.
  void* result = 0;
  if (current->cmt >= ppst)
  {
    result = (u8 *)current+ppre;
    current->pos = ppst;

    // @Pending: Use adress sanitizer like raddebugger does.
    //AsanUnpoisonMemoryRegion(result, size);
    if (size_to_zero != 0)
    {
      memset(result, 0, size_to_zero);
    }
  }
  
  check_msg(result != 0, "Unexpected memory allocation failure.");
  return result;
}

u64 arena_pos(struct arena* arena)
{
  struct arena *current = arena->current;
  u64 pos = current->base_pos + current->pos;
  return pos;
}

void arena_pop_to(struct arena* arena, u64 pos)
{
  u64 big_pos = MAX(ARENA_HEADER_SIZE, pos);
  struct arena *current = arena->current;
  
#if ARENA_FREE_LIST
  for (struct arena* prev = NULL; current->base_pos >= big_pos; current = prev)
  {
    prev = current->prev;
    current->pos = ARENA_HEADER_SIZE;
    current->prev = arena->free_last, arena->free_last = current;

    // @Pending: Use adress sanitizer like raddebugger does.
    //AsanPoisonMemoryRegion((u8*)current + ARENA_HEADER_SIZE, current->res - ARENA_HEADER_SIZE);
  }
#else
  for (struct arena* prev = 0; current->base_pos >= big_pos; current = prev)
  {
    prev = current->prev;
    release(current, current->res);
  }
#endif
  arena->current = current;
  u64 new_pos = big_pos - current->base_pos;
  check(new_pos <= current->pos);

  // @Pending: Use adress sanitizer like raddebugger does.
  // AsanPoisonMemoryRegion((u8*)current + new_pos, (current->pos - new_pos));
  current->pos = new_pos;
}

void* arena_first_raw(struct arena* arena, u64 align)
{
  u64 raw_start = (u64) ((u8*) arena) + ARENA_HEADER_SIZE;
  u64 start = ALIGN_POW2(raw_start, align);
  return (void*) start;
}

// @Note: arena push/pop helpers

void arena_clear(struct arena* arena)
{
  arena_pop_to(arena, 0);
}

void arena_pop(struct arena* arena, u64 amt)
{
  u64 pold = arena_pos(arena);
  u64 pnew = pold;
  if(amt < pold)
  {
    pnew = pold - amt;
  }
  arena_pop_to(arena, pnew);
}

// @Note: temporary arena scopes

struct temp temp_begin(struct arena* arena)
{
  u64 pos = arena_pos(arena);
  struct temp temp = {arena, pos};
  return temp;
}

void temp_end(struct temp temp)
{
  arena_pop_to(temp.arena, temp.pos);
}

// ============================================
// @i: Input
// ============================================

static enum cursor_mode g_cursor_mode = CURSOR_MODE_DEFAULT;
static struct arena* g_input_events_arena = NULL;
static s32 g_input_events_len = 0;
static b8 g_key_down_table[KEY_COUNT] = nil;

#ifdef TGF_WINDOWS

static void update_cursor() 
{
  if (g_cursor_mode == CURSOR_MODE_DEFAULT)
  return;

  HWND fg = GetForegroundWindow();
  
  RECT rect;
  GetClientRect(fg, &rect);
  
  POINT ul = { rect.left,  rect.top };
  POINT lr = { rect.right, rect.bottom };

  ClientToScreen(fg, &ul);
  ClientToScreen(fg, &lr);

  rect.left = ul.x;
  rect.top = ul.y;
  rect.right = lr.x;
  rect.bottom = lr.y;
  
  ClipCursor(&rect);
  
  if (g_cursor_mode == CURSOR_MODE_HIDDEN)
  {
    // @Note: We center the cursor.
    SetCursorPos
    (
        (rect.left + rect.right)  / 2,
        (rect.top  + rect.bottom) / 2
    );
  }
}

void set_cursor_mode(enum cursor_mode mode)
{
  if (mode == g_cursor_mode)
  {
    return;
  }

  if (mode == CURSOR_MODE_DEFAULT)
  {
    ClipCursor(NULL);

    // @Robustness: This won't work on multiple windows in different threads.
    while (ShowCursor(TRUE) < 0) {}
  }
  else
  {
    // @Robustness: This won't work on multiple windows in different threads.
    while (ShowCursor(FALSE) >= 0) {}
  }
  g_cursor_mode = mode;
}

static u32 key_from_vk(WPARAM wParam)
{
  // *** ASCII KEYS ***
  if (wParam >= '0' && wParam <= '9')
  {
    return (u32) wParam;
  }

  if (wParam >= 'A' && wParam <= 'Z')
  {
    return (u32) wParam;
  }

  // *** NON ASCII ***
  switch (wParam)
  {
  case VK_BACK:       return KEY_BACKSPACE;
  case VK_TAB:        return KEY_TAB;
  case VK_RETURN:     return KEY_ENTER;
  case VK_ESCAPE:     return KEY_ESCAPE;
  case VK_SPACE:      return KEY_SPACE;
  case VK_DELETE:     return KEY_DELETE;
  case VK_UP:         return KEY_ARROW_UP;
  case VK_DOWN:       return KEY_ARROW_DOWN;
  case VK_LEFT:       return KEY_ARROW_LEFT;
  case VK_RIGHT:      return KEY_ARROW_RIGHT;
  case VK_PRIOR:      return KEY_PAGE_UP;
  case VK_NEXT:       return KEY_PAGE_DOWN;
  case VK_HOME:       return KEY_HOME;
  case VK_END:        return KEY_END;
  case VK_INSERT:     return KEY_INSERT;
  case VK_PAUSE:      return KEY_PAUSE;
  case VK_SCROLL:     return KEY_SCROLL_LOCK;
  case VK_MENU:       return KEY_ALT;
  case VK_CONTROL:    return KEY_CTRL;
  case VK_SHIFT:      return KEY_SHIFT;
  case VK_LWIN:
  case VK_RWIN:       return KEY_CMD;
  case VK_SNAPSHOT:   return KEY_PRINT_SCREEN;

  // @Note: Apparently this doesn't work.
  //case VK_LBUTTON:    return KEY_MOUSE_LEFT;
  //case VK_MBUTTON:    return KEY_MOUSE_MIDDLE;
  //case VK_RBUTTON:    return KEY_MOUSE_RIGHT;
  }
  
  // *** FUNCTION KEYS ***
  if (wParam >= VK_F1 && wParam <= VK_F24)
  {
    return (u32) (KEY_F1 + (wParam - VK_F1));
  }

  return KEY_UNKNOWN;
}

static CALLBACK LRESULT process_events(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  // @Pending:
  //if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
  //{
      //return true;
  //}

  switch (msg) 
  {
    case WM_INPUT: 
    {
      UINT size = 0u;
      GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));

      BYTE buffer[64];
      GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER));
      
      RAWINPUT* raw = (RAWINPUT*)buffer;
      
      if (raw->header.dwType == RIM_TYPEMOUSE) 
      {
        LONG dx = raw->data.mouse.lLastX;
        LONG dy = raw->data.mouse.lLastY;
        
        struct input_event* ev = arena_push_element(g_input_events_arena, struct input_event);
        g_input_events_len += 1;
        ev->kind = INPUT_EVENT_MOUSE_MOVE;
        
        ev->mouse_delta_x = dx;
        ev->mouse_delta_y = dy;

        ev->mouse_x = (s32)(short)LOWORD(lParam);
        ev->mouse_y = (s32)(short)HIWORD(lParam);
      }
      break;
    }
    case WM_SIZE: 
    {
      struct input_event* ev = arena_push_element(g_input_events_arena, struct input_event);
      g_input_events_len += 1;
      ev->kind = INPUT_EVENT_WINDOW;
      ev->window_x = LOWORD(lParam);
      ev->window_y = HIWORD(lParam);
      update_cursor();
    }
    break;

    case WM_MOVE: 
    {
      update_cursor();
    }
    break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: 
    {
      struct input_event* ev = arena_push_element(g_input_events_arena, struct input_event);
      g_input_events_len += 1;
      ev->kind = INPUT_EVENT_KEY;
      ev->key = key_from_vk(wParam);

      ev->key_state = KEY_STATE_DOWN;

      b8 is_repeat = (lParam & (1 << 30)) != 0;

      if (!is_repeat)
      {
        ev->key_state |= KEY_STATE_START;
      }

      g_key_down_table[ev->key] = true;
    }
    break;

    case WM_KEYUP:
    case WM_SYSKEYUP: 
    {
      struct input_event* ev = arena_push_element(g_input_events_arena, struct input_event);
      g_input_events_len += 1;
      ev->kind = INPUT_EVENT_KEY;
      ev->key = key_from_vk(wParam);

      ev->key_state = KEY_STATE_END;

      g_key_down_table[ev->key] = false;
    }
    break;

    case WM_MOUSEMOVE: 
    {
        update_cursor();
    }
    break;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP: 
    {
      struct input_event* ev = arena_push_element(g_input_events_arena, struct input_event);
      g_input_events_len += 1;
      ev->kind = INPUT_EVENT_KEY;
      ev->key = KEY_MOUSE_LEFT;
      
      b8 down = msg == WM_LBUTTONDOWN;
      ev->key_state = down ? KEY_STATE_DOWN : KEY_STATE_END;
      g_key_down_table[ev->key] = down;
    }
    break;

    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP: 
    {
      struct input_event* ev = arena_push_element(g_input_events_arena, struct input_event);
      g_input_events_len += 1;
      ev->kind = INPUT_EVENT_KEY;
      ev->key = KEY_MOUSE_RIGHT;
      
      b8 down = msg == WM_RBUTTONDOWN;
      ev->key_state = down ? KEY_STATE_DOWN : KEY_STATE_END;
      g_key_down_table[ev->key] = down;
    }
    break;

    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP: 
    {
      struct input_event* ev = arena_push_element(g_input_events_arena, struct input_event);
      g_input_events_len += 1;
      ev->kind = INPUT_EVENT_KEY;
      ev->key = KEY_MOUSE_MIDDLE;
      
      b8 down = msg == WM_MBUTTONDOWN;
      ev->key_state = down ? KEY_STATE_DOWN : KEY_STATE_END;
      g_key_down_table[ev->key] = down;
    }
    break;

    case WM_MOUSEWHEEL: 
    {
      struct input_event* ev = arena_push_element(g_input_events_arena, struct input_event);
      g_input_events_len += 1;
      ev->kind = INPUT_EVENT_MOUSE_WHEEL;
      ev->wheel_delta = (s32)GET_WHEEL_DELTA_WPARAM(wParam);
    }
    break;

    case WM_CLOSE:
    case WM_QUIT: 
    {
      struct input_event* ev = arena_push_element(g_input_events_arena, struct input_event);
      g_input_events_len += 1;
      ev->kind = INPUT_EVENT_QUIT;
    }
    break;
    case WM_KILLFOCUS:
      memset(g_key_down_table, 0, sizeof(g_key_down_table));
    break;
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

void poll_events()
{
  arena_clear(g_input_events_arena);
  g_input_events_len = 0;

  MSG msg = {};
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

struct input_event_view events_this_frame()
{
  struct input_event_view view;
  view.data = arena_first(g_input_events_arena, struct input_event);
  view.len  = g_input_events_len;
  return view;
}

b8 is_key_down(u32 key)
{
  return g_key_down_table[key];
}

// ============================================
// @i: Window.
// ============================================

#define WIN32_CLASS_NAME L"Window Class"
#define WIN32_STYLE_WINDOWED    WS_OVERLAPPEDWINDOW
#define WIN32_STYLE_FULLSCREEN  WS_VISIBLE | WS_POPUP
#define WIN32_STYLE_SECONDARY   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME

#define WIN32_STYLE_DEFAULT WIN32_STYLE_WINDOWED 

struct
{
  struct window data[MAX_WINDOWS];
  u32 len;
} 
static g_window_array;
static b8 g_win32_class_registered = false;
static b8 g_win32_input_device_registered = false;
static HWND g_win32_main_window = NULL;

static b8 create_win32_class_once(struct window_params* params)
{
  if (g_win32_class_registered == false)
  {
    SetProcessDPIAware();
    // @Note:

    // DPI (Dots Per Inch): Indican cuántos píxeles hay en una
    // pulgada física de pantalla. Windows en monitores con alto
    // DPI puede llegar a escalar automáticamente tu ventana para 
    // que los elementos con pocos píxeles se continuen viendo.
    // Con esta llamada le comunicas a Windows "I got u, buddy".

    timeBeginPeriod(1);
    // @Note:

    // El Kernel de windows tiene un tick rate con el que actualiza
    // como el cambio de hilos (await/sleep) y la actualización de los
    // timers. Con esta llamada especificas que lo haga con una frecuencia
    // de un milisegundo, en vez de la default. Esto hace que esas 
    // actualizaciones sucedan con más frecuencia. A coste de más consumo
    // de CPU se gana más estabilidad en las partes de la aplicación que
    // dependen de esto.

    // @Note: This is the handle of the calling process.
    // @Robustness: This does not work on dlls.
    HINSTANCE hInstance = GetModuleHandle(0);

    WNDCLASSEXW wc = nil;
    wc.lpfnWndProc = &process_events;

    // @Note Use this to run the default event processing.
    //wc.lpfnWndProc = &DefWindowProc;
    
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);     // Default cursor.
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);   // Default icon.
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // Small icon.
    wc.hInstance = hInstance;
    wc.lpszClassName = WIN32_CLASS_NAME;
    // @Note: CreateSolidBrush changes the background color, I guess.
    wc.hbrBackground = CreateSolidBrush(RGB(params->bg_color.x * 255, params->bg_color.y * 255, params->bg_color.z * 255));
    g_win32_class_registered = RegisterClassExW(&wc) != 0;

    if (!ensure_msg(g_win32_class_registered != false, "Error! Couldn't register the Win32 window class."))
    {
        return false;
    }

    trace("Win32 struct window class registered!");
    return true;
  }
  return false;
}

static HWND create_win32_window(struct window_params* params)
{
  // Calculate the RECT -----------
  RECT rect;
  rect.top = 0;
  rect.left = 0;
  rect.right = params->width;
  rect.bottom = params->height;

  AdjustWindowRect(&rect, WIN32_STYLE_DEFAULT, 0);

  s32 client_width = rect.right - rect.left;
  s32 client_height = rect.bottom - rect.top;

  // Convert title to wchar_t* -----------
  #define MAX_TITLE_LEN 256u
  wchar_t title_buffer[MAX_TITLE_LEN + 1];
  {
    u64 title_len = strlen(params->title);
    if (!ensure_msg(title_len <= MAX_TITLE_LEN, "Error! The title len is %llu chars. MAX is %llu.", title_len, MAX_TITLE_LEN))
    {
      return NULL;
    }

    s32 len = MultiByteToWideChar
    (
      CP_UTF8,
      0,                 // flags.
      params->title,     // input.
      -1,                // (-1 = null-terminated).
      title_buffer,      // output buffer.
      MAX_TITLE_LEN + 1    // buffer len.
    );

    if (len == 0) 
    {
      DWORD err = GetLastError();
      trace("Error! %lu\n", err);
      STOP_AT_THIS_LINE();
      return NULL;
    }
  }
  // --------------------------------------

  // @Note: Create the window.
  
  HWND hwnd = CreateWindowExW
  (
    0,                          // Optional window styles.
    WIN32_CLASS_NAME,       // struct window class.
    title_buffer,               // struct window title
    WIN32_STYLE_DEFAULT,    // struct window style.
    CW_USEDEFAULT,              // X Pos.
    CW_USEDEFAULT,              // Y Pos.
    client_width,               // Width.
    client_height,              // Height.
    NULL,                       // Parent window.
    NULL,                       // Menu.
    0,                          // Instance handle. @Note: Apparently only required for Win98.
    NULL                        // Additional application data.
  );

  return hwnd;
}

b8 create_win32_input_device_once(HWND hwnd)
{
  if (g_win32_input_device_registered == false)
  {
    // @Note: Register the input device.
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01; // Generic Desktop
    rid.usUsage = 0x02;     // Mouse
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = hwnd;
    g_win32_input_device_registered = (b8) RegisterRawInputDevices(&rid, 1, sizeof(rid));
    
    return true;
  }
  return false;
}

struct window* create_window_default()
{
  struct window_params params = DEFAULT_WINDOW_PARAMS;
  return create_window(&params);
}

struct window* create_window(struct window_params* params)
{
  if (!ensure_msg(g_window_array.len < MAX_WINDOWS, "struct window limit reached!"))
  {
    return NULL;
  }

  // Get the window slot.
  struct window* window = NULL;

  for each_index(it, MAX_WINDOWS)
  {
    struct window*  curr = &g_window_array.data[it];
    if (curr->handle == NULL)
    {
      window = curr;
      break;
    }
  }
  
  if (!ensure_msg(window != NULL, "Unexpected window alloc error!"))
  {
    return NULL;
  }

  // Create the window class once.
  b8 created = create_win32_class_once(params);

  // If the window class is created we zero all the window array memory just in case.
  if (created)
  {
    memset(&g_window_array, 0, sizeof(g_window_array));
  }

  // Create Windows window.
  HWND hwnd = create_win32_window(params);

  if (!ensure_msg(hwnd != 0, "Error! Couldn't create the Win32 window."))
  {
    return NULL;
  }

  trace("Win32 struct window created!");
  
  // Create the window input device once.
  created = create_win32_input_device_once(hwnd);
  
  // If the input device is created setup the input event arena .
  if (created)
  {
    check(g_input_events_arena == NULL);
    struct arena_params params = DEFAULT_ARENA_PARAMS;
    params.flags &= ARENA_FLAG_NO_CHAIN;
    g_input_events_arena = arena_alloc_default();
  }

  // Fill the window slot.
  window->handle = hwnd;
  window->index  = g_window_array.len;
  g_window_array.len += 1;

#ifdef TGF_OPENGL
  // @Review: Create the opengl context.
  b8 success = gl_context_create(window);
  UNUSED(success);
#endif
  
  // Show the window.
  SetForegroundWindow(hwnd);
  UpdateWindow(hwnd);
  ShowWindow(hwnd, SW_SHOW);

  return window;
}

void destroy_window(struct window*  window)
{
  if (!ensure_msg((g_window_array.len > 0 && window->handle != NULL), "Not a valid window!"))
  {
    return;
  }

  b8 found = false;
  for each_index(it, MAX_WINDOWS)
  {
    struct window*  curr = &g_window_array.data[it];
    if (curr == window)
    {
        found = true;
        break;
    }
  }

  if (!ensure_msg(found, "struct window not found!"))
  {
    return;
  }

  HWND hwnd = (HWND) window->handle;

  DestroyWindow(hwnd);
  trace("Win32 struct window destroyed!");

  if (hwnd == g_win32_main_window)
  {
    if (g_win32_input_device_registered)
    {
      RAWINPUTDEVICE rid;
      rid.usUsagePage = 0x01; // Generic Desktop
      rid.usUsage = 0x02;     // Mouse
      rid.dwFlags = RIDEV_REMOVE;
      rid.hwndTarget = hwnd;
      RegisterRawInputDevices(&rid, 1, sizeof(rid));
      g_win32_input_device_registered = false;
      check(g_input_events_arena != NULL);
      arena_clear(g_input_events_arena);
    }
    // @Pending: Free the window class and set g_win32_class_registered to false.
    g_win32_main_window = NULL;
  }

  memset(window, 0, sizeof(struct window));
  g_window_array.len -= 1;
}

#ifdef TGF_OPENGL

void swap_buffers(struct window*  window, b8 vsync)
{
  wglSwapIntervalEXT(vsync ? 1 : 0);
  check(window->handle != NULL);
  GL_Win32_Context* context = &g_wgl_context_array[window->index];
  check(context->device != NULL);
  SwapBuffers(context->device);
}

#endif

#endif // TGF_WINDOWS

// ============================================
// @i: OpenGL Functions.
// ============================================

#ifdef TGF_OPENGL

// Use X macro to generate the function pointers definitions.
#define DO_FUNCTION_POINTER_DEFINITIONS(_NAME, _RETURN, _PARAMS) _RETURN (*_NAME) _PARAMS = NULL;                               \

FOR_OPEN_GL_FUNCTIONS(DO_FUNCTION_POINTER_DEFINITIONS)

#undef DO_FUNCTION_POINTER_DEFINITIONS 

#endif // TGF_OPENGL

// ============================================
// @i: OpenGL Context (Windows).
// ============================================

#if defined(TGF_OPENGL) && defined(TGF_WINDOWS)

GL_Win32_Context g_wgl_context_array[MAX_WINDOWS] = nil;

// @Note: Windows sucks.

PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB = NULL;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;

HMODULE ogl = NULL;

static void* resolve_gl_proc(const char* name)
{
	void* p = (void*) wglGetProcAddress(name);

	// @Note: Illegal values returned by wglGetProcAddress.
	if (p == NULL     ||
		p == (void*) 0x1 ||
		p == (void*) 0x2 ||
		p == (void*) 0x3 ||
		p == (void*) -1)
	{
    if (ogl == NULL)
    {
		  ogl = LoadLibrary(L"opengl32.dll");
    }
		p = (void*) GetProcAddress(ogl, name);
	}

	return p;
}

static void resolve_gl_procs()
{
   // Use X macro to generate the function pointer assignments.
   #define DO_FUNCTION_RESOLVE(_NAME, _RETURN, _PARAMS) _NAME = ( _RETURN(*) _PARAMS ) resolve_gl_proc(#_NAME);
 
   FOR_OPEN_GL_FUNCTIONS(DO_FUNCTION_RESOLVE)

   #undef DO_FUNCTION_RESOLVE 
}

b8 resolve_wgl_procs()
{
  // @Note: I guess we need to resolve this functions just once.
  if (wglChoosePixelFormatARB && wglCreateContextAttribsARB && wglSwapIntervalEXT)
  {
    return false;   
  }

  // @Pending: Do we need to reset this pointers if the class is destroyed?
  if (!ensure_msg(g_win32_class_registered, "We need at least one window created at this point."))
  {
    return false;   
  }
	// @Note: Old OpenGL context. Needed to load wgl extensions.
	
	// Dummy window creation.

  struct window_params dummy_params = make_struct(window_params,
    .width    = 0,
    .height   = 0,
    .title    = "Dummy",
    .bg_color = nil,
  );

  HWND window = create_win32_window(&dummy_params);
  check(window != NULL);

	// @Note: HDC stands for handle to device context.
	HDC dc = GetDC(window);
	PIXELFORMATDESCRIPTOR ds = {};
	
	ds.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	ds.nVersion = 1;
	ds.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	ds.iPixelType = PFD_TYPE_RGBA;
	ds.cColorBits = 32;
	ds.cDepthBits = 24;
	ds.iLayerType = PFD_MAIN_PLANE;
	
	s32 pf = ChoosePixelFormat(dc, &ds);
	SetPixelFormat(dc, pf, &ds);
	
	// @Note: HGLRC stands for handle to OpenGL rendering context. 
	HGLRC context = wglCreateContext(dc);
	wglMakeCurrent(dc, context);
	
	// @Note: Resolve wgl extension functions.
	wglChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC) (void*) (wglGetProcAddress("wglChoosePixelFormatARB"));
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) (void*) (wglGetProcAddress("wglCreateContextAttribsARB"));
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) (void*) (wglGetProcAddress("wglSwapIntervalEXT"));
	
	// @Note: Destroy the old context and the dummy window.
	wglMakeCurrent(dc, 0);
	wglDeleteContext(context);

  return true;
}

b8 gl_context_create(struct window* window)
{
  if (!ensure_msg(window->handle != NULL, "The window must to be valid!"))
  {
    return false;
  }
  
  GL_Win32_Context* context = &g_wgl_context_array[window->index];

  if (!ensure_msg(context->handle == NULL, "This window has already an associated context!"))
  {
    return false;
  }

  b8 result = resolve_wgl_procs();
  UNUSED(result);

  s32 WGL_DRAW_TO_WINDOW_ARB     = 0x2001;
  s32 WGL_SUPPORT_OPENGL_ARB     = 0x2010;
  s32 WGL_DOUBLE_BUFFER_ARB      = 0x2011;
  s32 WGL_ACCELERATION_ARB       = 0x2003;
  s32 WGL_FULL_ACCELERATION_ARB  = 0x2027;
  s32 WGL_PIXEL_TYPE_ARB         = 0x2013;
  s32 WGL_COLOR_BITS_ARB         = 0x2014;
  s32 WGL_DEPTH_BITS_ARB         = 0x2022;
  s32 WGL_STENCIL_BITS_ARB       = 0x2023;
  s32 WGL_TYPE_RGBA_ARB          = 0x202B;

	// @Note: Modern OpenGL initialization.
	s32 pixel_forattribs[] = 
  {
		WGL_DRAW_TO_WINDOW_ARB      , true,
		WGL_SUPPORT_OPENGL_ARB      , true,
		WGL_DOUBLE_BUFFER_ARB       , true,
		WGL_ACCELERATION_ARB        , WGL_FULL_ACCELERATION_ARB,
		WGL_PIXEL_TYPE_ARB          , WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB          , 32,
		WGL_DEPTH_BITS_ARB          , 24,
		WGL_STENCIL_BITS_ARB        , 8,
		0	
	};
	
	HDC dc = GetDC((HWND) window->handle);
	s32 pixel_format = 0;
	u32 num_formats  = 0u;

	wglChoosePixelFormatARB(dc, pixel_forattribs, 0, 1, &pixel_format, &num_formats);

	PIXELFORMATDESCRIPTOR pfd;
	DescribePixelFormat(dc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	SetPixelFormat(dc, pixel_format, &pfd);

  s32 WGL_CONTEXT_MAJOR_VERSION_ARB    = 0x2091;
  s32 WGL_CONTEXT_MINOR_VERSION_ARB    = 0x2092;
  s32 WGL_CONTEXT_PROFILE_MASK_ARB     = 0x9126;
  s32 WGL_CONTEXT_CORE_PROFILE_BIT_ARB = 0x00000001;
  s32 WGL_CONTEXT_FLAGS_ARB            = 0x2094;
  s32 WGL_CONTEXT_DEBUG_BIT_ARB        = 0x00000001;

	s32 context_attribs[] = 
  {
		WGL_CONTEXT_MAJOR_VERSION_ARB  , 4,
		WGL_CONTEXT_MINOR_VERSION_ARB  , 6,
		WGL_CONTEXT_PROFILE_MASK_ARB   , WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		
		#ifdef TGF_DEBUG 
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
		#endif
		
		0 
	};
	
	HGLRC hglrc = wglCreateContextAttribsARB(dc, 0, context_attribs);
	wglMakeCurrent(dc, hglrc);
	
	context->handle = hglrc;
	context->device = dc;
	
	resolve_gl_procs();

  trace("Win32 OpenGL Context created!");
  return true;
}

#endif // TGF_OPENGL && TGF_WINDOWS
 
// ============================================
// @i: Graphics (OpenGL).
// ============================================

#ifdef TGF_OPENGL

void clear_screen(union vec4 color)
{
  glClearColor(color.x, color.y, color.z, color.w);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

#endif

#endif // TGF_IMPL