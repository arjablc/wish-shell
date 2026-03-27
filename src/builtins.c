#include "builtins.h"
#include "shell_path.h"

#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

static bool change_directory(const char *path);
static BuiltinEntry *lookup_builtin(const char *command);
static const char *cd_target(const Command *cmd);

static int handle_type(Command *cmd);
static int handle_echo(Command *cmd);
static int handle_exit(Command *cmd);
static int handle_pwd(Command *cmd);
static int handle_cd(Command *cmd);

static BuiltinEntry builtins[] = {
    {"echo", handle_echo, 0}, {"exit", handle_exit, 0},
    {"type", handle_type, 0}, {"pwd", handle_pwd, 0},
    {"cd", handle_cd, 0},
};
static const int builtin_len = 5;

static BuiltinEntry *lookup_builtin(const char *command) {
  for (int i = 0; i < builtin_len; i++) {
    if (command && strcmp(command, builtins[i].name) == 0) {
      return &builtins[i];
    }
  }
  return NULL;
}

BuiltinResult handle_builtins(Command *cmd) {
  if (command_is_empty(cmd)) {
    return BUILTIN_NOT_HANDLED;
  }

  BuiltinEntry *entry = lookup_builtin(cmd->args[0]);
  if (entry == NULL) {
    return BUILTIN_NOT_HANDLED;
  }

  return entry->fpptr(cmd);
}

static int handle_pwd(Command *cmd) {
  (void)cmd;
  char buff[PATH_MAX];
  if (getcwd(buff, sizeof(buff)) == NULL) {
    perror("getcwd");
    return BUILTIN_ERROR;
  }
  printf("%s\n", buff);
  return BUILTIN_HANDLED;
}

static int handle_cd(Command *cmd) {
  const char *path = cd_target(cmd);
  if (!change_directory(path)) {
    printf("cd: %s: No such file or directory\n", path);
  }
  return BUILTIN_HANDLED;
}

static const char *cd_target(const Command *cmd) {
  if (cmd->argc < 2 || cmd->args[1] == NULL) {
    return "~";
  }

  return cmd->args[1];
}

static bool change_directory(const char *path) {
  if (path[0] == '~') {
    char *home = getenv("HOME");
    if (home == NULL) {
      return false;
    }

    char e_path[PATH_MAX];
    snprintf(e_path, sizeof(e_path), "%s%s", home, path + 1);
    if (chdir(e_path) == 0) {
      return true;
    }
  }
  return chdir(path) == 0;
}

static int handle_exit(Command *cmd) {
  (void)cmd;
  exit(0);
}

static int handle_echo(Command *cmd) {
  for (int i = 1; i < cmd->argc; i++) {
    if (i > 1)
      putchar(' ');
    fputs(cmd->args[i], stdout);
  }
  putchar('\n');
  return BUILTIN_HANDLED;
}

static int handle_type(Command *cmd) {
  if (cmd->argc < 2 || cmd->args[1] == NULL) {
    printf(": not found\n");
    return BUILTIN_HANDLED;
  }

  char *type_arg = cmd->args[1];
  if (lookup_builtin(type_arg) != NULL) {
    printf("%s is a shell builtin\n", type_arg);
    return BUILTIN_HANDLED;
  }

  char *path = find_executable_in_path(type_arg);
  if (path != NULL) {
    printf("%s is %s\n", type_arg, path);
    free(path);
    return BUILTIN_HANDLED;
  }

  printf("%s: not found\n", type_arg);
  return BUILTIN_HANDLED;
}
