#include "tokenizer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ArgV *new_argv() {
  ArgV *ptr = malloc(sizeof(ArgV));
  ptr->argc = 0;
  ptr->cap = 8;
  ptr->argv = malloc(sizeof(char *) * ptr->cap);

  return ptr;
}

void push_argv(ArgV *argv, char *elem) {
  if (argv->argc + 1 >= argv->cap) {
    argv->cap = argv->cap * 2;
    argv->argv = realloc(argv->argv, argv->cap * sizeof(char *));
    if (argv->argv == NULL) {
      perror("Realloc tok array");
      exit(1);
    }
  }
  argv->argv[argv->argc++] = elem;
}

// -1 error, 0 success, 1 complete but failed
int tokenize(ArgV *argv, char *input) {
  if (input == NULL) {
    return -1;
  }
  int i_len = strlen(input);
  int start = -1;
  char delim = ' ';
  for (int i = 0; i <= i_len; i++) {
    if (input[i] == delim || i == i_len) {
      if (start == -1) {
        start = i+1;
      } else {
        // found the second delim
        int tok_len = i - start;
        char *token = malloc(sizeof(char) * tok_len);
        if (token == NULL) {
          perror("Token malloc failed");
          return -1;
        }
        memcpy(token, input + start, tok_len);
        token[tok_len] = '\0';
        push_argv(argv, token);
        start = -1;
      }
    }
    else{
      if(start == -1){
        start = i;
      }
    }
  }
  return 0;
}
