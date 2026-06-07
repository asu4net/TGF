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

#include "engine_base.h"
#include "engine_math.h"
#include "engine_sys.h"
#include "engine_arena.h"
#include ".codegen_generated.h"

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

#ifdef ENGINE_API_OPENGL
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

#include "engine_state.h"

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

#include "engine_base.h"
#include "engine_math.h"
#include "engine_sys.h"
#include "engine_arena.h"

#include ".codegen_generated.c"

#include "engine_state.h"


// ============================================
// @i: Input
// ============================================

static enum cursor_mode g_cursor_mode = CURSOR_MODE_DEFAULT;
static struct arena* g_input_events_arena = NULL;
static s32 g_input_events_len = 0;
static b8 g_key_down_table[KEY_COUNT] = nil;

#ifdef ENGINE_OS_WINDOWS

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
#if ENGINE_DEBUG
      DWORD err = GetLastError();
      trace("Error! %lu\n", err);
      STOP_AT_THIS_LINE();
#endif
      return NULL;
    }
  }
  // --------------------------------------

  // @Note: Create the window.
  
  HWND hwnd = CreateWindowExW
  (
    0,                          // Optional window styles.
    WIN32_CLASS_NAME,           // struct window class.
    title_buffer,               // struct window title
    WIN32_STYLE_DEFAULT,        // struct window style.
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

#ifdef ENGINE_API_OPENGL
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

#ifdef ENGINE_API_OPENGL

void swap_buffers(struct window*  window, b8 vsync)
{
  wglSwapIntervalEXT(vsync ? 1 : 0);
  check(window->handle != NULL);
  struct wgl_context* context = &g_wgl_context_array[window->index];
  check(context->device != NULL);
  SwapBuffers(context->device);
}

#endif

#endif // ENGINE_OS_WINDOWS

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