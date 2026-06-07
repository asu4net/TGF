// ============================================================================================
// @: Info.
// ============================================================================================
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
// ENGINE_IMPLEMENTATION Pastes the function implementations.
// ENGINE_DEBUG          Enables trace, check, ensure macros.
// ENGINE_API_OPENGL     Uses OpenGL.

#define ENGINE_API_OPENGL // @Note: For now we force the OpenGL implementation.

#ifndef ENGINE_H
#define ENGINE_H

// ============================================================================================
// @: BEGIN OF DECLARATIONS
// ============================================================================================

#include "core/base.h"
#include "core/math.h"
#include "core/sys.h"
#include "core/arena.h"
#include "../.codegen_generated.h"

#include "input/key.h"
#include "input/cursor.h"
#include "input/event.h"
#include "window/wndcreation.h"
#include "engine_state.h"

// ============================================
// @: OpenGL Functions.
// ============================================

#ifdef ENGINE_API_OPENGL

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

#ifdef ENGINE_API_OPENGL
b8 gl_context_create(struct window* window);
#endif

// ============================================
// @: OpenGL Context (Windows).
// ============================================

#if defined(ENGINE_API_OPENGL) && defined(ENGINE_OS_WINDOWS)

typedef BOOL (WINAPI *PFNWGLCHOOSEPIXELFORMATARBPROC) (HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int *attribList);
typedef BOOL (WINAPI *PFNWGLSWAPINTERVALEXTPROC) (int interval);

extern PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
extern PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

struct wgl_context
{ 
  HGLRC handle;
  HDC   device;
};

// @Note: 
// This array it is just going to mimic the window array.
// So creating a context just means occuping the same slot
// as the window.

extern struct wgl_context g_wgl_context_array[MAX_WINDOWS];

#endif // defined(ENGINE_API_OPENGL) && defined(ENGINE_OS_WINDOWS)

// ============================================
// @: Graphics.
// ============================================

struct vertex_buffer
{
  struct hmap_handle handle;
  u32 id;
};

void clear_screen(union vec4 color);

#endif // ENGINE_H
  
// ============================================================================================
// @: END OF DECLARATIONS
// ============================================================================================

// ============================================================================================
// @i: BEGIN OF IMPLEMENTATIONS
// ============================================================================================

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

#include "core/base.h"
#include "core/math.h"
#include "core/sys.h"
#include "core/arena.h"

#include "../.codegen_generated.c"

#include "input/key.h"
#include "input/cursor.h"
#include "input/event.h"
#include "window/wndcreation.h"
#include "engine_state.h"

// ============================================
// @i: OpenGL Functions.
// ============================================

#ifdef ENGINE_API_OPENGL

// Use X macro to generate the function pointers definitions.
#define DO_FUNCTION_POINTER_DEFINITIONS(_NAME, _RETURN, _PARAMS) _RETURN (*_NAME) _PARAMS = NULL;                               \

FOR_OPEN_GL_FUNCTIONS(DO_FUNCTION_POINTER_DEFINITIONS)

#undef DO_FUNCTION_POINTER_DEFINITIONS 

#endif // ENGINE_API_OPENGL

// ============================================
// @i: OpenGL Context (Windows).
// ============================================

#if defined(ENGINE_API_OPENGL) && defined(ENGINE_OS_WINDOWS)

struct wgl_context g_wgl_context_array[MAX_WINDOWS] = nil;

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

  HWND window = create_raw_window_win32(&dummy_params);
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
  
  struct wgl_context* context = &g_wgl_context_array[window->index];

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
#ifdef ENGINE_DEBUG 
  s32 WGL_CONTEXT_FLAGS_ARB            = 0x2094;
  s32 WGL_CONTEXT_DEBUG_BIT_ARB        = 0x00000001;
#endif

	s32 context_attribs[] = 
  {
		WGL_CONTEXT_MAJOR_VERSION_ARB  , 4,
		WGL_CONTEXT_MINOR_VERSION_ARB  , 6,
		WGL_CONTEXT_PROFILE_MASK_ARB   , WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		
#ifdef ENGINE_DEBUG 
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

#endif // ENGINE_API_OPENGL && ENGINE_OS_WINDOWS
 
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

// ============================================
// @i: Entry Point.
// ============================================


#endif // ENGINE_IMPLEMENTATION

// ============================================================================================
// @i: END OF IMPLEMENTATIONS
// ============================================================================================