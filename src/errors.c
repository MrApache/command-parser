#include <stdlib.h>
#include "../include/errors.h"

static lexer_ctx e_context;
static token e_token;

void error(lexer_ctx context, token token)
{
  e_context = context;
  e_token = token;
  //abort();
}

void read_error(lexer_ctx *context, token *token)
{
  context = &e_context;
  token = &e_token;
}
