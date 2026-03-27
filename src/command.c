#include "command.h"

#include <stdio.h>
#include <stdlib.h>

static void *xmalloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    perror("malloc");
    exit(1);
  }

  return ptr;
}

static void *xrealloc(void *ptr, size_t size) {
  void *new_ptr = realloc(ptr, size);
  if (new_ptr == NULL) {
    perror("realloc");
    exit(1);
  }

  return new_ptr;
}

Command *command_create(void) {
  Command *cmd = xmalloc(sizeof(Command));

  cmd->argc = 0;
  cmd->arg_cap = 8;
  cmd->args = xmalloc(sizeof(char *) * cmd->arg_cap);
  cmd->args[0] = NULL;
  cmd->redirs = NULL;
  cmd->redirc = 0;
  cmd->redir_cap = 0;

  return cmd;
}

void command_free(Command *cmd) {
  if (cmd == NULL) {
    return;
  }

  for (int i = 0; i < cmd->argc; i++) {
    free(cmd->args[i]);
  }

  for (int i = 0; i < cmd->redirc; i++) {
    free(cmd->redirs[i].filename);
  }

  free(cmd->args);
  free(cmd->redirs);
  free(cmd);
}

void command_push_arg(Command *cmd, char *arg) {
  if (cmd->argc + 1 >= cmd->arg_cap) {
    cmd->arg_cap *= 2;
    cmd->args = xrealloc(cmd->args, sizeof(char *) * cmd->arg_cap);
  }

  cmd->args[cmd->argc++] = arg;
  cmd->args[cmd->argc] = NULL;
}

bool command_is_empty(const Command *cmd) {
  return cmd == NULL || cmd->argc == 0 || cmd->args[0] == NULL;
}
