#include <stddef.h>
#pragma once

typedef enum TokenizerState {
  SPACE,
  S_QUOTE,
  D_QUOTE,
  ESCAPE
} TokenizerState;

typedef enum CharType {
  SPC,
  SQ,
  DQ,
  ESC,
  END,
  OTHERS
  
} CharType;

char get_delim(TokenizerState state);
CharType get_char_type(char c);

// Just a dynamic array implementation for now
typedef struct {
  char **args;
  int argc;
  int cap;
} ArgV;

typedef struct{
  char *str;
  int len;
  int cap;
} StringBuffer;

StringBuffer *new_str_buff();
void str_buff_push(StringBuffer * buff,char c);
void clr_str_buff(StringBuffer * buff);
void free_str_buff(StringBuffer * buff);
int tokenize(ArgV *argv, char *input);
void push_argv(ArgV *argv, char *elem);
void free_argv(ArgV *argv);
ArgV *new_argv();
