#include <stdlib.h>
#include <string.h>
#include "../include/membuf.h"
#include "../include/allocator.h"

buffer *
init_buffer(u64 size)
{
  buffer *buffer = malloc(sizeof(buffer));
  buffer->size = size;
  buffer->pool = malloc(size);
  buffer->allocated = 0;
  return buffer;
}

void
free_buffer(buffer *b)
{
  if(b == null) {
    return;
  }

  if(b->pool == null) {
    b->size = 0;
    b->allocated = 0;
    free(b);
    return;
  }

  free(b->pool);
  b->pool = null;
  b->size = 0;
  b->allocated = 0;
  free(b);
}

void *
try_retrieve_memory(buffer *b, u64 size)
{
  void *ptr = null;
  if(size + b->allocated >= b->size) {
    return null;
  }
  ptr = &b->pool[b->allocated];
  b->allocated += size;
  return ptr;
}

int
try_free(buffer *b, void *ptr)
{
  void *pool = b->pool;
  if (ptr >= pool && ptr < pool + b->allocated) {
    return 1;
  }
  return 0;
}

void
clear_buffer(buffer *b)
{
  memset(b->pool, 0, b->allocated);
  b->allocated = 0;
}