
#include "builtins.h"
#include "tokenizer.h"
#include <limits.h>
#include <dirent.h>
#include <linux/limits.h>
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

int handle_type(ArgV *argv);
int handle_echo(ArgV *argv);
int handle_exit(ArgV *argv);
int handle_pwd(ArgV *argv);

BuiltinEntry builtins[] = {{"echo", handle_echo, 0},
                           {"exit", handle_exit, 0},
                           {"type", handle_type, 0},
                           {"pwd", handle_pwd, 0},
};
int builtin_len = 4;

BuiltinEntry *lookup(const char *command) {
  for (int i = 0; i < builtin_len; i++) {
    if (command && strcmp(command, builtins[i].name) == 0) {
      return &builtins[i];
    }
  }
  return NULL;
}

int hanlde_builtins(ArgV *argv) {
  BuiltinEntry *entry = lookup(argv->argv[0]);
  if (entry == NULL) {
    return 0;
  }
  return entry->fpptr(argv);
}


int handle_pwd(ArgV *argv) {
  char buff[PATH_MAX];
  if(getcwd(buff, sizeof(buff)) == NULL){
    perror("getcwd");
    return 0;
  }
  printf("%s",buff);
  return 1;
}

int handle_exit(ArgV *argv) {
  (void)argv;
  exit(0);
}

int handle_echo(ArgV *argv) {
  for (int i = 1; i < argv->argc; i++) {
    if (i > 1)
      putchar(' ');
    fputs(argv->argv[i], stdout);
  }
  putchar('\n');
  return 1;
}

int handle_type(ArgV *argv) {
  char *type_arg = argv->argv[1];
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
