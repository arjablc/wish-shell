#include "command.h"
#include "tokenizer.h"
#include "executor.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_SIZE 1024

int main(void) {
  setbuf(stdout, NULL);

  char input[INPUT_SIZE];
  while (1) {
    printf("$ ");
    if (fgets(input, INPUT_SIZE, stdin) == NULL) {
      break;
    }

    input[strcspn(input, "\n")] = '\0';

    Command *cmd = command_create();
    TokenizeResult tokenized = tokenize(cmd, input);
    if (tokenized == TOKENIZE_ERROR) {
      command_free(cmd);
      printf("Exiting fatal error");
      exit(-1);
    }

    if (tokenized == TOKENIZE_OK) {
      execute(cmd);
    }

    command_free(cmd);
  }

  return 0;
}
