#include "shell_path.h"

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

static bool is_executable_file(const char *path) {
  struct stat file_stat;

  if (access(path, X_OK) != 0) {
    return false;
  }

  if (stat(path, &file_stat) != 0) {
    return false;
  }

  return S_ISREG(file_stat.st_mode);
}

static char *join_path(const char *directory, const char *command) {
  size_t len = strlen(directory) + strlen(command) + 2;
  char *full_path = malloc(len);
  if (full_path == NULL) {
    perror("malloc");
    exit(1);
  }

  snprintf(full_path, len, "%s/%s", directory, command);
  return full_path;
}

char *find_executable_in_path(const char *command) {
  if (command == NULL || command[0] == '\0') {
    return NULL;
  }

  char *path_env = getenv("PATH");
  if (path_env == NULL) {
    return NULL;
  }

  char *path_copy = strdup(path_env);
  if (path_copy == NULL) {
    perror("strdup");
    exit(1);
  }

  char *save_ptr = NULL;
  for (char *dir = strtok_r(path_copy, PATH_LIST_SEPARATOR, &save_ptr); dir != NULL;
       dir = strtok_r(NULL, PATH_LIST_SEPARATOR, &save_ptr)) {
    char *full_path = join_path(dir, command);
    if (is_executable_file(full_path)) {
      free(path_copy);
      return full_path;
    }

    free(full_path);
  }

  free(path_copy);
  return NULL;
}
