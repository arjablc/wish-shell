#pragma once

#include "command.h"

typedef enum {
  BUILTIN_ERROR = -1,
  BUILTIN_NOT_HANDLED = 0,
  BUILTIN_HANDLED = 1,
} BuiltinResult;

typedef struct BuiltinEntry{
  const char *name;
  int (*fpptr)(Command *cmd);
  int flag;
} BuiltinEntry;

BuiltinResult handle_builtins(Command *cmd);
