#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  printf("$ ");
  char input[1024] = "";
  fgets(input, sizeof(char) * 1024, stdin);
  if(input != NULL){
    input[strcspn(input, "\n")] = '\0';
    printf("%s: command not found", input);
  }
  return 0;
}
