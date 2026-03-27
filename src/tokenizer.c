#include "tokenizer.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *copy_buffer(const StringBuffer *str_buff) {
  char *token = malloc((size_t)str_buff->len);
  if (token == NULL) {
    perror("malloc");
    exit(1);
  }

  memcpy(token, str_buff->str, (size_t)str_buff->len);
  return token;
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
    if (temp == NULL) {
      perror("Realloc tok array");
      exit(1);
    }
    buff->str = temp;
  }
  buff->str[buff->len++] = (char)c;
}

static bool emit_token_if_any(StringBuffer *str_buff, Command *cmd) {
  if (str_buff->len == 0) {
    return false;
  }
  str_buff_push(str_buff, '\0');
  command_push_arg(cmd, copy_buffer(str_buff));
  clr_str_buff(str_buff);
  return true;
}

TokenizeResult tokenize(Command *cmd, const char *input) {
  if (input == NULL) {
    return TOKENIZE_ERROR;
  }
  int i_len = (int)strlen(input);
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
        emit_token_if_any(str_buff, cmd);
        break;
      case SQ:
        state = S_QUOTE;
        break;
      case ESC:
        str_buff_push(str_buff, input[i + 1]);
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
        return TOKENIZE_INCOMPLETE;
      }
      if (c == '\'') {
        state = SPACE;
      } else {
        str_buff_push(str_buff, c);
      }
    } else {
      if (c == '\\') {
        char next = input[i + 1];
        if (next == '\\' || next == '"') {
          str_buff_push(str_buff, input[i + 1]);
          i++;
          continue;
        }
      }
      if (c == '\0') {
        free_str_buff(str_buff);
        return TOKENIZE_INCOMPLETE;
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
    return TOKENIZE_INCOMPLETE;
  }
  emit_token_if_any(str_buff, cmd);

  free_str_buff(str_buff);
  return TOKENIZE_OK;
}
