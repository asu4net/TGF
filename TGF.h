// ============================================
// @: Info.
// ============================================

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
// TGF_DEBUG  Enables Log, Check, Ensure macros.
// TGF_OPENGL Uses OpenGL.

#ifndef TINY_GAME_FRAMEWORK_H
#define TINY_GAME_FRAMEWORK_H

// ============================================
// @: Platform detection.
// ============================================

// Platform detection using predefined macros.
#ifdef _WIN32
	/* Windows x64/x86 */
	#ifdef _WIN64
		/* Windows x64  */
		#define TGF_WINDOWS
	#else
		/* Windows x86 */
		#error "x86 Builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>
	/* TARGET_OS_MAC exists on all the platforms
	 * so we must check all of them (in this order)
	 * to ensure that we're running on MAC
	 * and not some other Apple platform */
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
/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
	#define TGF_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define TGF_LINUX 
	#error "Linux is not supported!"
#else
	/* Unknown compiler/platform */
	#error "Unknown platform!"
#endif

// Language detection.
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
    //#define WIN32_LEAN_AND_MEAN //@Note: Removed because timeBeginPeriod
    // Win32 API and Unicode proc versions.
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

typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;
typedef float    F32;
typedef double   F64;

typedef U32      U32Enum;
typedef S32      S32Size;

// ============================================
// @: Bool.
// ============================================

#ifdef TGF_C

// Just in case lets undef some true and false.
#undef true
#undef false

#define true  ((B8) 1)
#define false ((B8) 0)

#endif

// ============================================
// @: Units.
// ============================================

#define KB(n)  (((U64)(n)) << 10)
#define MB(n)  (((U64)(n)) << 20)
#define GB(n)  (((U64)(n)) << 30)
#define TB(n)  (((U64)(n)) << 40)
#define Thousand(n)   ((n)*1000)
#define Million(n)    ((n)*1000000)
#define Billion(n)    ((n)*1000000000)

// ============================================
// @: Clamp, Min, Max.
// ============================================

#define Min(A,B) (((A)<(B))?(A):(B))
#define Max(A,B) (((A)>(B))?(A):(B))
#define Clamp(A,X,B) (((X)<(A))?(A):((X)>(B))?(B):(X))

// ============================================
// @: AlignOf.
// ============================================

// @Pending(Platform): This will work just in clang and msvc.
#define AlignOf(T) __alignof(T)

// ============================================
// @: Helper Macros.
// ============================================

#define InternalStringify(S) #S
#define Stringify(S) InternalStringify(S)

#define InternalGlue(A,B) A##B
#define Glue(A,B) InternalGlue(A,B)

#define EachIndex(it, count) (S32 it = 0; it < (count); it += 1)
#define DeferFor(begin, end) for(S32 _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#define AlignPow2(x,b) (((x) + (b) - 1)&(~((b) - 1))) // @Note: b has to be pow2. 

#if defined(TGF_CPP)
  #define ZeroStruct
#elif defined(TGF_C)
  #define ZeroStruct 0
#endif

#if defined(TGF_CPP)
  #define MakeStruct(T, ...) { __VA_ARGS__ }
#elif defined(TGF_C)
  #define MakeStruct(T, ...) (T){ __VA_ARGS__ }
#endif

#define UnusedVar(v) ((void)v)

// ============================================
// @: Log and Assert/Check.
// ============================================

#ifdef TGF_DEBUG
void internal_log(const char* fmt, ...);
void internal_check(B8 expr, const char* fmt, ...);
B8   internal_ensure(B8 expr, const char* fmt, ...);
#endif

#ifdef TGF_DEBUG
    #define Log(X, ...) (internal_log(X, ##__VA_ARGS__))
#else
    #define Log(X, ...)
#endif

#ifdef TGF_DEBUG
    #ifdef TGF_WINDOWS
        #define StopAtThisLine() __debugbreak()
    #else
        /* @Pending(Platform): This just works on Windows. */
        #define StopAtThisLine()
#endif
#else
    #define StopAtThisLine()
#endif

#ifdef TGF_DEBUG
    #define Check(X) internal_check((X), "Check failed: %s\n", #X)
    #define CheckMsg(X, ...) internal_check((X), __VA_ARGS__)
    #define StaticCheck(C, ID) static U8 Glue(ID, __LINE__)[(C)?1:-1] // @Review: Maybe we shouldn't get rid of this one, ever.
    #define Ensure(X) internal_ensure((X), "Ensure failed: %s\n", #X)
    #define EnsureMsg(X, ...) internal_ensure((X), __VA_ARGS__)
#else
    #define Check(X) ((void)0)
    #define CheckMsg(...) ((void)0)
    #define StaticCheck(C, ID)
    #define Ensure(X) (X)
    #define EnsureMsg(X, ...) (X)
#endif

// ============================================
// @: OS.
// ============================================

// @Pending(Platform): All the OS code it is just windows for now.

typedef struct OS_System_Info OS_System_Info;
struct OS_System_Info
{
  U64 page_size;
  U64 large_page_size;
  U64 allocation_granularity;
};

OS_System_Info system_info = {ZeroStruct};

#ifdef TGF_WINDOWS

OS_System_Info* 
os_get_system_info();

void* 
os_reserve_large(U64 size);

B32 
os_commit_large(void *ptr, U64 size);

void*
os_reserve(U64 size);

B32
os_commit(void *ptr, U64 size);

void
os_decommit(void *ptr, U64 size);

void
os_release(void *ptr, U64 size);

#endif

// ============================================
// @: Arena.
// ============================================

#define ARENA_HEADER_SIZE 128
#define ARENA_FREE_LIST 1

typedef U64 Arena_Flags;
enum
{
  ArenaFlag_NoChain    = (1<<0),
  ArenaFlag_LargePages = (1<<1),
};

typedef struct Arena_Params Arena_Params;
struct Arena_Params
{
  Arena_Flags flags;
  U64 reserve_size;
  U64 commit_size;
  void *optional_backing_buffer;
  const char *allocation_site_file;
  S32 allocation_site_line;
};

