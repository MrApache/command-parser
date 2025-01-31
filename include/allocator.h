#pragma once
#include "types.h"

typedef struct
{
  void **ptrs;
  u32 allocations;
  u32 buffer_size;
} allocator;

allocator init_allocator(void **, u32);
void *a_malloc(allocator  *, u64);
void free_allocator(allocator *);
void a_free(allocator *, void *);

#define stack_alloc(alc, type, size) \
  type*__st_al_buf[size] = {0}; \
  alc = init_allocator(__st_al_buf, sizeof(__st_al_buf))

#define static_alloc_decl(name) \
  static allocator name; \
  \
  static inline void * \
  name##_malloc(u64 size) { return a_malloc(&name, size); } \
  \
  static inline void \
  name##_free(void *ptr) { a_free(&name, ptr); } \
  \
  static inline void \
  name##_free_allocator(void) { free_allocator(&name); }

  #define alloc_decl(name) \
  allocator name; \
  \
  inline void * \
  name##_malloc(u64 size) { return a_malloc(&name, size); } \
  \
  inline void \
  name##_free(void *ptr) { a_free(&name, ptr); } \
  \
  inline void \
  name##_free_allocator(void) { free_allocator(&name); }
