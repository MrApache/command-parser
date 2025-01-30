#include <stdio.h>
#include "../include/debug.h"

void
print(const char *i)
{
  #ifdef NDEBUG
  puts(i);
  #endif
}