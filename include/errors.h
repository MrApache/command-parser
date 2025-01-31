#pragma once
#include "types.h"

void read_error(token *actual_token, enum token_type *type);
void write_error(token actual_token, enum token_type expected);
