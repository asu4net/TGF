#ifndef ENGINE_H_WINDOW_WIN32
#define ENGINE_H_WINDOW_WIN32

HWND create_raw_window_win32(struct window_params* params);

struct window* create_window_win32(struct window_params* params);
void destroy_window_win32(struct window*  window);

#endif // ENGINE_H_WINDOW_WIN32 

#ifdef ENGINE_IMPL_WINDOW_WIN32

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
    wc.lpfnWndProc = &process_events_win32;

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

HWND create_raw_window_win32(struct window_params* params)
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

static b8 create_win32_input_device_once(HWND hwnd)
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

struct window* create_window_win32(struct window_params* params)
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
  HWND hwnd = create_raw_window_win32(params);

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

  // Show the window.
  SetForegroundWindow(hwnd);
  UpdateWindow(hwnd);
  ShowWindow(hwnd, SW_SHOW);

  return window;
}

void destroy_window_win32(struct window* window)
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
      clear_events();
    }
    // @Pending: Free the window class and set g_win32_class_registered to false.
    g_win32_main_window = NULL;
  }

  memset(window, 0, sizeof(struct window));
  g_window_array.len -= 1;
}

#endif // ENGINE_IMPL_WINDOW_WIN32
