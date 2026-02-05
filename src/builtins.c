
#include "tokenizer.h"
#include <dirent.h>
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

void handle_type(char **argv);

int is_builtin(const char *command) {
  static char *builtins[] = {"echo", "exit", "type"};
  int builtins_len = sizeof(builtins) / sizeof(builtins[0]);
  for (int i = 0; i < builtins_len; i++) {
    if (strcmp(command, builtins[i]) == 0) {
      return 1;
    }
  }
  return 0;
}

int hanlde_builtin(ArgV *argv) {
  char *command = argv->argv[0];
  if (strcmp(command, "exit") == 0) {
    exit(0);
  }
  if (strcmp(command, "echo") == 0) {
    for (int i = 1; i < argv->argc; i++) {
      if (i > 1)
        putchar(' ');
      fputs(argv->argv[i], stdout);
    }
    putchar('\n');
    return 1;
  }
  if (strcmp(command, "type") == 0) {
    handle_type(argv->argv);
    return 1;
  }
  return -1;
}

void handle_type(char **argv) {
  char *type_arg = argv[1];
  if (is_builtin(type_arg) == 1) {
    printf("%s is a shell builtin\n", type_arg);
    return;

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
        return;
      }
      path = strtok(NULL, PATH_LIST_SEPARATOR);
    }
    free(path_copy);
  }
  printf("%s: not found\n", type_arg);
}
