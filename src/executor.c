#include "executor.h"
#include "builtins.h"
#include "external.h"

ExecuteResult execute(Command *cmd) {
  if (command_is_empty(cmd)) {
    return EXECUTE_OK;
  }

  BuiltinResult builtin_result = handle_builtins(cmd);
  if (builtin_result == BUILTIN_ERROR) {
    return EXECUTE_ERROR;
  }

  if (builtin_result == BUILTIN_NOT_HANDLED) {
    return handle_external(cmd) == 0 ? EXECUTE_OK : EXECUTE_ERROR;
  }

  return EXECUTE_OK;
}
