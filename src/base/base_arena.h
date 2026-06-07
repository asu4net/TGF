#ifndef ENGINE_H_ARENA
#define ENGINE_H_ARENA 

#define ARENA_HEADER_SIZE 128
#define ARENA_FREE_LIST 1
#define ARENA_FLAG_NO_CHAIN (1<<0)
#define ARENA_FLAG_LARGE_PAGES (1<<1)

struct arena_params 
{
  u32_enum flags;
  u64 reserve_size;
  u64 commit_size;
  void* optional_backing_buffer;
  const char* allocation_site_file;
  s32 allocation_site_line;
};

#define DEFAULT_ARENA_PARAMS make_struct(arena_params, \
  .flags                   = 0,                        \
  .reserve_size            = MB(64),                   \
  .commit_size             = KB(64),                   \
  .optional_backing_buffer = NULL,                     \
  .allocation_site_file    = __FILE__,                 \
  .allocation_site_line    = __LINE__                  \
)                                                      \

struct arena 
{
  struct arena* prev;    // Previous arena in chain.
  struct arena* current; // Current arena in chain.
  u32_enum flags;
  u64 cmt_size;
  u64 res_size;
  u64 base_pos;
  u64 pos;
  u64 cmt;
  u64 res;
  const char* allocation_site_file;
  s32 allocation_site_line;

#if ARENA_FREE_LIST
    struct arena* free_last;
#endif
};

STATIC_CHECK(sizeof(struct arena) <= ARENA_HEADER_SIZE, arena_header_size_check);

struct temp
{
  struct arena* arena;
  u64 pos;
};

struct arena*   arena_alloc           (struct arena_params* params);
void            arena_release         (struct arena* arena);
void*           arena_push            (struct arena* arena, u64 size, u64 align, b8 zero);
u64             arena_pos             (struct arena* arena);
void            arena_pop_to          (struct arena* arena, u64 pos);
void*           arena_first_raw       (struct arena* arena, u64 align);
void            arena_clear           (struct arena* arena);
void            arena_pop             (struct arena* arena, u64 amt);
struct temp     temp_begin            (struct arena* arena);
void            temp_end              (struct temp temp);

// @Note: Push helper macros.
#define arena_push_array_no_zero_aligned(a, T, c, align) (T*) arena_push((a), sizeof(T)*(c), (align), (0))    
#define arena_push_array_aligned(a, T, c, align) (T*) arena_push((a), sizeof(T)*(c), (align), (1))
#define arena_push_array_no_zero(a, T, c) arena_push_array_no_zero_aligned(a, T, c, MAX(8, ALIGN_OF(T)))
#define arena_push_array(a, T, c) arena_push_array_aligned(a, T, c, MAX(8, ALIGN_OF(T)))
#define arena_push_element(a, T) (T*) arena_push((a), sizeof(T), ALIGN_OF(T), (1))
#define arena_first(a, T) (T*) arena_first_raw((a), ALIGN_OF(T))

#endif // ENGINE_H_ARENA 

// -------------------------------------------------------------------------------------------------

#ifdef ENGINE_IMPL_ARENA

struct arena* arena_alloc(struct arena_params* params)
{
  // Round up reserve/commit sizes.
  u64 reserve_size = params->reserve_size;
  u64 commit_size = params->commit_size;
  if(params->flags & ARENA_FLAG_LARGE_PAGES)
  {
    reserve_size = ALIGN_POW2(reserve_size, get_system_info()->large_page_size);
    commit_size  = ALIGN_POW2(commit_size,  get_system_info()->large_page_size);
  }
  else
  {
    reserve_size = ALIGN_POW2(reserve_size, get_system_info()->page_size);
    commit_size  = ALIGN_POW2(commit_size,  get_system_info()->page_size);
  }
  
  // Reserve/commit initial block.
  void* base = params->optional_backing_buffer;
  if(base == 0)
  {
    if(params->flags & ARENA_FLAG_LARGE_PAGES)
    {
      base = reserve_large(reserve_size);
      commit_large(base, commit_size);
    }
    else
    {
      base = reserve(reserve_size);
      commit(base, commit_size);
    }
    
    // @Pending: Keep track of allocations like raddebugger does.
    trace("*** Arena allocation: %u bytes reserved, %u commited from %s:%i\n", reserve_size, commit_size, params->allocation_site_file, params->allocation_site_line);
    //raddbg_annotate_vaddr_range(base, reserve_size, "arena %s:%i", params->allocation_site_file, params->allocation_site_line);
  }
  
  check_msg(base != 0, "Unexpected memory allocation failure.");
  
  // Extract arena header & fill.
  struct arena* arena = (struct arena*) base;
  arena->current = arena;
  arena->flags = params->flags;
  arena->cmt_size = params->commit_size;
  arena->res_size = params->reserve_size;
  arena->base_pos = 0;
  arena->pos = ARENA_HEADER_SIZE;
  arena->cmt = commit_size;
  arena->res = reserve_size;
  arena->allocation_site_file = params->allocation_site_file;
  arena->allocation_site_line = params->allocation_site_line;
#if ARENA_FREE_LIST
  arena->free_last = 0;
#endif
  // @Pending: Use adress sanitizer like raddebugger does.
  //AsanPoisonMemoryRegion(base, commit_size);
  //AsanUnpoisonMemoryRegion(base, ARENA_HEADER_SIZE);
  return arena;
}

void arena_release(struct arena* arena)
{
  for(struct arena* n = arena->current, *prev = 0; n != 0; n = prev)
  {
    prev = n->prev;
    release(n, n->res);
    trace("*** Arena freed: %u bytes.", arena->cmt);
  }
}

