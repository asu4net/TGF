#ifndef ENGINE_DECLARATION_SYS
#define ENGINE_DECLARATION_SYS

struct system_info 
{
  u64 page_size;
  u64 large_page_size;
  u64 allocation_granularity;
};

struct system_info* get_system_info();

void* reserve_large(u64 size);

b8 commit_large(void* ptr, u64 size);

void* reserve(u64 size);

b8 commit(void* ptr, u64 size);

void decommit(void* ptr, u64 size);

void release(void* ptr, u64 size);

#endif // ENGINE_DECLARATION_SYS

// -------------------------------------------------------------------------------------------------

#ifdef ENGINE_IMPLEMENTATION_SYS

#ifdef ENGINE_OS_WINDOWS
#define ENGINE_IMPLEMENTATION_SYS_WIN32
#include "core/sys_win32.h"
#endif

struct system_info* get_system_info()
{
#ifdef ENGINE_OS_WINDOWS
  return get_system_info_win32();
#else
  #error "Not implemented!"
  return NULL;
#endif
}

void* reserve_large(u64 size)
{
#ifdef ENGINE_OS_WINDOWS
  return reserve_large_win32(size);
#else
  #error "Not implemented!"
  return NULL;
#endif
}

b8 commit_large(void* ptr, u64 size) 
{
#ifdef ENGINE_OS_WINDOWS
  return commit_large_win32(ptr, size);
#else
  #error "Not implemented!"
  return false;
#endif
}

void* reserve(u64 size)
{
#ifdef ENGINE_OS_WINDOWS
  return reserve_win32(size);
#else
  #error "Not implemented!"
  return NULL;
#endif
}

b8 commit(void* ptr, u64 size)
{
#ifdef ENGINE_OS_WINDOWS
  return commit_win32(ptr, size);
#else
  #error "Not implemented!"
  return false;
#endif
}

void decommit(void* ptr, u64 size)
{
#ifdef ENGINE_OS_WINDOWS
  decommit_win32(ptr, size);
#else
  #error "Not implemented!"
#endif
}

void release(void* ptr, u64 size)
{
#ifdef ENGINE_OS_WINDOWS
  release_win32(ptr, size);
#else
  #error "Not implemented!"
#endif
}

#endif // ENGINE_IMPLEMENTATION_SYS
