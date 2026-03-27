#pragma once

#include "command.h"

typedef enum {
  EXECUTE_ERROR = -1,
  EXECUTE_OK = 0,
} ExecuteResult;

ExecuteResult execute(Command *cmd);
