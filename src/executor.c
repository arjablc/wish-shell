#include "builtins.h"
#include "external.h"
#include "tokenizer.h"

int execute(ArgV *argv) {
  int builtins_res = hanlde_builtins(argv);
  if (builtins_res == 0){
    handle_external(argv);
  }
  return 0;
}
