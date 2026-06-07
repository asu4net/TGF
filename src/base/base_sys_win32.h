#ifndef ENGINE_H_SYS_WIN32
#define ENGINE_H_SYS_WIN32

struct system_info* get_system_info_win32();

void* reserve_large_win32(u64 size);

b8 commit_large_win32(void* ptr, u64 size);

void* reserve_win32(u64 size);

b8 commit_win32(void* ptr, u64 size);

void decommit_win32(void* ptr, u64 size);

void release_win32(void* ptr, u64 size);

#endif // ENGINE_H_SYS_WIN32

// -------------------------------------------------------------------------------------------------

#ifdef ENGINE_IMPL_SYS_WIN32

static struct system_info g_system_info_win32 = nil;

struct system_info* get_system_info_win32()
{
  if (g_system_info_win32.page_size == 0)
  {  
    SYSTEM_INFO sysinfo = nil;
    GetSystemInfo(&sysinfo);
    // @Pending: This we don't need this for now.
    // g_system_info_win32.logical_processor_count = (u64)sysinfo.dwNumberOfProcessors;
    g_system_info_win32.page_size = sysinfo.dwPageSize;
    g_system_info_win32.large_page_size = GetLargePageMinimum();
    g_system_info_win32.allocation_granularity = sysinfo.dwAllocationGranularity;
  }    
  check(g_system_info_win32.page_size != 0);
  return &g_system_info_win32;
}

void* reserve_large_win32(u64 size)
{
  // We commit on reserve because windows. I trust.
  return VirtualAlloc(0, size, MEM_RESERVE|MEM_COMMIT|MEM_LARGE_PAGES, PAGE_READWRITE);
}

b8 commit_large_win32(void* ptr, u64 size) 
{
  UNUSED(ptr);
  UNUSED(size);
  return 1; 
}

void* reserve_win32(u64 size)
{
  UNUSED(size);
  return VirtualAlloc(0, size, MEM_RESERVE, PAGE_READWRITE);
}

b8 commit_win32(void* ptr, u64 size)
{
  return VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE) != 0;
}

void decommit_win32(void* ptr, u64 size)
{
  VirtualFree(ptr, size, MEM_DECOMMIT);
}

void release_win32(void* ptr, u64 size)
{
  // @Note: Size necessary in other OSs
  UNUSED(size);
  VirtualFree(ptr, 0, MEM_RELEASE);
}

#endif // ENGINE_IMPL_SYS_WIN32
