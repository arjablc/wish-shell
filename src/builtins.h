
#include "tokenizer.h"

typedef struct BuiltinEntry{
  const char *name;
  int (*fpptr)(ArgV *argv);
  int flag;
} BuiltinEntry;

// 1 for success
// 0 for not running
// -1 error
int hanlde_builtins(ArgV* argv);
