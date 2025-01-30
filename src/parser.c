#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <setjmp.h>
#include "../include/parser.h"
#include "../include/errors.h"
#include "../include/membuf.h"
#include "../include/allocator.h"
#include "../include/debug.h"

static_alloc_decl(alc);

static jmp_buf jump_buffer;
static lexer_ctx ctx;
static int start;
static int length;
static node *ast = null;
static buffer *buf = null;

static inline void
free_mem(void *ptr)
{
  assert(ptr);
  if (!try_free(buf, ptr)) {
    alc_free(ptr);
  }
}

static inline void *
alloc_mem(u64 size)
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

#define throw(context, token) \
   error(context, token); \
   longjmp(jump_buffer, 1);

static inline token
t_read(void)
{
  return read_token(ctx);
}

static inline void
consume_open_par(void)
{
  const token token = t_read();
  if(token.type != OPEN_PAR) {
    throw(ctx, token);
  } 
}

static inline void
consume_close_par(void)
{
  const token token = t_read();
  if(token.type != CLOSE_PAR) {
    throw(ctx, token);
  } 
}

static node *
parse_inline_command_node(void)
{
  node *command;
  token token;

  consume_open_par();
  token = t_read();
  if (token.type != IDENTIFIER) {
    throw(ctx, token);
  }
  command = parse_command_node(token, true);
  consume_close_par();
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
        an = try_retrieve_memory(buf, sizeof(argument_node));
        if (an == null) {
          puts("cant retrieve memory");
          abort(); //TODO: message
        }
        an->type = b_node.type;
        an->token = b_node.token;
        arguments[buf_pos++] = an;
        arguments[buf_pos] = null;
        break;
      case DOLLAR:
        cn = parse_inline_command_node();
        arguments[buf_pos++] = cn;
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
      case VERTICAL_BAR:
      case OPEN_PAR:
        throw(ctx, token);
      case NONE:
        assert(false);
    }
  }
  if (buf_pos != 0) {
    const size_t size = sizeof(node *) * (buf_pos + 1); //include null at the end
    result = alc_malloc(size); //TODO: buffer array
    if (result == null) {
      puts("out of memory");
      abort(); //TODO: message
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

  cn = alloc_mem(sizeof(command_node));
  cn->node = b_node;
  cn->args = args;
  cn->inl = inl;
  return &cn->node;
}

#define abs(x) x < 0 ? -(x) : x

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
    //error(ctx, token);
  }

  right = parse_node();

  if(right == null) {
    puts("right == null");
    abort();
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
      case IDENTIFIER:
        ast = parse_command_node(token, false);
        break;
      case SEMICOLON:
        ast = parse_sequence_node();
        break;
      case DOLLAR:
        ast = parse_inline_command_node();
        break;
      case END:
        run = 0;
        break;

      case NONE: assert(false);
      case UNKNOWN: 
      case STR_LITERAL:
      case INT_LITERAL:
      case OPEN_PAR:
      case CLOSE_PAR:
      case VERTICAL_BAR:
        throw(ctx, token);
    }
  }
  return ast; //TODO: Fixme
}

node *
to_ast(const char *input)
{
  if (buf == null) {
    buf = init_buffer(sizeof(sequence_node) * 16);
  }
  /*
  else {
    puts("Buffer is not null");
    abort();
  }
  */
  stack_alloc(alc, void, 256);

  start = 0;
  length = strlen(input);

  ctx.str = input;
  ctx.start = &start;
  ctx.length = &length;

  if (setjmp(jump_buffer) == 0) {
    parse_node();
    #ifndef MEMPRNT
      #ifndef NDEBUG
    printf("Used memory: %lu, Total: %lu, Allocations:%d\n", buf->allocated, buf->size, alc.allocations);
      #endif
    #endif
    return ast;
  }
  clear_buffer(buf);
  buf = null;
  alc_free_allocator();
  assert(alc.allocations == 0);
  return null;
}

static void 
free_command_node(command_node *cn)
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

static void free_sequence_node(sequence_node *sn);

static void 
free_node(node *node)
{
  switch(node->type) {
    case COMMAND:
      command_node *cn = (command_node *)node;
      free_command_node(cn);
      break;
    case SEQUENCE:
      sequence_node *sn = (sequence_node *)node;
      free_sequence_node(sn);
      break;
    case ARGUMENT:
    case INLINE:
    default: assert(false);
  }
}

static inline
void free_sequence_node(sequence_node *sn)
{
  free_node(sn->left);
  free_node(sn->right);
  free_mem(sn);
}

void
free_ast(node *ast)
{
  if (ast == null) {
    return;
  }
  free_node(ast);
  clear_buffer(buf);

  assert(alc.allocations == 0);
}
