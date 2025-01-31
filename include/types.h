#pragma once

#define true 1
#define false 0
#define null ((void*)0)

typedef unsigned long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

typedef long i64;
typedef int i32;
typedef short i16;
typedef char i8;

enum token_type {
  NONE = 0,
  UNKNOWN = 1,

  STR_LITERAL = 2,
  INT_LITERAL = 3,
  IDENTIFIER = 4,
  END = 5,

  VERTICAL_BAR = 6,
  SEMICOLON = 7,
  DOLLAR = 8,
  OPEN_PAR = 9,
  CLOSE_PAR = 10
};

typedef struct
{
  enum token_type type;
  int start;
  int length;
} token;

#define T_INIT(name) \
  token name; \
  name.type = NONE; \
  name.start = 0; \
  name.length = 0

#define T_SET(name, t, s, l) \
  name.type = t; \
  name.start= s; \
  name.length = l

typedef struct
{
  const char *str;
  int *start;
  int *length;
} lexer_ctx;

#define ctx(s, st, len) \
  lexer_ctx ctx; \
  ctx.str = s; \
  ctx.start = st; \
  ctx.length = len

const char *get_enum_name(enum token_type type);
