#pragma once
#include "types.h"

enum error_type
{
  UNKNOWN_TOKEN,
  UNEXPECTED_TOKEN
};

void read_error(token *token, enum error_type *type);
void write_error(token token, enum error_type type);
