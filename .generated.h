#ifndef ENGINE_H_GENERATED                                                         
#define ENGINE_H_GENERATED                                                         
                                                                                   
#ifndef HMAP_HANDLE_DECL                                                           
#define HMAP_HANDLE_DECL                                                           
struct hmap_handle                                                                 
{                                                                                  
   u32 index;                                                                      
   u32 gen; // Generation of the handle.                                           
};                                                                                 
#endif // HMAP_HANDLE_DECL                                                         
                                                                                   
struct gl_vertex_buffer_hmap                                                                     
{                                                                                  
   struct gl_vertex_buffer* data;                                                                
   u32 len;                                                                        
   u32 cap;                                                                        
   struct                                                                          
   {                                                                               
      u32* data;                                                                   
      u32 len;                                                                     
      u32 next;                                                                    
   }                                                                               
   free; // Free indices.                                                          
};                                                                                 
                                                                                   
inline b8 gl_vertex_buffer_hmap_valid(struct gl_vertex_buffer_hmap* map)                                       
{                                                                                  
   return map->data != NULL && map->free.data != NULL && map->cap > 0;             
}                                                                                  
                                                                                   
inline void gl_vertex_buffer_hmap_reset(struct gl_vertex_buffer_hmap* map)                                     
{                                                                                  
   memset(map, 0, sizeof(struct gl_vertex_buffer_hmap));                                         
}                                                                                  
                                                                                   
b8 gl_vertex_buffer_hmap_init(struct gl_vertex_buffer_hmap* map, u32 cap, struct arena* arena);                
struct hmap_handle gl_vertex_buffer_hmap_add(struct gl_vertex_buffer_hmap* map, struct gl_vertex_buffer* elem);              
struct gl_vertex_buffer* gl_vertex_buffer_hmap_get(struct gl_vertex_buffer_hmap* map, struct hmap_handle handle);            
b8 gl_vertex_buffer_hmap_remove(struct gl_vertex_buffer_hmap* map, struct hmap_handle handle);                 
                                                                                   
#endif // ENGINE_H_GENERATED                                                       
                                                                                   
#ifdef ENGINE_IMPL_GENERATED                                                       
                                                                                   
b8 gl_vertex_buffer_hmap_init(struct gl_vertex_buffer_hmap* map, u32 cap, struct arena* arena)                 
{                                                                                  
   check(!gl_vertex_buffer_hmap_valid(map));                                                     
                                                                                   
   gl_vertex_buffer_hmap_reset(map);                                                             
   map->data = arena_push_array(arena, struct gl_vertex_buffer, cap);                            
   map->free.data = arena_push_array(arena, u32, cap);                             
   check(gl_vertex_buffer_hmap_valid(map));                                                      
   return true;                                                                    
}                                                                                  
                                                                                   
struct hmap_handle gl_vertex_buffer_hmap_add(struct gl_vertex_buffer_hmap* map, struct gl_vertex_buffer* elem)               
{                                                                                  
   check(gl_vertex_buffer_hmap_valid(map));                                                      
                                                                                   
   u32 n = map->free.next;                                                         
                                                                                   
   // Occupy free slot. New generation.                                            
   if (n != 0)                                                                     
   {                                                                               
      struct gl_vertex_buffer* data = &map->data[n];                                             
      map->free.next = map->free.data[n];                                          
      map->free.data[n] = 0;                                                       
      u32 prev_gen = data->handle.gen;                                             
      *data = *elem;                                                               
      data->handle.index = n;                                                      
      data->handle.gen = prev_gen + n;                                             
      map->free.len -= 1;                                                          
      return data->handle;                                                         
   }                                                                               
                                                                                   
   // ZII chad                                                                     
   if (n == 0)                                                                     
   {                                                                               
      memset(&map->data[0], 0, sizeof(struct gl_vertex_buffer));                                 
      map->len += 1;                                                               
   }                                                                               
                                                                                   
   // Full.                                                                        
   if (map->len == map->cap)                                                       
   {                                                                               
      return map->data[0].handle;                                                  
   }                                                                               
                                                                                   
   // Occupy new slot. First generation.                                           
   struct gl_vertex_buffer* data = &map->data[map->len];                                         
   *data = *elem;                                                                  
   data->handle.index = map->len;                                                  
   data->handle.gen = 1;                                                           
   map->len += 1;                                                                  
   return data->handle;                                                            
}                                                                                  
                                                                                   
struct gl_vertex_buffer* gl_vertex_buffer_hmap_get(struct gl_vertex_buffer_hmap* map, struct hmap_handle handle)             
{                                                                                  
  check(gl_vertex_buffer_hmap_valid(map));                                                       
                                                                                   
  // Discard non-sense handle.                                                     
  if (handle.index == 0 || handle.index >= map->len)                               
  {                                                                                
    return NULL;                                                                   
  }                                                                                
                                                                                   
  // Check if elem is valid.                                                       
  struct gl_vertex_buffer* elem = &map->data[handle.index];                                      
  if (elem->handle.index == handle.index && elem->handle.gen == handle.gen)        
  {                                                                                
    // Success.                                                                    
    return elem;                                                                   
  }                                                                                
                                                                                   
  return NULL;                                                                     
}                                                                                  
                                                                                   
b8 gl_vertex_buffer_hmap_remove(struct gl_vertex_buffer_hmap* map, struct hmap_handle handle)                  
{                                                                                  
  check(gl_vertex_buffer_hmap_valid(map));                                                       
                                                                                   
  // Discard non-sense handle.                                                     
  if (handle.index == 0 || handle.index >= map->len)                               
  {                                                                                
    return false;                                                                  
  }                                                                                
                                                                                   
  // Check if elem is valid.                                                       
  struct gl_vertex_buffer* elem = &map->data[handle.index];                                      
  if (elem->handle.index == handle.index && elem->handle.gen == handle.gen)        
  {                                                                                
    // Success.                                                                    
    map->free.data[handle.index] = map->free.next;                                 
    map->free.next = handle.index;                                                 
    map->free.len += 1;                                                            
    elem->handle.index = 0;                                                        
    return true;                                                                   
  }                                                                                
                                                                                   
  return false;                                                                    
}                                                                                  
                                                                                   
#endif // ENGINE_IMPL_GENERATED                                                    
                                                                                   
