#include "../include/errors.h"

static token e_token;
static enum token_type expected_type;

void write_error(token actual_token, enum token_type expected)
{
  e_token = actual_token;
  expected_type = expected;
}

void read_error(token *actual_token, enum token_type *type)
{
  *actual_token = e_token;
  *type = expected_type;

  e_token.start = 0,
  e_token.length = 0,
  e_token.type = NONE;
  expected_type = NONE;
}