// @Note: arena push/pop core functions

void* arena_push(struct arena* arena, u64 size, u64 align, b8 zero)
{
  struct arena* current = arena->current;
  u64 ppre = ALIGN_POW2(current->pos, align);
  u64 ppst = ppre + size;
  
  // Chain, if needed.
  if(current->res < ppst && !(arena->flags & ARENA_FLAG_NO_CHAIN))
  {
    struct arena* new_block = 0;
    
#if ARENA_FREE_LIST
    {
      struct arena* prev_block;
      for (new_block = arena->free_last, prev_block = 0; new_block != 0; prev_block = new_block, new_block = new_block->prev)
      {
        if (new_block->res >= ALIGN_POW2(new_block->pos, align) + size)
        {
          if (prev_block)
          {
            prev_block->prev = new_block->prev;
          }
          else
          {
            arena->free_last = new_block->prev;
          }
          break;
        }
      }
    }
#endif
    
    if (new_block == 0)
    {
      u64 res_size = current->res_size;
      u64 cmt_size = current->cmt_size;

      if (size + ARENA_HEADER_SIZE > res_size)
      {
        res_size = ALIGN_POW2(size + ARENA_HEADER_SIZE, align);
        cmt_size = ALIGN_POW2(size + ARENA_HEADER_SIZE, align);
      }
      struct arena_params params = DEFAULT_ARENA_PARAMS;
      params.reserve_size = res_size;
      params.commit_size = cmt_size;
      params.flags = current->flags;
      params.allocation_site_file = current->allocation_site_file;
      params.allocation_site_line = current->allocation_site_line;
      new_block = arena_alloc(&params);
    }
    
    new_block->base_pos = current->base_pos + current->res;
    new_block->prev = arena->current, arena->current = new_block;
    
    current = new_block;
    ppre = ALIGN_POW2(current->pos, align);
    ppst = ppre + size;
  }
  
  // Compute the size we need to zero.
  u64 size_to_zero = 0;
  if (zero)
  {
    size_to_zero = MIN(current->cmt, ppst) - ppre;
  }
  
  // Commit new pages, if needed.
  if (current->cmt < ppst)
  {
    u64 cmt_pst_aligned = ppst + current->cmt_size - 1;
    cmt_pst_aligned -= cmt_pst_aligned % current->cmt_size;
    u64 cmt_pst_clamped = MIN(cmt_pst_aligned, current->res);
    u64 cmt_size = cmt_pst_clamped - current->cmt;
    u8 *cmt_ptr = (u8*) current + current->cmt;

    if (current->flags & ARENA_FLAG_LARGE_PAGES)
    {
      commit_large(cmt_ptr, cmt_size);
    }
    else
    {
      commit(cmt_ptr, cmt_size);
    }
    current->cmt = cmt_pst_clamped;
  }
  
  // Push onto current block.
  void* result = 0;
  if (current->cmt >= ppst)
  {
    result = (u8 *)current+ppre;
    current->pos = ppst;

    // @Pending: Use adress sanitizer like raddebugger does.
    //AsanUnpoisonMemoryRegion(result, size);
    if (size_to_zero != 0)
    {
      memset(result, 0, size_to_zero);
    }
  }
  
  check_msg(result != 0, "Unexpected memory allocation failure.");
  return result;
}

u64 arena_pos(struct arena* arena)
{
  struct arena *current = arena->current;
  u64 pos = current->base_pos + current->pos;
  return pos;
}

void arena_pop_to(struct arena* arena, u64 pos)
{
  u64 big_pos = MAX(ARENA_HEADER_SIZE, pos);
  struct arena *current = arena->current;
  
#if ARENA_FREE_LIST
  for (struct arena* prev = NULL; current->base_pos >= big_pos; current = prev)
  {
    prev = current->prev;
    current->pos = ARENA_HEADER_SIZE;
    current->prev = arena->free_last, arena->free_last = current;

    // @Pending: Use adress sanitizer like raddebugger does.
    //AsanPoisonMemoryRegion((u8*)current + ARENA_HEADER_SIZE, current->res - ARENA_HEADER_SIZE);
  }
#else
  for (struct arena* prev = 0; current->base_pos >= big_pos; current = prev)
  {
    prev = current->prev;
    release(current, current->res);
  }
#endif
  arena->current = current;
  u64 new_pos = big_pos - current->base_pos;
  check(new_pos <= current->pos);

  // @Pending: Use adress sanitizer like raddebugger does.
  // AsanPoisonMemoryRegion((u8*)current + new_pos, (current->pos - new_pos));
  current->pos = new_pos;
}

void* arena_first_raw(struct arena* arena, u64 align)
{
  u64 raw_start = (u64) ((u8*) arena) + ARENA_HEADER_SIZE;
  u64 start = ALIGN_POW2(raw_start, align);
  return (void*) start;
}

// @Note: arena push/pop helpers

void arena_clear(struct arena* arena)
{
  arena_pop_to(arena, 0);
}

void arena_pop(struct arena* arena, u64 amt)
{
  u64 pold = arena_pos(arena);
  u64 pnew = pold;
  if(amt < pold)
  {
    pnew = pold - amt;
  }
  arena_pop_to(arena, pnew);
}

// @Note: temporary arena scopes

struct temp temp_begin(struct arena* arena)
{
  u64 pos = arena_pos(arena);
  struct temp temp = {arena, pos};
  return temp;
}

void temp_end(struct temp temp)
{
  arena_pop_to(temp.arena, temp.pos);
}

#endif // ENGINE_IMPL_ARENA 
