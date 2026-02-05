
#include "tokenizer.h"

// 1 or 0
int is_builtin(const char *command);

// 1 for success
// -1 error
int hanlde_builtin(ArgV* argv);
