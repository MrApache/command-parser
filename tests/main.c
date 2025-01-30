#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "../include/errors.h"
#include "../include/parser.h"

#define NOT_ENOUGH_ARGS (-2)

static void print_argument_node(const char *input, const argument_node *an, int indent)
{
  char arr[indent + 1];
  memset(arr, ' ', sizeof(arr));
  arr[indent + 1] = '\0';

  printf("%s[%s] %.*s\n",
    arr,
    "ARGUMENT",
    an->token.length,
    input + an->token.start);
}

static void print_command_node(const char *input, const command_node *cn, int indent)
{
  int i = 0;
  int arg_indent = indent + 1;
  node **args = cn->args;
  node *arg;
  char arr[indent + 1];

  memset(arr, ' ', sizeof(arr));
  arr[indent + 1] = '\0';
  printf("%s[%s] %.*s\n",
    arr,
    cn->inl ? "INLINE_COMMAND" : "COMMAND",
    cn->node.token.length,
    input + cn->node.token.start);

  if (args != null) {
    while ((arg = args[i++]) != null) {
      switch(arg->type) {
        case ARGUMENT: print_argument_node(input, arg, arg_indent); break;
        case COMMAND:
        case INLINE: print_command_node(input, (command_node *)arg, arg_indent); break;
        case SEQUENCE: assert(false);
      }
    }
  }
}

static void print_sequence_node(const char *input, const sequence_node *sn, int indent)
{
  char arr[indent + 1];
  memset(arr, ' ', sizeof(arr));
  arr[indent + 1] = '\0';
  printf("%s[%s] %.*s\n",
    arr,
    "SEQUENCE",
    sn->node.token.length,
    input + sn->node.token.start);
  print_command_node(input, (command_node *)sn->left, indent + 1);
  print_command_node(input, (command_node *)sn->right, indent + 1);
}

static void print_ast(const char *input, const node *ast)
{ 
  switch(ast->type) {
    case COMMAND:
      print_command_node(input, (command_node *)ast, 0);
      break;
    case SEQUENCE:
      print_sequence_node(input, (sequence_node *)ast, 0);
      break;
  }
}

int main(const int argc, const char **argv)
{
  const char *input = null;

  if(argc < 2) {
    return NOT_ENOUGH_ARGS;
  }

  input = argv[1];
  node *ast = null;

  /*
  for (int i = 0; i < 20000000; ++i) {
    ast = to_ast(input);
    free_ast(ast);
  }
  */
  ast = to_ast(input);

  if(ast == null) {
    lexer_ctx ctx;
    token token;
    read_error(&ctx, &token);
    puts("Error");
  }
  else {
    print_ast(input, ast);
    free_ast(ast);
  }
  return 0;
}
