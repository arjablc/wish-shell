#include "builtins.h"
#include "external.h"
#include "tokenizer.h"

int execute(ArgV *argv) {
  char *command = argv->argv[0];
  if (is_builtin(command)) {
    hanlde_builtin(argv);
    return 0;
  }
  exec_external(argv);
  return 0;
}
