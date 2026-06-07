#define ENGINE_IMPLEMENTATION_BASE
#define ENGINE_IMPLEMENTATION_MATH
#define ENGINE_IMPLEMENTATION_SYS
#define ENGINE_IMPLEMENTATION_ARENA

#include "engine_base.h"
#include "engine_math.h"
#include "engine_sys.h"
#include "engine_arena.h"

#include "codegen_hmap.h"

const char* g_code_gen_h_header = 
"#ifndef CODEGEN_DECLARATION_GENERATED                                              \n"
"#define CODEGEN_DECLARATION_GENERATED                                              \n"
"                                                                                   \n";

const char* g_code_gen_h_footer = 
"#endif // CODEGEN_DECLARATION_GENERATED                                            \n"
"                                                                                   \n";

const char* g_code_gen_c_header = 
"#include \".codegen_generated.h\"                                                  \n"
"                                                                                   \n";

// @Note: Dead simple token replace function.
static void replace_token(const char* src, const char* token, const char* replace, struct arena* arena, b8 add_null_terminator)
{
  u64 replace_len = strlen(replace);
  u64 token_len = strlen(token);
  u64 token_index = 0;
  for each_index(i, (s32) strlen(src))
  {
    char c = src[i];
    // Paste the char.
    char* pc = (char*) arena_push(arena, 1, ALIGN_OF(char), false);
    *pc = c;

    if (c == token[token_index])
    {
      token_index += 1;
      if (token_index == token_len)
      {
        // Token recognized, paste the replace.
        arena_pop(arena, token_len);
        char* replace_begin = arena_push(arena, replace_len, ALIGN_OF(char), false);
        memcpy(replace_begin, replace, replace_len);
        token_index = 0;
      }
    }
    else
    {
      token_index = 0;
    }
  }

  if (add_null_terminator)
  {
    char* pc = (char*) arena_push(arena, 1, ALIGN_OF(char), false);
    *pc = '\0';
  }
}

// @Note: This is fucking ugly, but gets the job done.
s32 main(s32 argc, char** argv) 
{
  UNUSED(argc);
  UNUSED(argv);
  struct arena* arena = arena_alloc_default();
  {
    FILE* file = NULL;
    fopen_s(&file, ".codegen_generated.h", "w");
    u64 header_len = strlen(g_code_gen_h_header);
    char* header = arena_push(arena, header_len, ALIGN_OF(char), false);
    memcpy(header, g_code_gen_h_header, header_len);

    // @Note: Add here your declaration string.
    replace_token(g_template_h_hmap, "$T", "vertex_buffer", arena, false);

    u64 footer_len = strlen(g_code_gen_h_footer);
    char* footer = arena_push(arena, footer_len, ALIGN_OF(char), false);
    memcpy(footer, g_code_gen_h_footer, footer_len);

    char* terminator = (char*) arena_push(arena, 1, ALIGN_OF(char), false);
    *terminator = '\0';
    char* data = arena_first(arena, char);
    fputs(data, file);
    fclose(file);
  }
  arena_clear(arena);
  {
    FILE* file = NULL;
    fopen_s(&file, ".codegen_generated.c", "w");
    u64 header_len = strlen(g_code_gen_c_header);
    char* header = arena_push(arena, header_len, ALIGN_OF(char), false);
    memcpy(header, g_code_gen_c_header, header_len);

    // @Note: Add here your implementation string.
    replace_token(g_template_c_hmap, "$T", "vertex_buffer", arena, false);

    char* terminator = (char*) arena_push(arena, 1, ALIGN_OF(char), false);
    *terminator = '\0';
    char* data = arena_first(arena, char);
    fputs(data, file);
    fclose(file);
  }
  // We are on "comptime" so we don't need to release the arena.
}
