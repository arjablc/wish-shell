#include "executor.h"
#include "builtins.h"
#include "external.h"
#include "redirection.h"

ExecuteResult execute(Command *cmd) {
  if (command_is_empty(cmd)) {
    return EXECUTE_OK;
  }

  int saved_fds[3];
  if (redirections_apply(cmd, saved_fds) != 0) {
    return EXECUTE_ERROR;
  }

  BuiltinResult builtin_result = handle_builtins(cmd);
  redirections_restore(saved_fds);
  if (builtin_result == BUILTIN_ERROR) {
    return EXECUTE_ERROR;
  }

  if (builtin_result == BUILTIN_NOT_HANDLED) {
    return handle_external(cmd) == 0 ? EXECUTE_OK : EXECUTE_ERROR;
  }

  return EXECUTE_OK;
}
