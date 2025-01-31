#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "../include/parser.h"
#include "../include/errors.h"
#include "../include/membuf.h"
#include "../include/allocator.h"

#define return_if_null(a) \
  if(a == null) { \
    return null; \
  }

#define error(__token, __err) \
  interrupt = 1; \
  write_error(__token, __err); \
  return null

static_alloc_decl(alc);

static u8 interrupt = 0;
static lexer_ctx ctx;
static int start;
static int length;
static node *ast = null;
static buffer *buf = null;

static inline void
free_mem(void *ptr)
{
  if (!try_free(buf, ptr)) {
    alc_free(ptr);
  }
}

static inline void *
alloc_mem(const u64 size)
{
  void *ptr = try_retrieve_memory(buf, size);
  if (ptr == null) {
    ptr = alc_malloc(size);
  }
  if (ptr == null) {
    puts("out of memory");
    exit(1);
  }
  return ptr;
}

static node **parse_arguments_node(void);
static node *parse_command_node(token, int);
static node *parse_inline_command_node(void);
static node *parse_sequence_node(void);
static node *parse_node(void);

#define s_parse_command_node(name, _token, _int) \
  name = parse_command_node(_token, _int); \
  return_if_null(name)

#define s_parse_inline_command_node(name) \
  name = parse_inline_command_node(); \
  return_if_null(name)

#define s_parse_sequence_node(name) \
  name = parse_sequence_node(); \
  return_if_null(name)

#define s_parse_node(name) \
  name = parse_node(); \
  return_if_null(name)

static inline token
t_read(void)
{
  return read_token(ctx);
}

//SAFE
static node *
parse_inline_command_node(void)
{
  node *command;
  token tok;

  const token t_open_par = t_read();
  if (t_open_par.type != OPEN_PAR) {
    error(t_open_par, UNEXPECTED_TOKEN);
  }

  tok = t_read();
  if (tok.type != IDENTIFIER) {
    error(tok, UNEXPECTED_TOKEN);
  }

  s_parse_command_node(command, tok, true);

  const token t_close_par = t_read();
  if (t_close_par.type != CLOSE_PAR) {
    error(t_close_par, UNEXPECTED_TOKEN);
  }
  return command;
}

static node **
parse_arguments_node(void)
{
  node **result = null;
  node *arguments[64];
  node b_node;
  i32 buf_pos = 0;
  b_node.type = ARGUMENT;

  argument_node *an = null;
  node *cn = null;

  token token;
  i8 run = 1;

  while(run) {
    token = t_read();
    b_node.token = token;
    switch(token.type) {
      case IDENTIFIER:
      case INT_LITERAL:
      case STR_LITERAL:
        an = alloc_mem(sizeof(argument_node));
        an->type = b_node.type;
        an->token = b_node.token;
        arguments[buf_pos++] = an;
        //TODO: index out of range (UB)
        arguments[buf_pos] = null;
        break;
      case DOLLAR:
        s_parse_inline_command_node(cn);
        arguments[buf_pos++] = cn;
        //TODO: index out of range (UB) or allocate memory
        arguments[buf_pos] = null;
        break;
      case CLOSE_PAR:
      case SEMICOLON:
        move_back(&ctx, token);
        run = 0;
        break;
      case END:
        run = 0;
        break;
      case UNKNOWN:
        error(token, UNKNOWN_TOKEN);
      case VERTICAL_BAR:
      case OPEN_PAR:
        error(token, UNEXPECTED_TOKEN);
      case NONE:
        assert(false);
    }
  }
  if (buf_pos != 0) {
    const size_t size = sizeof(node *) * (buf_pos + 1); //include null at the end
    result = alc_malloc(size);
    if (result == null) {
      puts("Error: Out of memory");
      abort();
    }
    memcpy(result, arguments, size);
  }
  return result;
}

static node *
parse_command_node(token token, int inl)
{
  command_node *cn = null;
  node **args = null;
  node b_node;

  b_node.type = inl ? INLINE : COMMAND;
  b_node.token = token;

  args = parse_arguments_node();
  if (args == null && interrupt) {
    return null;
  }

  cn = alloc_mem(sizeof(command_node));
  cn->node = b_node;
  cn->args = args;
  return &cn->node;
}

