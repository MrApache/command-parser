#pragma once
#include "types.h"

typedef struct
{
  void *pool;
  u64 size;
  u64 allocated;
} buffer;

buffer * init_buffer(u64 size);
void free_buffer(buffer *b);
void * try_retrieve_memory(buffer *b, u64 size);
int try_free(buffer *b, void *ptr);
void clear_buffer(buffer *b);