typedef struct Arena Arena;
struct Arena
{
  Arena *prev;    // previous arena in chain
  Arena *current; // current arena in chain
  Arena_Flags flags;
  U64 cmt_size;
  U64 res_size;
  U64 base_pos;
  U64 pos;
  U64 cmt;
  U64 res;
  const char* allocation_site_file;
  S32 allocation_site_line;
#if ARENA_FREE_LIST
  Arena *free_last;
#endif
};

StaticCheck(sizeof(Arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

typedef struct Temp Temp;
struct Temp
{
  Arena *arena;
  U64 pos;
};

////////////////////////////////
//~ rjf: Arena Functions

Arena_Flags arena_default_flags = 0;
U64 arena_default_reserve_size = MB(64);
U64 arena_default_commit_size  = KB(64);

// @Note: arena creation/destruction
#define Arena_Params_Default MakeStruct(Arena_Params,    \
  .flags                   = arena_default_flags,        \
  .reserve_size            = arena_default_reserve_size, \
  .commit_size             = arena_default_commit_size,  \
  .optional_backing_buffer = NULL,                       \
  .allocation_site_file    = __FILE__,                   \
  .allocation_site_line    = __LINE__                    \
)                                                        \

Arena* 
arena_alloc(Arena_Params *params);

Arena* 
arena_alloc_default();

void
arena_release(Arena *arena);

// @Note: Arena push/pop/pos/begin core functions.
void*
arena_push(Arena *arena, U64 size, U64 align, B32 zero);

U64
arena_pos(Arena *arena);

void  
arena_pop_to(Arena *arena, U64 pos);

void* 
arena_begin_raw(Arena* arena, U64 align);

// @Note: Arena push/pop helpers.
void 
arena_clear(Arena *arena);

void 
arena_pop(Arena *arena, U64 amt);

// @Note: Temporary arena scopes.
Temp
temp_begin(Arena *arena);

void 
temp_end(Temp temp);

// @Note: Push helper macros.
#define arena_push_array_no_zero_aligned(a, T, c, align) \
  (T *) arena_push((a), sizeof(T)*(c), (align), (0))     \

#define arena_push_array_aligned(a, T, c, align)     \
  (T *) arena_push((a), sizeof(T)*(c), (align), (1)) \

#define arena_push_array_no_zero(a, T, c)                       \
  arena_push_array_no_zero_aligned(a, T, c, Max(8, AlignOf(T))) \

#define arena_push_array(a, T, c)                       \
  arena_push_array_aligned(a, T, c, Max(8, AlignOf(T))) \

#define arena_push_element(a, T) (T *)        \
  arena_push((a), sizeof(T), AlignOf(T), (1)) \

#define arena_begin(a, T) (T *)    \
  arena_begin_raw((a), AlignOf(T)) \

// ============================================
// @: Vec2.
// ============================================

typedef union Vec2
{
  struct
  {
    F32 x;
    F32 y;
  };

  F32 v[2];
} 
Vec2;

// ============================================
// @: Vec3.
// ============================================

typedef union Vec3
{
  struct
  {
    F32 x;
    F32 y;
    F32 z;
  };

  struct
  {
    Vec2 xy;
    F32 _z;
  };

  struct
  {
    F32 _x;
    Vec2 yz;
  };
    
  F32 v[3];
}
Vec3;

// ============================================
// @: Vec4.
// ============================================

typedef union Vec4
{
  struct
  {
    F32 x;
    F32 y;
    F32 z;
    F32 w;
  };
    
  struct
  {
    Vec2 xy;
    Vec2 zw;
  };
    
  struct
  {
    Vec3 xyz;
    F32 _w;
  };
    
  struct
  {
    F32 _x;
    Vec3 yzw;
  };

  F32 v[4];
} Vec4;

// ============================================
// @: Colors. (Vec4)
// ============================================

#define Vec4_White            MakeStruct(Vec4, .x=1.000f, .y=1.000f, .z=1.000f, .w=1.000f)
#define Vec4_White_Faded      MakeStruct(Vec4, .x=1.000f, .y=1.000f, .z=1.000f, .w=0.000f)
#define Vec4_Black            MakeStruct(Vec4, .x=0.000f, .y=0.000f, .z=0.000f, .w=1.000f)
#define Vec4_Corn_Flower_Blue MakeStruct(Vec4, .x=0.388f, .y=0.584f, .z=0.933f, .w=1.000f)
#define Vec4_Blue             MakeStruct(Vec4, .x=0.000f, .y=0.000f, .z=1.000f, .w=1.000f)
#define Vec4_Light_Blue       MakeStruct(Vec4, .x=0.300f, .y=0.300f, .z=1.000f, .w=1.000f)
#define Vec4_Cyan             MakeStruct(Vec4, .x=0.000f, .y=1.000f, .z=1.000f, .w=1.000f)
#define Vec4_Gray             MakeStruct(Vec4, .x=0.500f, .y=0.500f, .z=0.500f, .w=1.000f)
#define Vec4_Dark_Gray        MakeStruct(Vec4, .x=0.200f, .y=0.200f, .z=0.200f, .w=1.000f)
#define Vec4_Green            MakeStruct(Vec4, .x=0.000f, .y=1.000f, .z=0.000f, .w=1.000f)
#define Vec4_Light_Green      MakeStruct(Vec4, .x=0.300f, .y=1.000f, .z=0.300f, .w=1.000f)
#define Vec4_Chill_Green      MakeStruct(Vec4, .x=0.047f, .y=0.651f, .z=0.408f, .w=1.000f)
#define Vec4_Magenta          MakeStruct(Vec4, .x=1.000f, .y=0.000f, .z=1.000f, .w=1.000f)
#define Vec4_Red              MakeStruct(Vec4, .x=1.000f, .y=0.000f, .z=0.000f, .w=1.000f)
#define Vec4_Light_Red        MakeStruct(Vec4, .x=1.000f, .y=0.300f, .z=0.300f, .w=1.000f)
#define Vec4_Yellow           MakeStruct(Vec4, .x=1.000f, .y=0.920f, .z=0.016f, .w=1.000f)
#define Vec4_Orange           MakeStruct(Vec4, .x=0.970f, .y=0.600f, .z=0.110f, .w=1.000f)


// ============================================
// @: Window.
// ============================================

typedef struct Window Window;
struct Window 
{
  void* handle; // Handle to the platform-specific window. (Maybe we should change this later)
  U32 index;    // The index occupied in the window array.
};

typedef struct Window_Params Window_Params;
struct Window_Params
{
  S32 width;
  S32 height;
  const char* title;
  Vec4 bg_color;
};

Window_Params Window_Params_Default = MakeStruct(Window_Params,
  .width    = 1920,
  .height   = 1080,
  .title    = "Application",
  .bg_color = Vec4_Corn_Flower_Blue,
);

Window* 
window_create(Window_Params* params);

Window* 
window_create_default();

void 
window_destroy(Window* window);

// ============================================
// @: Input.
// ============================================

typedef enum Cursor_Mode
{
    Cursor_Mode_Default
  , Cursor_Mode_Confined
  , Cursor_Mode_Hidden
} 
Cursor_Mode;

// @Note: Common use case.
// 
// Key_State_Pressed  -> Key_State_Start | Key_State_Down
// Key_State_Repeat   -> Key_State_Down
// Key_State_Released -> Key_State_End

typedef enum Key_State
{
    Key_State_None  = 1 << 0
  , Key_State_Down  = 1 << 1
  , Key_State_Start = 1 << 2 
  , Key_State_End   = 1 << 3
} 
Key_State;

typedef enum Key
{
  Key_Unknown = 0
    
  // *** ASCII (REAL CODES) ***

  , Key_Backspace    = 8
  , Key_Tab          = 9
  , Key_Linefeed     = 10
  , Key_Enter        = 13
  , Key_Escape       = 27
  , Key_Space        = 32

  // @Note: Here live ASCII letters, digits and symbols via direct char input.
  // They are not enumerated because they are received as char or UTF-32 text.

  // *** END OF ASCII ***
  // Begin custom keycodes (physical keys)

  , _Key_BEGIN_NON_ASCII = 512 // @Note: Explicit boundary to avoid collisions.

  , Key_Delete       
  , Key_Arrow_Up     
  , Key_Arrow_Down   
  , Key_Arrow_Left   
  , Key_Arrow_Right  
  , Key_Page_Up      
  , Key_Page_Down    
  , Key_Home         
  , Key_End          
  , Key_Insert       
  , Key_Pause        
  , Key_Scroll_Lock  
  , Key_Alt          
  , Key_Ctrl         
  , Key_Shift        
  , Key_CMD          

  // *** FUNCTION KEYS ***
  , Key_F1  , Key_F2  , Key_F3  , Key_F4  , Key_F5  , Key_F6  , Key_F7  , Key_F8  , Key_F9 , Key_F10
  , Key_F11 , Key_F12 , Key_F13 , Key_F14 , Key_F15 , Key_F16 , Key_F17 , Key_F18 , Key_F19
  , Key_F20 , Key_F21 , Key_F22 , Key_F23 , Key_F24

  , Key_Print_Screen

  // *** MOUSE AS BUTTON INPUT KEYS ***
  , Key_Mouse_Left
  , Key_Mouse_Middle
  , Key_Mouse_Right

  // *** End ***
  , Key_Count
} 
Key;

typedef enum Input_Event_Kind 
{
    Input_Event_Kind_None
  , Input_Event_Kind_Key
  , Input_Event_Kind_Window
  , Input_Event_Kind_Mouse_Move
  , Input_Event_Kind_Mouse_Wheel
  , Input_Event_Kind_Quit
} 
Input_Event_Kind;

typedef struct Input_Event
{
  Input_Event_Kind kind;

  Key key;
  U32 key_state;
  S32 wheel_delta;

  S32 window_x;
  S32 window_y;

  S32 mouse_x;
  S32 mouse_y;

  S32 mouse_delta_x;
  S32 mouse_delta_y;
} 
Input_Event;

typedef struct Input_Event_View
{
  Input_Event* data;
  S32 len;
} 
Input_Event_View;

// @Note: Input state

Arena*      input_events_arena              = NULL;
S32         input_events_len                = 0;
Key_State   input_key_states[Key_Count]     = {ZeroStruct};
B32         input_key_down_table[Key_Count] = {ZeroStruct};
Cursor_Mode input_cursor_mode               = Cursor_Mode_Default;

void
input_poll_events();

void
input_set_cursor_mode(Cursor_Mode mode);

Input_Event_View 
input_events_this_frame();

B32 
input_key_down(Key key);

// ============================================
// @: OpenGL Functions.
// ============================================

#ifdef TGF_OPENGL

// Utility function types.
typedef void (*GLDebugProc)(U32Enum source, U32Enum type, U32 id, U32Enum severity, S32Size length, const char* message, const void* userParam);

// X Macro declaration for defining the OpenGL function prototypes.
#define ForOpenGLFunctions(Do)                                                        \
  Do( glDebugMessageCallback , void , (GLDebugProc callback, const void* userParam) ) \
  Do( glCreateShader         , U32  , (U32Enum source                             ) ) \

// Use X macro to generate the function pointer declarations.
#define DoFunctionDeclarations(name, ret, params) \
  extern ret (*name) params;                      \

ForOpenGLFunctions(DoFunctionDeclarations)

#undef DoFunctionDeclarations 

#endif

// ============================================
// @: OpenGL Context.
// ============================================

#ifdef TGF_OPENGL

B8
gl_context_create(Window* window);

#endif

#endif // TINY_GAME_FRAMEWORK_H

// ============================================================================================================================================================================================================================

#ifdef TGF_IMPL

// ============================================
// @i: Log and Assert/Check.
// ============================================

#ifdef TGF_DEBUG

void 
internal_log(const char* fmt, ...) 
{
  va_list args;
  va_start(args, fmt);
  vprintf(fmt, args);
  va_end(args);
  printf("\n");
}

void 
internal_check(B8 expr, const char* fmt, ...) 
{
  if (expr == false) 
  {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
    StopAtThisLine();
  }
}

B8 
internal_ensure(B8 expr, const char* fmt, ...) 
{
  if (expr == false) 
  {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");
    StopAtThisLine();
  }
  return expr != 0;
}

#endif // TGF_DEBUG

// ============================================
// @i: OS.
// ============================================

#ifdef TGF_WINDOWS

OS_System_Info* 
os_get_system_info()
{
  if (system_info.page_size == 0)
  {  
    SYSTEM_INFO sysinfo = {ZeroStruct};
    GetSystemInfo(&sysinfo);
    // @Pending: This we don't need this for now.
    // system_info.logical_processor_count = (U64)sysinfo.dwNumberOfProcessors;
    system_info.page_size               = sysinfo.dwPageSize;
    system_info.large_page_size         = GetLargePageMinimum();
    system_info.allocation_granularity  = sysinfo.dwAllocationGranularity;
  }    
  Check(system_info.page_size != 0);
  return &system_info;
}

void* 
os_reserve_large(U64 size)
{
  // rjf: we commit on reserve because windows.
  // @Note: I trust.
  return VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT|MEM_LARGE_PAGES, PAGE_READWRITE);
}

