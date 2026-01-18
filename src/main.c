#include <stdio.h>
#include <string.h>

#define INPUT_SIZE 1024

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

int main(void) {
  setbuf(stdout, NULL);
  char input[INPUT_SIZE];
  while (1) {
    printf("$ ");

    if (fgets(input, INPUT_SIZE, stdin) == NULL) {
      break;
    }
    input[strcspn(input, "\n")] = '\0';
    if (strcmp(input, "exit") == 0) {
      break;
    }
    char *command = strtok(input, " ");
    if (command == NULL) {
      continue;
    }
    if (strcmp(command, "echo") == 0) {
      printf("%s\n", input + strlen(command) + 1);
    }
    if (strcmp(command, "type") == 0) {
      char *cmd_args = input + strlen(command) + 1;
      if (is_builtin(cmd_args)) {
        printf("%s is a shell builtin\n", cmd_args);
      } else {
        printf("%s: not found\n", cmd_args);
      }
      continue;
    }
    printf("%s: command not found\n", command);
  }

  return 0;
}
