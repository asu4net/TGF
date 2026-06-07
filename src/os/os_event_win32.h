#ifndef ENGINE_H_EVENT_WIN32
#define ENGINE_H_EVENT_WIN32

#ifdef ENGINE_OS_WINDOWS
CALLBACK LRESULT process_events_win32(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif // ENGINE_OS_WINDOWS

void poll_events_win32();

#endif // ENGINE_H_EVENT_WIN32 

#ifdef ENGINE_IMPL_EVENT_WIN32

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

CALLBACK LRESULT process_events_win32(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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

      set_key_down(ev->key, true);
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

      set_key_down(ev->key, false);
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
      set_key_down(ev->key, down);
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
      set_key_down(ev->key, down);
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
      set_key_down(ev->key, down);
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
      release_all_keys();
    break;
  }

  return DefWindowProc(hWnd, msg, wParam, lParam);
}

void poll_events_win32()
{
  clear_events();

  MSG msg = {};
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

#endif // ENGINE_IMPL_EVENT_WIN32
