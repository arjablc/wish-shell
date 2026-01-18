#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  char input[1024] = "";
  fgets(input, sizeof(char) * 1024, stdin);
  printf("$ %s", input);
    printf("%s: command not found", input);

  return 0;
}
