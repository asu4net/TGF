#ifndef ENGINE_H_KEY
#define ENGINE_H_KEY

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

extern b8 g_key_down_table[KEY_COUNT];

inline b8 is_key_down(u32 key)
{
  return g_key_down_table[key];
}

inline void set_key_down(u32 key, b8 down)
{
  g_key_down_table[key] = down;
}

inline void release_all_keys()
{
  memset(g_key_down_table, 0, sizeof(g_key_down_table));
}

#endif // ENGINE_H_KEY 

#ifdef ENGINE_IMPL_KEY

b8 g_key_down_table[KEY_COUNT] = nil;

#endif
