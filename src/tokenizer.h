#pragma once

#include "command.h"

typedef enum {
  TOKENIZE_ERROR = -1,
  TOKENIZE_OK = 0,
  TOKENIZE_INCOMPLETE = 1,
} TokenizeResult;

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

CharType get_char_type(char c);

typedef struct{
  char *str;
  int len;
  int cap;
} StringBuffer;

StringBuffer *new_str_buff();
void str_buff_push(StringBuffer * buff,char c);
void clr_str_buff(StringBuffer * buff);
void free_str_buff(StringBuffer * buff);
TokenizeResult tokenize(Command *cmd, const char *input);
