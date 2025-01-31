#include "../include/errors.h"

static token e_token;
static enum error_type e_type;

void write_error(token token, enum error_type type)
{
  e_token = token;
  e_type = type;
}

void read_error(token *token, enum error_type *type)
{
  *token = e_token;
  *type = e_type;
}