B32 
os_commit_large(void *ptr, U64 size) 
{
  UnusedVar(ptr);
  UnusedVar(size);
  return 1; 
}

void* 
os_reserve(U64 size)
{
  UnusedVar(size);
  return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
}

B32 
os_commit(void *ptr, U64 size)
{
  return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0;
}

void 
os_decommit(void *ptr, U64 size)
{
  VirtualFree(ptr, size, MEM_DECOMMIT);
}

void 
os_release(void *ptr, U64 size)
{
  // NOTE(rjf): size not used - not necessary on Windows, but necessary for other OSes.
  UnusedVar(size);
  VirtualFree(ptr, 0, MEM_RELEASE);
}

#endif

// ============================================
// @i: Arena.
// ============================================

Arena* 
arena_alloc(Arena_Params* params)
{
  // rjf: round up reserve/commit sizes
  U64 reserve_size = params->reserve_size;
  U64 commit_size = params->commit_size;
  if(params->flags & ArenaFlag_LargePages)
  {
    reserve_size = AlignPow2(reserve_size, os_get_system_info()->large_page_size);
    commit_size  = AlignPow2(commit_size,  os_get_system_info()->large_page_size);
  }
  else
  {
    reserve_size = AlignPow2(reserve_size, os_get_system_info()->page_size);
    commit_size  = AlignPow2(commit_size,  os_get_system_info()->page_size);
  }
  
  // rjf: reserve/commit initial block
  void *base = params->optional_backing_buffer;
  if(base == 0)
  {
    if(params->flags & ArenaFlag_LargePages)
    {
      base = os_reserve_large(reserve_size);
      os_commit_large(base, commit_size);
    }
    else
    {
      base = os_reserve(reserve_size);
      os_commit(base, commit_size);
    }
    
    // @Pending: Keep track of allocations like raddebugger does.
    //raddbg_annotate_vaddr_range(base, reserve_size, "arena %s:%i", params->allocation_site_file, params->allocation_site_line);
  }
  
  CheckMsg(base != 0, "Unexpected memory allocation failure.");
  
  // rjf: extract arena header & fill
  Arena *arena = (Arena *)base;
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

Arena* arena_alloc_default()
{
  Arena_Params params = Arena_Params_Default;
  return arena_alloc(&params);
}

void arena_release(Arena *arena)
{
  for(Arena *n = arena->current, *prev = 0; n != 0; n = prev)
  {
    prev = n->prev;
    os_release(n, n->res);
  }
}

// @Note: arena push/pop core functions

void* 
arena_push(Arena *arena, U64 size, U64 align, B32 zero)
{
  Arena *current = arena->current;
  U64 pos_pre = AlignPow2(current->pos, align);
  U64 pos_pst = pos_pre + size;
  
  // rjf: chain, if needed
  if(current->res < pos_pst && !(arena->flags & ArenaFlag_NoChain))
  {
    Arena *new_block = 0;
    
#if ARENA_FREE_LIST
    {
      Arena *prev_block;
      for(new_block = arena->free_last, prev_block = 0; new_block != 0; prev_block = new_block, new_block = new_block->prev)
      {
        if(new_block->res >= AlignPow2(new_block->pos, align) + size)
        {
          if(prev_block)
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
    
    if(new_block == 0)
    {
      U64 res_size = current->res_size;
      U64 cmt_size = current->cmt_size;
      if(size + ARENA_HEADER_SIZE > res_size)
      {
        res_size = AlignPow2(size + ARENA_HEADER_SIZE, align);
        cmt_size = AlignPow2(size + ARENA_HEADER_SIZE, align);
      }
      Arena_Params params = Arena_Params_Default;
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
    pos_pre = AlignPow2(current->pos, align);
    pos_pst = pos_pre + size;
  }
  
  // rjf: compute the size we need to zero
  U64 size_to_zero = 0;
  if(zero)
  {
    size_to_zero = Min(current->cmt, pos_pst) - pos_pre;
  }
  
  // rjf: commit new pages, if needed
  if(current->cmt < pos_pst)
  {
    U64 cmt_pst_aligned = pos_pst + current->cmt_size-1;
    cmt_pst_aligned -= cmt_pst_aligned%current->cmt_size;
    U64 cmt_pst_clamped = Min(cmt_pst_aligned, current->res);
    U64 cmt_size = cmt_pst_clamped - current->cmt;
    U8 *cmt_ptr = (U8 *)current + current->cmt;
    if(current->flags & ArenaFlag_LargePages)
    {
      os_commit_large(cmt_ptr, cmt_size);
    }
    else
    {
      os_commit(cmt_ptr, cmt_size);
    }
    current->cmt = cmt_pst_clamped;
  }
  
  // rjf: push onto current block
  void *result = 0;
  if(current->cmt >= pos_pst)
  {
    result = (U8 *)current+pos_pre;
    current->pos = pos_pst;

    // @Pending: Use adress sanitizer like raddebugger does.
    //AsanUnpoisonMemoryRegion(result, size);
    if(size_to_zero != 0)
    {
      memset(result, 0, size_to_zero);
    }
  }
  
  CheckMsg(result != 0, "Unexpected memory allocation failure.");
  return result;
}

U64 
arena_pos(Arena *arena)
{
  Arena *current = arena->current;
  U64 pos = current->base_pos + current->pos;
  return pos;
}

void 
arena_pop_to(Arena *arena, U64 pos)
{
  U64 big_pos = Max(ARENA_HEADER_SIZE, pos);
  Arena *current = arena->current;
  
#if ARENA_FREE_LIST
  for(Arena* prev = NULL; current->base_pos >= big_pos; current = prev)
  {
    prev = current->prev;
    current->pos = ARENA_HEADER_SIZE;
    current->prev = arena->free_last, arena->free_last = current;

    // @Pending: Use adress sanitizer like raddebugger does.
    //AsanPoisonMemoryRegion((U8*)current + ARENA_HEADER_SIZE, current->res - ARENA_HEADER_SIZE);
  }
#else
  for(Arena* prev = 0; current->base_pos >= big_pos; current = prev)
  {
    prev = current->prev;
    os_release(current, current->res);
  }
#endif
  arena->current = current;
  U64 new_pos = big_pos - current->base_pos;
  Check(new_pos <= current->pos);

  // @Pending: Use adress sanitizer like raddebugger does.
  // AsanPoisonMemoryRegion((U8*)current + new_pos, (current->pos - new_pos));
  current->pos = new_pos;
}

void*
arena_begin_raw(Arena* arena, U64 align)
{
  U64 raw_start = (U64) ((U8*) arena) + ARENA_HEADER_SIZE;
  U64 start = AlignPow2(raw_start, align);
  return (void*) start;
}

// @Note: arena push/pop helpers

void 
arena_clear(Arena *arena)
{
  arena_pop_to(arena, 0);
}

void 
arena_pop(Arena *arena, U64 amt)
{
  U64 pos_old = arena_pos(arena);
  U64 pos_new = pos_old;
  if(amt < pos_old)
  {
    pos_new = pos_old - amt;
  }
  arena_pop_to(arena, pos_new);
}

// @Note: temporary arena scopes

Temp 
temp_begin(Arena *arena)
{
  U64 pos = arena_pos(arena);
  Temp temp = {arena, pos};
  return temp;
}

void 
temp_end(Temp temp)
{
  arena_pop_to(temp.arena, temp.pos);
}

// ============================================
// @i: Input
// ============================================

#ifdef TGF_WINDOWS

Key 
internal_input_key_from_vk_win32(WPARAM wParam)
{
  // *** ASCII KEYS ***
  if (wParam >= '0' && wParam <= '9')
  {
    return (Key) wParam;
  }

  if (wParam >= 'A' && wParam <= 'Z')
  {
    return (Key) wParam;
  }

  // *** NON ASCII ***
  switch (wParam)
  {
  case VK_BACK:       return Key_Backspace;
  case VK_TAB:        return Key_Tab;
  case VK_RETURN:     return Key_Enter;
  case VK_ESCAPE:     return Key_Escape;
  case VK_SPACE:      return Key_Space;
  case VK_DELETE:     return Key_Delete;
  case VK_UP:         return Key_Arrow_Up;
  case VK_DOWN:       return Key_Arrow_Down;
  case VK_LEFT:       return Key_Arrow_Left;
  case VK_RIGHT:      return Key_Arrow_Right;
  case VK_PRIOR:      return Key_Page_Up;
  case VK_NEXT:       return Key_Page_Down;
  case VK_HOME:       return Key_Home;
  case VK_END:        return Key_End;
  case VK_INSERT:     return Key_Insert;
  case VK_PAUSE:      return Key_Pause;
  case VK_SCROLL:     return Key_Scroll_Lock;
  case VK_MENU:       return Key_Alt;
  case VK_CONTROL:    return Key_Ctrl;
  case VK_SHIFT:      return Key_Shift;
  case VK_LWIN:
  case VK_RWIN:       return Key_CMD;
  case VK_SNAPSHOT:   return Key_Print_Screen;

  // @Note: Apparently this doesn't work.
  //case VK_LBUTTON:    return Key_Mouse_Left;
  //case VK_MBUTTON:    return Key_Mouse_Middle;
  //case VK_RBUTTON:    return Key_Mouse_Right;
  }
  
  // *** FUNCTION KEYS ***
  if (wParam >= VK_F1 && wParam <= VK_F24)
  {
    return (Key) (Key_F1 + (wParam - VK_F1));
  }

  return Key_Unknown;
}

void 
internal_input_update_cursor_state_win32() 
{
  if (input_cursor_mode == Cursor_Mode_Default)
  return;

  HWND fg = GetForegroundWindow();
  
  RECT rect;
  GetClientRect(fg, &rect);
  
  POINT ul = { rect.left,  rect.top };
  POINT lr = { rect.right, rect.bottom };

  ClientToScreen(fg, & ul);
  ClientToScreen(fg, &lr);

  rect.left = ul.x;
  rect.top = ul.y;
  rect.right = lr.x;
  rect.bottom = lr.y;
  
  ClipCursor(&rect);
  
  if (input_cursor_mode == Cursor_Mode_Hidden)
  {
    // @Note: We center the cursor.
    SetCursorPos
    (
        (rect.left + rect.right) / 2,
        (rect.top + rect.bottom) / 2
    );
  }
}

CALLBACK LRESULT 
internal_input_process_events_win32(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
        
        Input_Event* ev = arena_push_element(input_events_arena, Input_Event);
        input_events_len += 1;
        ev->kind = Input_Event_Kind_Mouse_Move;
        
        ev->mouse_delta_x = dx;
        ev->mouse_delta_y = dy;

        ev->mouse_x = (S32)(short)LOWORD(lParam);
        ev->mouse_y = (S32)(short)HIWORD(lParam);
      }
      break;
    }
    case WM_SIZE: 
    {
      Input_Event* ev = arena_push_element(input_events_arena, Input_Event);
      input_events_len += 1;
      ev->kind = Input_Event_Kind_Window;
      ev->window_x = LOWORD(lParam);
      ev->window_y = HIWORD(lParam);
      internal_input_update_cursor_state_win32();
    }
    break;

    case WM_MOVE: 
    {
        internal_input_update_cursor_state_win32();
    }
    break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: 
    {
      Input_Event* ev = arena_push_element(input_events_arena, Input_Event);
      input_events_len += 1;
      ev->kind = Input_Event_Kind_Key;
      ev->key = internal_input_key_from_vk_win32(wParam);

      ev->key_state = Key_State_Down;

      B32 is_repeat = (lParam & (1 << 30)) != 0;

      if (!is_repeat)
      {
          ev->key_state |= Key_State_Start;
      }

      input_key_down_table[ev->key] = 1;
    }
    break;

    case WM_KEYUP:
    case WM_SYSKEYUP: 
    {
      Input_Event* ev = arena_push_element(input_events_arena, Input_Event);
      input_events_len += 1;
      ev->kind = Input_Event_Kind_Key;
      ev->key = internal_input_key_from_vk_win32(wParam);

      ev->key_state = Key_State_End;

      input_key_down_table[ev->key] = 1;
    }
    break;

    case WM_MOUSEMOVE: 
    {
        internal_input_update_cursor_state_win32();
    }
    break;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP: 
    {
      Input_Event* ev = arena_push_element(input_events_arena, Input_Event);
      input_events_len += 1;
      ev->kind = Input_Event_Kind_Key;
      ev->key = Key_Mouse_Left;
      
      B32 down = msg == WM_LBUTTONDOWN;
      ev->key_state = down ? Key_State_Down : Key_State_End;
      input_key_down_table[ev->key] = down;
    }
    break;

    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP: 
    {
      Input_Event* ev = arena_push_element(input_events_arena, Input_Event);
      input_events_len += 1;
      ev->kind = Input_Event_Kind_Key;
      ev->key = Key_Mouse_Right;
      
      B32 down = msg == WM_RBUTTONDOWN;
      ev->key_state = down ? Key_State_Down : Key_State_End;
      input_key_down_table[ev->key] = down;
    }
    break;

    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP: 
    {
      Input_Event* ev = arena_push_element(input_events_arena, Input_Event);
      input_events_len += 1;
      ev->kind = Input_Event_Kind_Key;
      ev->key = Key_Mouse_Middle;
      
      B32 down = msg == WM_MBUTTONDOWN;
      ev->key_state = down ? Key_State_Down : Key_State_End;
      input_key_down_table[ev->key] = down;
    }
    break;

    case WM_MOUSEWHEEL: 
    {
      Input_Event* ev = arena_push_element(input_events_arena, Input_Event);
      input_events_len += 1;
      ev->kind = Input_Event_Kind_Mouse_Wheel;
      ev->wheel_delta = (S32)GET_WHEEL_DELTA_WPARAM(wParam);
    }
    break;

    case WM_CLOSE:
    case WM_QUIT: 
    {
      Input_Event* ev = arena_push_element(input_events_arena, Input_Event);
      input_events_len += 1;
      ev->kind = Input_Event_Kind_Quit;
    }
    break;
    case WM_KILLFOCUS:
      memset(input_key_down_table, 0, sizeof(input_key_down_table));
    break;
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

void
input_poll_events()
{
  arena_clear(input_events_arena);
  input_events_len = 0;

  MSG msg = {};
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

void
input_set_cursor_mode(Cursor_Mode mode)
{
  if (mode == input_cursor_mode)
  {
    return;
  }

  if (mode == Cursor_Mode_Default)
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
  input_cursor_mode = mode;
}

Input_Event_View 
input_events_this_frame()
{
  Input_Event_View view;
  view.data = arena_begin(input_events_arena, Input_Event);
  view.len  = input_events_len;
  return view;
}

B32 
input_key_down(Key key)
{
  return input_key_down_table[key];
}

// ============================================
// @i: Window.
// ============================================

#define MAX_WINDOWS 30

#define WindowClassNameWin32 L"Window Class"

#define WindowStyleWindowedWin32    WS_OVERLAPPEDWINDOW
#define WindowStyleFullscreenWin32  WS_VISIBLE | WS_POPUP
#define WindowStyleSecondaryWin32   WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME

#define WindowStyleDefaultWin32 WindowStyleWindowedWin32

typedef struct Window_Array Window_Array;
struct Window_Array
{
  Window data[MAX_WINDOWS];
  U32 len;
};

Window_Array window_array;
B8 window_class_registered_win32 = 0;
B8 input_device_registered_win32 = 0;
HWND main_window_win32 = NULL;

B8
internal_window_class_create_once_win32(Window_Params* params)
{
  if (window_class_registered_win32 == 0)
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

    WNDCLASSEXW wc = {ZeroStruct};
    wc.lpfnWndProc = &internal_input_process_events_win32;

    // @Note Use this to run the default event processing.
    //wc.lpfnWndProc = &DefWindowProc;
    
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);     // Default cursor.
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);   // Default icon.
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION); // Small icon.
    wc.hInstance = hInstance;
    wc.lpszClassName = WindowClassNameWin32;
    // @Note: CreateSolidBrush changes the background color, I guess.
    wc.hbrBackground = CreateSolidBrush(RGB(params->bg_color.x * 255, params->bg_color.y * 255, params->bg_color.z * 255));
    window_class_registered_win32 = RegisterClassExW(&wc) != 0;

    if (!EnsureMsg(window_class_registered_win32 != 0, "Error! Couldn't register the Win32 window class."))
    {
        return NULL;
    }

    Log("Win32 Window class registered!");
    return true;
  }
  return false;
}

HWND 
internal_window_create_win32(Window_Params* params)
{
  // Calculate the RECT -----------
  RECT rect;
  rect.top = 0;
  rect.left = 0;
  rect.right = params->width;
  rect.bottom = params->height;

  AdjustWindowRect(&rect, WindowStyleDefaultWin32, 0);

  S32 client_width = rect.right - rect.left;
  S32 client_height = rect.bottom - rect.top;

  // Convert title to wchar_t* -----------
  #define MaxTitleLen 256u
  wchar_t title_buffer[MaxTitleLen + 1];
  {
    U64 title_len = strlen(params->title);
    if (!EnsureMsg(title_len <= MaxTitleLen, "Error! The title len is %llu chars. Max is %llu.", title_len, MaxTitleLen))
    {
      return NULL;
    }

    S32 len = MultiByteToWideChar
    (
      CP_UTF8,
      0,                 // flags.
      params->title,     // input.
      -1,                // (-1 = null-terminated).
      title_buffer,      // output buffer.
      MaxTitleLen + 1    // buffer len.
    );

    if (len == 0) 
    {
      DWORD err = GetLastError();
      Log("Error! %lu\n", err);
      StopAtThisLine();
      return NULL;
    }
  }
  // --------------------------------------

  // @Note: Create the window.
  
  HWND hwnd = CreateWindowExW
  (
    0,                          // Optional window styles.
    WindowClassNameWin32,       // Window class.
    title_buffer,               // Window title
    WindowStyleDefaultWin32,    // Window style.
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

B8
internal_window_input_device_create_once_win32(HWND hwnd)
{
  if (input_device_registered_win32 == 0)
  {
    // @Note: Register the input device.
    RAWINPUTDEVICE rid;
    rid.usUsagePage = 0x01; // Generic Desktop
    rid.usUsage = 0x02;     // Mouse
    rid.dwFlags = RIDEV_INPUTSINK;
    rid.hwndTarget = hwnd;
    input_device_registered_win32 = (B8) RegisterRawInputDevices(&rid, 1, sizeof(rid));
    
    return true;
  }
  return false;
}

Window* 
window_create_default()
{
  Window_Params params = Window_Params_Default;
  return window_create(&params);
}

Window* 
window_create(Window_Params* params)
{
  if (!EnsureMsg(window_array.len < MAX_WINDOWS, "Window limit reached!"))
  {
    return NULL;
  }

  // Get the window slot.
  Window* window = NULL;

  for EachIndex(it, MAX_WINDOWS)
  {
    Window* curr = &window_array.data[it];
    if (curr->handle == NULL)
    {
      window = curr;
      break;
    }
  }
  
  if (!EnsureMsg(window != NULL, "Unexpected window alloc error!"))
  {
    return NULL;
  }

  // Create the window class once.
  B8 created = internal_window_class_create_once_win32(params);

  // If the window class is created we zero all the window array memory just in case.
  if (created)
  {
    memset(&window_array, 0, sizeof(Window_Array));
  }

  // Create Windows window.
  HWND hwnd = internal_window_create_win32(params);

  if (!EnsureMsg(hwnd != 0, "Error! Couldn't create the Win32 window."))
  {
    return NULL;
  }

  Log("Win32 Window created!");
  
  // Create the window input device once.
  created = internal_window_input_device_create_once_win32(hwnd);
  
  // If the input device is created setup the input event arena .
  if (created)
  {
    Check(input_events_arena == NULL);
    Arena_Params params = Arena_Params_Default;
    params.flags &= ArenaFlag_NoChain;
    input_events_arena = arena_alloc_default();
  }

  // Fill the window slot.
  window->handle = hwnd;
  window->index  = window_array.len;
  window_array.len += 1;

  // @Review: Create the opengl context.
  B8 success = gl_context_create(window);
  UnusedVar(success);
  
  // Show the window.
  SetForegroundWindow(hwnd);
  UpdateWindow(hwnd);
  ShowWindow(hwnd, SW_SHOW);

  return window;
}

void 
window_destroy(Window* window)
{
  if (!EnsureMsg((window_array.len > 0 && window->handle != NULL), "Not a valid window!"))
  {
    return;
  }

  B8 found = false;
  for EachIndex(it, MAX_WINDOWS)
  {
    Window* curr = &window_array.data[it];
    if (curr == window)
    {
        found = true;
        break;
    }
  }

  if (!EnsureMsg(found, "Window not found!"))
  {
    return;
  }

  HWND hwnd = (HWND) window->handle;

  DestroyWindow(hwnd);
  Log("Win32 Window destroyed!");

  if (hwnd == main_window_win32)
  {
    if (input_device_registered_win32)
    {
      RAWINPUTDEVICE rid;
      rid.usUsagePage = 0x01; // Generic Desktop
      rid.usUsage = 0x02;     // Mouse
      rid.dwFlags = RIDEV_REMOVE;
      rid.hwndTarget = hwnd;
      RegisterRawInputDevices(&rid, 1, sizeof(rid));
      input_device_registered_win32 = 0;
      Check(input_events_arena != NULL);
      arena_clear(input_events_arena);
    }
    // @Pending: Free the window class and set window_class_registered_win32 to false.
    main_window_win32 = NULL;
  }

  memset(window, 0, sizeof(Window));
  window_array.len -= 1;
}

#endif // TGF_WINDOWS

// ============================================
// @i: OpenGL Functions.
// ============================================

#ifdef TGF_OPENGL

// Use X macro to generate the function pointers definitions.
#define DoFunctionPointerDefinitions(name, ret, params)    \
  ret (*name) params = NULL;                               \

ForOpenGLFunctions(DoFunctionPointerDefinitions)

#undef DoFunctionPointerDefinitions 

#endif // TGF_OPENGL

// ============================================
// @i: OpenGL Context (Windows).
// ============================================

#if defined(TGF_OPENGL) && defined(TGF_WINDOWS)

typedef struct GL_Win32_Context GL_Win32_Context;
struct GL_Win32_Context 
{ 
  HGLRC handle = NULL;
  HDC   device = NULL;
};

// @Note: 
// This array it is just going to mimic the window array.
// So creating a context just means occuping the same slot
// as the window.


GL_Win32_Context gl_context_array_win32[MAX_WINDOWS] = {ZeroStruct};

// @Note: Windows sucks.

typedef BOOL  (WINAPI * PFNWGLCHOOSEPIXELFORMATARBPROC)    (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL  (WINAPI * PFNWGLSWAPINTERVALEXTPROC)         (int interval);

PFNWGLCHOOSEPIXELFORMATARBPROC    wglChoosePixelFormatARB    = NULL;
PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
PFNWGLSWAPINTERVALEXTPROC         wglSwapIntervalEXT         = NULL;

void*
internal_gl_resolve_function(const char* name)
{
	void* p = (void*) wglGetProcAddress(name);

	// @Note: Illegal values returned by wglGetProcAddress.
	if (p == nullptr     ||
		p == (void*) 0x1 ||
		p == (void*) 0x2 ||
		p == (void*) 0x3 ||
		p == (void*) -1)
	{
		static HMODULE ogl = LoadLibrary(L"opengl32.dll");
		p = (void*) GetProcAddress(ogl, name);
	}

	return p;
}

void
internal_gl_resolve_functions_win32()
{
   // Use X macro to generate the function pointer assignments.
   #define DoFunctionResolve(name, ret, params) \
     name = ( ret(*) params ) internal_gl_resolve_function(#name);
 
   ForOpenGLFunctions(DoFunctionResolve)

   #undef DoFunctionResolve 
}

B8
internal_gl_resolve_wgl_functions_once_win32()
{
  // @Note: I guess we need to resolve this functions just once.
  if (wglChoosePixelFormatARB && wglCreateContextAttribsARB && wglSwapIntervalEXT)
  {
    return false;   
  }

  // @Pending: Do we need to reset this pointers if the class is destroyed?
  if (!EnsureMsg(window_class_registered_win32, "We need at least one window created at this point."))
  {
    return false;   
  }
	// @Note: Old OpenGL context. Needed to load wgl extensions.
	
	// Dummy window creation.

  Window_Params dummy_params = MakeStruct(Window_Params,
    .width    = 0,
    .height   = 0,
    .title    = "Dummy",
    .bg_color = Vec4_White,
  );

  HWND window = internal_window_create_win32(&dummy_params);
  Check(window != NULL);

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
	
	S32 pf = ChoosePixelFormat(dc, &ds);
	SetPixelFormat(dc, pf, &ds);
	
	// @Note: HGLRC stands for handle to OpenGL rendering context. 
	HGLRC context = wglCreateContext(dc);
	wglMakeCurrent(dc, context);
	
	// @Note: Resolve wgl extension functions.
	wglChoosePixelFormatARB    = (PFNWGLCHOOSEPIXELFORMATARBPROC   ) (void*) (wglGetProcAddress("wglChoosePixelFormatARB"));
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC) (void*) (wglGetProcAddress("wglCreateContextAttribsARB"));
	wglSwapIntervalEXT         = (PFNWGLSWAPINTERVALEXTPROC        ) (void*) (wglGetProcAddress("wglSwapIntervalEXT"));
	
	// @Note: Destroy the old context and the dummy window.
	wglMakeCurrent(dc, 0);
	wglDeleteContext(context);

  return true;
}

B8
gl_context_create(Window* window)
{
  if (!EnsureMsg(window->handle != NULL, "The window must to be valid!"))
  {
    return false;
  }
  
  GL_Win32_Context* context = &gl_context_array_win32[window->index];

  if (!EnsureMsg(context->handle == NULL, "This window has already an associated context!"))
  {
    return false;
  }

  B8 result = internal_gl_resolve_wgl_functions_once_win32();
  UnusedVar(result);

  S32 WGL_DRAW_TO_WINDOW_ARB     = 0x2001;
  S32 WGL_SUPPORT_OPENGL_ARB     = 0x2010;
  S32 WGL_DOUBLE_BUFFER_ARB      = 0x2011;
  S32 WGL_ACCELERATION_ARB       = 0x2003;
  S32 WGL_FULL_ACCELERATION_ARB  = 0x2027;
  S32 WGL_PIXEL_TYPE_ARB         = 0x2013;
  S32 WGL_COLOR_BITS_ARB         = 0x2014;
  S32 WGL_DEPTH_BITS_ARB         = 0x2022;
  S32 WGL_STENCIL_BITS_ARB       = 0x2023;
  S32 WGL_TYPE_RGBA_ARB          = 0x202B;

	// @Note: Modern OpenGL initialization.
	S32 pixel_forattribs[] = 
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
	S32 pixel_format = 0;
	U32 num_formats  = 0u;

	wglChoosePixelFormatARB(dc, pixel_forattribs, 0, 1, &pixel_format, &num_formats);

	PIXELFORMATDESCRIPTOR pfd;
	DescribePixelFormat(dc, pixel_format, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	SetPixelFormat(dc, pixel_format, &pfd);

  S32 WGL_CONTEXT_MAJOR_VERSION_ARB    = 0x2091;
  S32 WGL_CONTEXT_MINOR_VERSION_ARB    = 0x2092;
  S32 WGL_CONTEXT_PROFILE_MASK_ARB     = 0x9126;
  S32 WGL_CONTEXT_CORE_PROFILE_BIT_ARB = 0x00000001;
  S32 WGL_CONTEXT_FLAGS_ARB            = 0x2094;
  S32 WGL_CONTEXT_DEBUG_BIT_ARB        = 0x00000001;

	S32 context_attribs[] = 
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
	
	internal_gl_resolve_functions_win32();

  Log("Win32 OpenGL Context created!");
  return true;
}

#endif // TGF_OPENGL && TGF_WINDOWS

#endif // TGF_IMPL