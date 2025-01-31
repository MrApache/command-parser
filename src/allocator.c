#include <stdlib.h>
#include <stdio.h>
#include "../include/allocator.h"

allocator
init_allocator(void **buffer, u32 size)
{
  allocator alloc;
  alloc.ptrs = buffer;
  alloc.buffer_size = size;
  alloc.allocations = 0;
  return alloc;
}

void *
a_malloc(allocator *alloc, u64 size)
{
  if(alloc->allocations + size >= alloc->buffer_size) {
    puts("Allocator is full");
    abort();
  }
  void *result = malloc(size);
  if (result == null) {
    return null;
  }
  alloc->ptrs[alloc->allocations++] = result;
  //TODO: index out of range
  //if (alloc->allocations > 1) {}
  alloc->ptrs[alloc->allocations] = null;
  return result;
}

void
free_allocator(allocator *alloc)
{
  void *ptr = null;
  int i = 0;
  while ((ptr = alloc->ptrs[i++]) != null) {
    a_free(alloc, ptr);
  }
}

void
a_free(allocator *alloc, void *ptr)
{
  if (ptr == null) {
    puts("trying to free a null pointer");
    abort();
  }
  free(ptr);
  --alloc->allocations;
}
