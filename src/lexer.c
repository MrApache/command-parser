#include <stdio.h>
#include <ctype.h>
#include "../include/lexer.h"

static lexer_ctx *ctx;
static i32 position;

static void move_str_ptr(void)
{
  ++*ctx->start;
  --*ctx->length;
}

static void t_move_str_ptr(token token)
{
  *ctx->start += token.length;
  *ctx->length -= token.length;
}

static char read_char(void)
{
  //Unsafe function
  return ctx->str[position++];
}

static token read_str_literal(void)
{
  const int start = *ctx->start;
  int length = 0;
  char ch = '\0';
  T_INIT(token);
  for(;length < *ctx->length; ++length) {
    ch = read_char();
    if(ch == '"') {
      T_SET(token, STR_LITERAL, start, length);
      return token;
    }
  }

  T_SET(token, UNKNOWN, start - 1, 1);
  return token;
}

static token read_identifier(void)
{
  const int start = *ctx->start;
  i32 length = 0;
  char ch = '\0';
  T_INIT(token);

  for(;; ++length) {
    ch = read_char();
    if(isalpha(ch) || isdigit(ch) || ch == '_') {
      continue;
    }
    break;
  }
  T_SET(token, IDENTIFIER, start, length);
  return token;
}

//TODO: change max_size and implement negative numbers
static token read_int_literal(void)
{
  const int start = *ctx->start;
  int length = 0;
  char ch = '\0';
  T_INIT(token);

  for(;; ++length) {
    ch = read_char();
    if(isdigit(ch) || ch == '.') {
      continue;
    }
    break;
  }

  T_SET(token, INT_LITERAL, start, length);
  return token;
}

token read_token(lexer_ctx context)
{
  ctx = &context;
  T_INIT(token);
  char ch = '\0';
  position = *ctx->start;

  while (*ctx->length != 0) {
    ch = ctx->str[position];
    if(isspace(ch)) {
      read_char();
      move_str_ptr();
    }
    else if(isalpha(ch)) {
      token = read_identifier();
      t_move_str_ptr(token);
      return token;
    }
    else if(isdigit(ch)) {
      token = read_int_literal();
      t_move_str_ptr(token);
      return token;
    }
    else if(ch == ';') {
      T_SET(token, SEMICOLON, position, 1);
      move_str_ptr();
      return token;
    }
    else if(ch == '$') {
      T_SET(token, DOLLAR, position, 1);
      move_str_ptr();
      return token;
    }
    else if(ch == '|') {
      T_SET(token, VERTICAL_BAR, position, 1);
      move_str_ptr();
      return token;
    }
    else if(ch == '(') {
      T_SET(token, OPEN_PAR, position, 1);
      move_str_ptr();
      return token;
    }
    else if(ch == ')') {
      T_SET(token, CLOSE_PAR, position, 1);
      move_str_ptr();
      return token;
    }
    /*
    else if(ch == '&') {
      T_SET(token, AMPERSAND, position, 1);
      move_str_ptr();
      return token;
    }
    */
    else if(ch == '"') {
      read_char();
      move_str_ptr();
      token = read_str_literal();
      if (token.type == STR_LITERAL)  {
        t_move_str_ptr(token);
        move_str_ptr();
      }
      return token;
    }
    else {
      T_SET(token, UNKNOWN, position, 1);
      move_str_ptr();
      return token;
    }
  }

  T_SET(token, END, 0, 0);
  return token;
}

void move_back(lexer_ctx *ctx, token token)
{
  *ctx->start = token.start;
  *ctx->length += token.length;
}