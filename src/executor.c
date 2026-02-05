
#include <stdio.h>

#include "builtins.h"
#include "tokenizer.h"

int execute(ArgV *argv) {
  char *command = argv->argv[0];
  if (is_builtin(command)) {
     hanlde_builtin(argv);
     return 0;
  }
  printf("%s: command not found\n", command);
  return 0;
}
