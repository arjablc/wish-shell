#pragma once

#include <stdbool.h>

typedef struct {
  int target_fd;
  int mode;
  char *filename;
} Redir;

typedef struct {
  char **args;
  int argc;
  int arg_cap;
  Redir *redirs;
  int redirc;
  int redir_cap;
} Command;

Command *command_create(void);
void command_free(Command *cmd);
void command_push_arg(Command *cmd, char *arg);
bool command_is_empty(const Command *cmd);
