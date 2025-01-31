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
    cn->node.type == INLINE ? "INLINE_COMMAND" : "COMMAND",
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
  if (sn->right->type == SEQUENCE) {
    print_sequence_node(input, (sequence_node *)sn->right, indent + 1);
  }
  else {
    print_command_node(input, (command_node *)sn->right, indent + 1);
  }
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

static void test(const char *input)
{
  node *ast = to_ast(input);

  if(ast == null) {
    token token;
    enum error_type err;
    read_error(&token, &err);
    printf("ERROR: %s\n", get_enum_name(token.type));
  }
  else {
    print_ast(input, ast);
    free_ast(ast);
  }
}

int main(const int argc, const char **argv)
{
  if(argc < 2) {
    return NOT_ENOUGH_ARGS;
  }

  set_node_cache(16);
  test(argv[1]);
  if (argc > 2) {
    test(argv[2]);
  }
  free_node_cache();
  return 0;
}
