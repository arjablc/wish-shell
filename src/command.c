#include "command.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

void command_push_redir(Command *cmd, int target_fd, int mode, char *filename) {
  if (cmd->redirc + 1 >= cmd->redir_cap) {
    cmd->redir_cap = cmd->redir_cap == 0 ? 4 : cmd->redir_cap * 2;
    cmd->redirs = xrealloc(cmd->redirs, sizeof(Redir) * cmd->redir_cap);
  }

  cmd->redirs[cmd->redirc].target_fd = target_fd;
  cmd->redirs[cmd->redirc].mode = mode;
  cmd->redirs[cmd->redirc].filename = filename;
  cmd->redirc++;
}

static bool parse_redir_operator(const char *arg, int *target_fd, int *mode) {
  if (arg == NULL) {
    return false;
  }

  if (arg[0] != '\x1f') {
    return false;
  }

  const char *op = arg + 1;

  if (strcmp(op, ">") == 0 || strcmp(op, "1>") == 0) {
    *target_fd = STDOUT_FILENO;
    *mode = REDIR_TRUNCATE;
    return true;
  }

  if (strcmp(op, "2>") == 0) {
    *target_fd = STDERR_FILENO;
    *mode = REDIR_TRUNCATE;
    return true;
  }

  if (strcmp(op, ">>") == 0 || strcmp(op, "1>>") == 0) {
    *target_fd = STDOUT_FILENO;
    *mode = REDIR_APPEND;
    return true;
  }

  if (strcmp(op, "2>>") == 0) {
    *target_fd = STDERR_FILENO;
    *mode = REDIR_APPEND;
    return true;
  }

  return false;
}

bool command_extract_redirs(Command *cmd) {
  int write_idx = 0;

  for (int read_idx = 0; read_idx < cmd->argc; read_idx++) {
    int target_fd;
    int mode;
    if (!parse_redir_operator(cmd->args[read_idx], &target_fd, &mode)) {
      cmd->args[write_idx++] = cmd->args[read_idx];
      continue;
    }

    if (read_idx + 1 >= cmd->argc) {
      free(cmd->args[read_idx]);
      cmd->argc = write_idx;
      cmd->args[cmd->argc] = NULL;
      return false;
    }

    char *filename = cmd->args[++read_idx];
    command_push_redir(cmd, target_fd, mode, filename);
    free(cmd->args[read_idx - 1]);
  }

  cmd->argc = write_idx;
  cmd->args[cmd->argc] = NULL;
  return true;
}

bool command_is_empty(const Command *cmd) {
  return cmd == NULL || cmd->argc == 0 || cmd->args[0] == NULL;
}
