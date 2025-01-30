#pragma once
#include "lexer.h"

enum type
{
  COMMAND = 0,
  ARGUMENT = 1,
  INLINE = 2,
  SEQUENCE = 3
};

typedef struct
{
  enum type type;
  token token;
} node;

#define argument_node node

typedef struct command_node
{
  node node;
  node **args;
  int inl;
} command_node;

typedef struct sequence_node
{
  node node;
  node *left;
  node *right; 
} sequence_node;

node *to_ast(const char *input);
void free_ast(node *ast);
