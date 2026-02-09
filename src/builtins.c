
#include "builtins.h"
#include "tokenizer.h"
#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <linux/limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef _WIN32
#define PATH_LIST_SEPARATOR ";"
#else
#define PATH_LIST_SEPARATOR ":"
#endif

bool change_directory(char *path);

int handle_type(ArgV *argv);
int handle_echo(ArgV *argv);
int handle_exit(ArgV *argv);
int handle_pwd(ArgV *argv);
int handle_cd(ArgV *argv);

BuiltinEntry builtins[] = {
    {"echo", handle_echo, 0}, {"exit", handle_exit, 0},
    {"type", handle_type, 0}, {"pwd", handle_pwd, 0},
    {"cd", handle_cd, 0},
};
int builtin_len = 5;

BuiltinEntry *lookup(const char *command) {
  for (int i = 0; i < builtin_len; i++) {
    if (command && strcmp(command, builtins[i].name) == 0) {
      return &builtins[i];
    }
  }
  return NULL;
}

int hanlde_builtins(ArgV *argv) {
  BuiltinEntry *entry = lookup(argv->args[0]);
  if (entry == NULL) {
    return 0;
  }
  return entry->fpptr(argv);
}

int handle_pwd(ArgV *argv) {
  char buff[PATH_MAX];
  if (getcwd(buff, sizeof(buff)) == NULL) {
    perror("getcwd");
    return 0;
  }
  printf("%s\n", buff);
  return 1;
}

int handle_cd(ArgV *argv) {
  if (argv->args[1] == NULL) {
    argv->args[1] = "~";
  }
  char *path = argv->args[1];
  if (!change_directory(path)) {
    printf("cd: %s: No such file or directory\n", path);
  }
  return 1;
}

bool change_directory(char *path) {
  if (path[0] == '~') {
    // here e_path is a pointer
    char *home = getenv("HOME");
    char e_path[PATH_MAX];
    strcpy(e_path, home);
    // doing strcat on the buffer returned by get env
    // will change the buffer pointer and
    // subsequently it would change the HOME environment variable too
    strcat(e_path, (path + 1));
    if (chdir(e_path) == 0) {
      return true;
    }
  }
  return chdir(path) == 0;
}

int handle_exit(ArgV *argv) {
  (void)argv;
  exit(0);
}

int handle_echo(ArgV *argv) {
  for (int i = 1; i < argv->argc; i++) {
    if (i > 1)
      putchar(' ');
    fputs(argv->args[i], stdout);
  }
  putchar('\n');
  return 1;
}

int handle_type(ArgV *argv) {
  char *type_arg = argv->args[1];
  if (lookup(type_arg) != NULL) {
    printf("%s is a shell builtin\n", type_arg);
    return 1;

  } else {
    int ex_found = 0;
    char *path_variables = getenv("PATH");
    // copy the path variables because they are owned by os
    char *path_copy = strdup(path_variables);
    // strtok takes a pointer to the start of a string, returns the pointer to
    // the first token then if we call strtok on again with NULL it will start
    // from the last saved pointer after the delimeter
    // NOTE: since strtok uses internal static variable to keep track of the
    // last pointer
    // it shouldn't be used in real world
    char *path = strtok(path_copy, PATH_LIST_SEPARATOR);
    // working with directories  in c
    // using dirent header(posix complient)
    DIR *dir;
    while (path != NULL) {
      dir = opendir(path);
      struct dirent *entry;
      if (dir == NULL) {
        path = strtok(NULL, PATH_LIST_SEPARATOR);
        continue;
      }

      while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
          continue;
        }
        // check if same
        if (strcmp(type_arg, entry->d_name) == 0) {
          // build the full path of the file
          char file_path[1024];
          snprintf(file_path, sizeof(file_path), "%s/%s", path, entry->d_name);
          // check if the file is executable and user has access to it
          if (access(file_path, X_OK) == 0) {
            // check if the file is a file and not a directory
            struct stat file_stat;
            stat(file_path, &file_stat);
            if (S_ISREG(file_stat.st_mode)) {
              printf("%s is %s\n", type_arg, file_path);
              ex_found = 1;
            }
          }
        }
      }
      if (ex_found) {
        closedir(dir);
        return 1;
      }
      path = strtok(NULL, PATH_LIST_SEPARATOR);
    }
    free(path_copy);
  }
  printf("%s: not found\n", type_arg);
  return 1;
}
