#ifndef ENGINE_H_BASE
#define ENGINE_H_BASE 

// Platform detection.
#ifdef _WIN64
#define ENGINE_OS_WINDOWS
#else
#error "Unsupported platform!"
#endif

// Language detection.
#if defined(__cplusplus)
#define ENGINE_LANG_CPP
#else
#define ENGINE_LANG_C
#endif

// Vendor headers.
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#ifdef ENGINE_OS_WINDOWS 
#define UNICODE
#define _UNICODE
#include <Windows.h>
#endif

#define ENGINE_API_OPENGL // @Note: For now we force the OpenGL implementation.

#ifdef ENGINE_API_OPENGL
#include "GL/GL.h"
#endif

// Numeric types.
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

// Cross-language true/false macros.
#ifdef ENGINE_LANG_C
#undef true
#undef false
#define true ((b8) 1)
#define false ((b8) 0)
#endif

// Cross-language zero struct literal macros.
#if defined(ENGINE_LANG_CPP)
#define nil {}
#elif defined(ENGINE_LANG_C)
#define nil {0}
#endif

// Cross-language struct intializer/designated initializer list helper macros.
#if defined(ENGINE_LANG_CPP)
#define make_struct(T, ...) { __VA_ARGS__ }
#define make_union(T, ...) { __VA_ARGS__ }
#elif defined(ENGINE_LANG_C)
#define make_struct(T, ...) (struct T){ __VA_ARGS__ }
#define make_union(T, ...) (union T){ __VA_ARGS__ }
#endif

// Macro expansion helpers.
#define STRINGIFY_NO_EXPAND(S) #S
#define STRINGIFY(S) STRINGIFY_NO_EXPAND(S)
#define GLUE_NO_EXPAND(A,B) A##B
#define GLUE(A,B) GLUE_NO_EXPAND(A,B)

// Debug break macros.
#ifdef ENGINE_DEBUG
#ifdef ENGINE_OS_WINDOWS
#define STOP_AT_THIS_LINE() __debugbreak()
#else
#define STOP_AT_THIS_LINE()
#endif
#else
#define STOP_AT_THIS_LINE()
#endif

#ifdef ENGINE_DEBUG

// Log and assertion functions.
void trace_function(const char* fmt, ...);
void check_function(b8 expr, const char* fmt, ...);
b8 ensure_function(b8 expr, const char* fmt, ...);

// Log and assertion macros.
#define trace(X, ...) (trace_function(X, ##__VA_ARGS__))
#define STATIC_CHECK(C, ID) static u8 GLUE(ID, __LINE__)[(C)?1:-1] // @Review: Maybe we shouldn't get rid of this one, ever.
#define check(X) check_function((X), "Check failed: %s\n", #X)
#define check_msg(X, ...) check_function((X), __VA_ARGS__)
#define ensure(X) ensure_function((X), "Ensure failed: %s\n", #X)
#define ensure_msg(X, ...) ensure_function((X), __VA_ARGS__)

#else // ENGINE_DEBUG
 
#define trace(X, ...)
#define STATIC_CHECK(C, ID)
#define check(X) ((void)0)
#define check_msg(...) ((void)0)
#define ensure(X) (X)
#define ensure_msg(X, ...) (X)
#endif // ENGINE_DEBUG

// Loop helpers macros.
#define each_index(it, count) (s32 it = 0; it < (count); it += 1)

// Unit macros.
#define KB(n) (((u64)(n)) << 10)
#define MB(n) (((u64)(n)) << 20)
#define GB(n) (((u64)(n)) << 30)
#define TB(n) (((u64)(n)) << 40)
#define THOUSAND(n) ((n)*1000)
#define MILLION(n) ((n)*1000000)
#define BILLION(n) ((n)*1000000000)

// ALIGN_OF, MIN, MAX, CLAMP macros.
#define ALIGN_OF(T) __alignof(T) // @Pending(Platform): This will work just in clang and msvc.
#define MIN(A,B) (((A)<(B))?(A):(B))
#define MAX(A,B) (((A)>(B))?(A):(B))
#define CLAMP(A,X,B) (((X)<(A))?(A):((X)>(B))?(B):(X))

// Misc macros.
#define UNUSED(v) ((void)v)
#define DEFER_BLOCK(begin, end) for(s32 _i_ = ((begin), 0); !_i_; _i_ += 1, (end))
#define ALIGN_POW2(x,b) (((x) + (b) - 1)&(~((b) - 1))) // @Note: b has to be pow2. 

// Type info.
enum data_type
{
   TYPE_NONE
  ,TYPE_FLOAT
  ,TYPE_FLOAT2
  ,TYPE_FLOAT3
  ,TYPE_FLOAT4
  ,TYPE_MAT3
  ,TYPE_MAT4
  ,TYPE_INT
  ,TYPE_INT2
  ,TYPE_INT3
  ,TYPE_INT4
  ,TYPE_SAMPLER2D
  ,TYPE_BOOL
};

b8 is_integer_type(enum data_type type);
u32 get_type_size(enum data_type type);
u32 get_type_len(enum data_type type);

#endif // ENGINE_H_BASE

// -------------------------------------------------------------------------------------------------

#ifdef ENGINE_IMPL_BASE

#ifdef ENGINE_DEBUG

// Log and assertions functions.
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

#endif // ENGINE_DEBUG

// Type info.
b8 is_integer_type(enum data_type type)
{
  switch (type) 
  {
    case TYPE_FLOAT:
    case TYPE_FLOAT2:
    case TYPE_FLOAT3:
    case TYPE_FLOAT4:
    case TYPE_MAT3:
    case TYPE_MAT4: 
      return false; 
    case TYPE_INT:
    case TYPE_INT2:
    case TYPE_INT3:
    case TYPE_INT4:
    case TYPE_SAMPLER2D:
    case TYPE_BOOL: 
      return true;
    case TYPE_NONE:
    default: 
      check(false); 
      return false;
  }
}

u32 get_type_size(enum data_type type)
{
  switch(type) 
  {
    case TYPE_FLOAT     : return 4;
    case TYPE_FLOAT2    : return 4 * 2;
    case TYPE_FLOAT3    : return 4 * 3;
    case TYPE_FLOAT4    : return 4 * 4;
    case TYPE_MAT3      : return 4 * 3 * 3;
    case TYPE_MAT4      : return 4 * 4 * 4;
    case TYPE_INT       : return 4;
    case TYPE_SAMPLER2D : return 32;
    case TYPE_INT2      : return 4 * 2;
    case TYPE_INT3      : return 4 * 3;
    case TYPE_INT4      : return 4 * 4;
    case TYPE_BOOL      : return 1;
    case TYPE_NONE: 
    default: 
      check(false); 
      return 0;
  }
}

u32 get_type_len(enum data_type type)
{
  switch(type) 
  {
    case TYPE_FLOAT     : return 1;         
    case TYPE_FLOAT2    : return 2;
    case TYPE_FLOAT3    : return 3;
    case TYPE_FLOAT4    : return 4;
    case TYPE_MAT3      : return 3 * 3;
    case TYPE_MAT4      : return 4 * 4;
    case TYPE_INT       : return 1;
    case TYPE_SAMPLER2D : return 32;
    case TYPE_INT2      : return 2;
    case TYPE_INT3      : return 3;
    case TYPE_INT4      : return 4;
    case TYPE_BOOL      : return 1;
    default: 
      check(false); 
      return 0;
  }
}

#endif // ENGINE_IMPL_BASE
