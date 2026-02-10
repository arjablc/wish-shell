#include "tokenizer.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ArgV *new_argv() {
  ArgV *ptr = malloc(sizeof(ArgV));
  if (ptr == NULL) {
    perror("malloc");
    exit(1);
  }
  ptr->argc = 0;
  ptr->cap = 8;
  ptr->args = malloc(sizeof(char *) * ptr->cap);
  if (ptr->args == NULL) {
    perror("malloc");
    exit(1);
  }

  return ptr;
}
void free_argv(ArgV *argv) {
  free(argv->args);
  free(argv);
}

void push_argv(ArgV *argv, char *elem) {
  if (argv->argc + 1 >= argv->cap) {
    argv->cap = argv->cap * 2;
    char **temp = realloc(argv->args, argv->cap * sizeof(char *));
    if (temp == NULL) {
      perror("Realloc tok array");
      exit(1);
    }
    argv->args = temp;
  }
  argv->args[argv->argc++] = elem;
}

CharType get_char_type(char c) {
  switch (c) {
  case ' ':
    return SPC;
  case '\'':
    return SQ;
  case '\"':
    return DQ;
  case '\0':
    return END;
  case '\\':
    return ESC;
  default:
    return OTHERS;
  }
}

char get_delim(TokenizerState state) {
  switch (state) {
  case SPACE:
    return ' ';
  case S_QUOTE:
    return '\'';
  case D_QUOTE:
    return '"';
  case ESCAPE:
    return '\\';
  }
  return '\0';
}

StringBuffer *new_str_buff() {
  StringBuffer *out = malloc(sizeof(StringBuffer));
  if (out == NULL) {
    perror("malloc");
    exit(1);
  }
  out->cap = 64;
  out->len = 0;
  out->str = malloc(sizeof(char) * out->cap);
  if (out->str == NULL) {
    perror("malloc");
    exit(1);
  }
  return out;
}

void clr_str_buff(StringBuffer *buff) {
  buff->cap = 64;
  buff->len = 0;
  char *temp = realloc(buff->str, 64 * sizeof(char));
  if (temp == NULL) {
    perror("malloc");
    exit(1);
  }
  buff->str = temp;
}

void free_str_buff(StringBuffer *buff) {
  if (buff == NULL) {
    return;
  }
  free(buff->str);
  free(buff);
}

void str_buff_push(StringBuffer *buff, char c) {
  if (buff->len + 1 >= buff->cap) {
    buff->cap = buff->cap * 2;
    char *temp = realloc(buff->str, buff->cap * sizeof(char));
    buff->str = temp;
    if (buff->str == NULL) {
      perror("Realloc tok array");
      exit(1);
    }
  }
  buff->str[buff->len++] = (char)c;
}

bool emit_token_if_any(StringBuffer *str_buff, ArgV *argv) {
  if (str_buff->len == 0) {
    return false;
  }
  str_buff_push(str_buff, '\0');
  char *token = malloc((strlen(str_buff->str) + 1) * sizeof(char));
  if (token == NULL) {
    perror("malloc");
    exit(1);
  }
  strcpy(token, str_buff->str);
  push_argv(argv, token);
  clr_str_buff(str_buff);
  return true;
}

// -1 error, 0 success, 1 complete but failed
int tokenize(ArgV *argv, char *input) {
  if (input == NULL) {
    return -1;
  }
  int i_len = strlen(input);
  TokenizerState state = SPACE;
  StringBuffer *str_buff = new_str_buff();
  for (int i = 0; i <= i_len; i++) {
    char c = input[i];
    CharType charType = get_char_type(c);
    if (state == SPACE) {
      if (c == '\0') {
        break;
      }
      switch (charType) {
      case SPC:
        emit_token_if_any(str_buff, argv);
        break;
      case SQ:
        state = S_QUOTE;
        break;
      case ESC:
        str_buff_push(str_buff, input[i+1]);
        i++;
        continue;
        break;
      case DQ:
        state = D_QUOTE;
        break;
      default:
        str_buff_push(str_buff, c);
        break;
      }
    } else if (state == S_QUOTE) {
      if (c == '\0') {
        free_str_buff(str_buff);
        return 1;
      }
      if (c == '\'') {
        state = SPACE;
      } else {
        str_buff_push(str_buff, c);
      }
    } else {
      if (c == '\0') {
        free_str_buff(str_buff);
        return 1;
      }
      if (c == '"') {
        state = SPACE;
      } else {
        str_buff_push(str_buff, c);
      }
    }
  }

  if (state != SPACE) {
    free_str_buff(str_buff);
    return 1;
  }
  emit_token_if_any(str_buff, argv);

  // for (size_t i = 0; i < argv->argc; i++) {
  //   puts(argv->args[i]);
  // }

  free_str_buff(str_buff);
  return 0;
}
