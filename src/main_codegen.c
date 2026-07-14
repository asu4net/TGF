#define ENGINE_IMPL_CORE
#define ENGINE_IMPL_MATH
#define ENGINE_IMPL_SYS
#define ENGINE_IMPL_ARENA

#include "base_core.h"
#include "base_math.h"
#include "base_sys.h"
#include "base_arena.h"

#include "template_hmap.h"

const char* g_code_gen_h_header = 
"#ifndef ENGINE_H_GENERATED                                                         \n"
"#define ENGINE_H_GENERATED                                                         \n"
"                                                                                   \n";

const char* g_code_gen_h_footer = 
"#endif // ENGINE_H_GENERATED                                                       \n"
"                                                                                   \n";

const char* g_code_gen_c_header = 
"#ifdef ENGINE_IMPL_GENERATED                                                       \n"
"                                                                                   \n";

const char* g_code_gen_c_footer = 
"#endif // ENGINE_IMPL_GENERATED                                                    \n"
"                                                                                   \n";

// @Note: Dead simple token replace function.
static void replace_token(const char* src, const char* token, const char* replace, struct arena* arena)
{
  u64 token_len = strlen(token);
  u64 token_index = 0;
  for each_index(i, (s32) strlen(src))
  {
    char c = src[i];
    // Paste the char.
    arena_push_char(arena, c);

    if (c == token[token_index])
    {
      token_index += 1;
      if (token_index == token_len)
      {
        // Token recognized, paste the replace.
        arena_pop(arena, token_len);
        arena_push_cstring(arena, replace);
        token_index = 0;
      }
    }
    else
    {
      token_index = 0;
    }
  }
}

// @Note: This is ugly, but gets the job done.
s32 main(s32 argc, char** argv) 
{
  UNUSED(argc); UNUSED(argv);

  struct arena_params params = DEFAULT_ARENA_PARAMS;
  struct arena* arena = arena_alloc(&params);
  {
    FILE* file = NULL;
    fopen_s(&file, "src/generated.h", "w");

    arena_push_cstring(arena, g_code_gen_h_header);
    replace_token(g_template_h_hmap, "$T", "gl_vertex_buffer", arena);
    arena_push_cstring(arena, g_code_gen_h_footer);

    arena_push_cstring(arena, g_code_gen_c_header);
    replace_token(g_template_c_hmap, "$T", "gl_vertex_buffer", arena);
    arena_push_cstring(arena, g_code_gen_c_footer);

    arena_push_char(arena, '\0');

    fputs(arena_first(arena, char), file);
    fclose(file);
  }
}
