#pragma once
#include "types.h"

void error(lexer_ctx context, token token);
void read_error(lexer_ctx *context, token *token);