#define abs(x) (x < 0 ? -(x) : x)

static node *
parse_sequence_node(void)
{ 
  i32 delta = 0;
  node b_node;
  b_node.type = SEQUENCE;
  node *left = ast;
  node *right = null;

  if(left == null){
    puts("left == null");
    abort();
  }

  ast = null; //replace

  right = parse_node();

  if(right == null) {
    if (interrupt) {
      return null;
    }
    return left;
  }

  delta = abs(right->token.length - right->token.start);
  b_node.token.type = SEMICOLON;
  b_node.token.start = left->token.start;
  b_node.token.length = right->token.start + delta;

  sequence_node *sn = alloc_mem(sizeof(sequence_node));
  sn->node = b_node;
  sn->left = left;
  sn->right = right;

  return &sn->node;
} 

static node *
parse_node(void)
{
  token token;
  i8 run = 1;

  while(run) {
    token = t_read();
    switch(token.type) {
      case IDENTIFIER: s_parse_command_node(ast, token, false); break;
      case SEMICOLON:  s_parse_sequence_node(ast); break;
      case DOLLAR:     s_parse_inline_command_node(ast); break;
      case END:        run = 0; break;
      case UNKNOWN:
        error(token, UNKNOWN_TOKEN);
      case STR_LITERAL:
      case INT_LITERAL:
      case OPEN_PAR:
      case CLOSE_PAR:
      case VERTICAL_BAR: error(token, UNEXPECTED_TOKEN);
      case NONE: assert(false);
    }
  }
  return ast;
}

node *
to_ast(const char *input)
{
  stack_alloc(alc, void, 256);

  interrupt = 0;
  start = 0;
  length = strlen(input);

  ctx.str = input;
  ctx.start = &start;
  ctx.length = &length;

  ast = parse_node();

  if (ast == null) {
    clear_buffer(buf);
    alc_free_allocator();
    assert(alc.allocations == 0);
    return null;
  }

#ifndef MEMPRNT
#ifndef NDEBUG
  if (buf != null) {
    printf("Used memory: %lu, Total: %lu, Allocations:%d\n", buf->allocated, buf->size, alc.allocations);
  }
  else {
    printf("Cache is not set. Allocations:%d\n", alc.allocations);
  }
#endif
#endif

  return ast;
}

static void free_command_node(command_node *cn)
{
  node **args = cn->args;
  i32 i = 0;
  node *arg = null;

  if (args != null) {
    while ((arg = args[i++]) != null) {
      switch(arg->type) {
        case ARGUMENT: free_mem(arg); break;
        case COMMAND:
        case INLINE: free_command_node((command_node *)arg); break;
        case SEQUENCE:
        default:
          printf("Unknown type: %d\n", arg->type);
          assert(false);
      }
    }

    free_mem(args);
  }

  free_mem(cn);
}

static void free_node(node *node)
{
  switch(node->type) {
    case COMMAND:
      command_node *cn = (command_node *)node;
      free_command_node(cn);
      break;
    case SEQUENCE:
      sequence_node *sn = (sequence_node *)node;
      free_node(sn->left);
      free_node(sn->right);
      free_mem(sn);
      break;
    case ARGUMENT:
    case INLINE:
    default: assert(false);
  }
}

void free_ast(node *node)
{
  if (node == null) {
    return;
  }
  free_node(node);
  clear_buffer(buf);
  ast = null;
  assert(alc.allocations == 0);
}

u64 set_node_cache(u32 node_count)
{
  if (node_count == 0) {
    return 0;
  }

  u64 size = sizeof(sequence_node) * node_count;
  if (buf != null) {
    puts("Invalid operation: Buffer already allocated");
    abort();
  }

  buf = init_buffer(size);
  return size;
}

void free_node_cache(void)
{
  if (buf == null) {
    puts("Invalid operation: Buffer is null");
    abort();
  }

  if (buf->allocated != 0) {
    puts("Invalid operation: Buffer is used now");
    abort();
  }

  free_buffer(buf);
  buf = null;
}
