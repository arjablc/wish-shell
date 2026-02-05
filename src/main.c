#include "tokenizer.h"
#include "executor.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

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
    ArgV *argv = new_argv();
    int tokenized = tokenize(argv, input);
    if (tokenized == -1){
      printf("Exiting fatal error");
      exit(-1);
    }
    execute(argv);
  }
  return 0;
}
