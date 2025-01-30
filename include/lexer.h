#pragma once
#include "types.h"

token read_token(lexer_ctx context);
void move_back(lexer_ctx *ctx, token token);
